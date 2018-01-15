#include "stdafx.h"
#include "Slice.h"
#include <algorithm>

CSlice::CSlice(void)
{
	m_dHeight = 5;
}

CSlice::~CSlice(void)
{
	unsigned int szS = m_vecpSliceEdge.size();
	for (unsigned int i = 0; i < szS; i++)
	{
		delete m_vecpSliceEdge[i];
		m_vecpSliceEdge[i] = NULL;
	}
	m_vecpSliceEdge.clear();

	unsigned int szL = m_vecpLayers.size();
	for (unsigned int i = 0; i < szL; i++)
	{
		delete m_vecpLayers[i];
		m_vecpLayers[i] = NULL;
	}
	m_vecpLayers.clear();

	unsigned int sz = m_vecpSliceTris.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		delete m_vecpSliceTris[i];
		m_vecpSliceTris[i] = NULL;
	}
	m_vecpSliceTris.clear();
}

void CSlice::LoadSTLModel(CSTLModel* model)//载入stl模型
{
	model->Topologize();//给模型建立拓扑结构
	unsigned int szTri = model->m_vecpLTris.size();
	for (unsigned int j = 0; j<szTri; j++)
	{
		m_vecpSliceTris.push_back(model->m_vecpLTris[j]);     //为CSlice对象拷贝一份三角面片数据
	}
}

void CSlice::Slice(CSTLModel* model)
{
	double model_ex[6];
	model->FindExtreme(model_ex);
	double z_min = model_ex[4];
	double z_max = model_ex[5];
	double dz = m_dHeight;
	double z = z_min + dz; 

	while (z<z_max)
	{
		CSliceLayer* pLayer = new CSliceLayer();
		pLayer->m_ptLayerPoint = CPoint3D(0, 0, z);
		m_vecpLayers.push_back(pLayer);

		int index = m_vecpLayers.size() - 1;//index 指向当前的切平面,这里确定的平面法向量永远为 z 轴正向
		GetBoundaryPoints(m_vecpLayers[index]);

		//将轮廓逆时针排列并删除共线点
		m_vecpLayers[index]->OptimizeBoundary();
		m_vecpLayers[index]->RearrangeBoundary();

		//判断是否需要变法向切片
		unsigned int szL = m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments.size();		
		for (unsigned int i = 0; i < szL; i++)
		{
			double angle = GetAngle(CVector3D(0, 0, 1), CVector3D(*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_pTriangle->n))*180.0 / PI;
			if (angle > 128.0)
			{
				//偏移该线段
				CVector3D offset_vec = m_vecpLayers[index]->m_vLayerCoordinate[2] * m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec;
				offset_vec.Normalize();
				//此段有 bug 存在，当一个层切面有多条线段需要变法向切且法向量不同时，zaxis_distance 应该不同，但此处相同了
				double zmin_tmp = m_vecpLayers[index]->ZminofLayer();
				double zmin_prev = m_vecpLayers[index - 1]->ZminofLayer();
				double zaxis_distance = zmin_tmp - zmin_prev;
				double offset = zaxis_distance * tan(GetAngle(CVector3D(0,0,1), CVector3D(*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_pTriangle->n*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec)));
				MoveSegment(m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i], CVector3D(offset_vec*offset));

				//求偏移后的线段与前后线段的交点,形成新的轮廓
				CPoint3D point_out;
				GetCrossPoint(point_out, *m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i], *m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i - 1) % szL]);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart = CPoint3D(point_out);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i - 1) % szL]->m_ptEnd = CPoint3D(point_out);
				GetCrossPoint(point_out, *m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i], *m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i + 1) % szL]);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd = CPoint3D(point_out);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i + 1) % szL]->m_ptStart = CPoint3D(point_out);

				//定义变法向切平面的坐标系和平面上一点
				CSliceLayer* pTurnLayer = new CSliceLayer();
				pTurnLayer->m_vLayerCoordinate[0] = CVector3D(*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_pTriangle->n);
				pTurnLayer->m_vLayerCoordinate[1] = CVector3D(m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec);
				pTurnLayer->m_vLayerCoordinate[2] = pTurnLayer->m_vLayerCoordinate[0] * pTurnLayer->m_vLayerCoordinate[1];
				
				CPoint3D layer_point = CPoint3D(m_vecpLayers[index - 1]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart);
				layer_point = layer_point + pTurnLayer->m_vLayerCoordinate[2] * dz;
				pTurnLayer->m_ptLayerPoint = layer_point;


				GetBoundaryPoints(pTurnLayer);
				pTurnLayer->OptimizeBoundary();

				ModifyTurnLayer(pTurnLayer);
				
				m_vecpLayers.push_back(pTurnLayer);

				CSliceLayer* pAddedLayer = new CSliceLayer();
				GetAddedLayerBoundary(pAddedLayer, pTurnLayer, m_vecpLayers[index]);
				pTurnLayer->m_pAddictedLayer = pAddedLayer;

				double layer_dist = m_vecpLayers[index]->m_ptLayerPoint.z - pTurnLayer->m_vecpBoundaries[0]->m_vecpSegments[2]->m_ptStart.z;
				if (layer_dist >= dz / 2)
				{
					CSliceLayer* pAddedTurnLayer = new CSliceLayer();
					pAddedTurnLayer->m_vLayerCoordinate[0] = pTurnLayer->m_vLayerCoordinate[0];
					pAddedTurnLayer->m_vLayerCoordinate[1] = pTurnLayer->m_vLayerCoordinate[1];
					pAddedTurnLayer->m_vLayerCoordinate[2] = pTurnLayer->m_vLayerCoordinate[2];
					CPoint3D added_layer_point = CPoint3D(m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptStart);
					added_layer_point = added_layer_point + pAddedTurnLayer->m_vLayerCoordinate[2] * dz;
					pAddedTurnLayer->m_ptLayerPoint = added_layer_point;
					m_vecpLayers.push_back(pAddedTurnLayer);
					GetBoundaryPoints(pAddedTurnLayer);
					pAddedTurnLayer->OptimizeBoundary();
					ModifyTurnLayer(pAddedTurnLayer);

					CSliceLayer* pAddedToAddedLayer = new CSliceLayer();
					GetAddedLayerBoundary(pAddedToAddedLayer, pAddedTurnLayer, m_vecpLayers[index]);
					pAddedTurnLayer->m_pAddictedLayer = pAddedToAddedLayer;
				}
			}
		}
		z += dz;
	}
}

void CSlice::GetBoundaryPoints(CSliceLayer* layer)
{
	// 筛选相交面片
	vector<CLTriangle*> status;
	double v1_dist, v2_dist, v3_dist;

	unsigned int szTri = m_vecpSliceTris.size();
	for (unsigned int i = 0; i < szTri; i++)
	{
		v1_dist = ::CalPointtoPlane(*m_vecpSliceTris[i]->v1, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);
		v2_dist = ::CalPointtoPlane(*m_vecpSliceTris[i]->v2, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);
		v3_dist = ::CalPointtoPlane(*m_vecpSliceTris[i]->v3, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);

		if(!((v1_dist<0&&v2_dist<0&&v3_dist<0)||(v1_dist>0&&v2_dist>0&&v3_dist>0)))
		{
			if ((v1_dist == 0) &&
				(v2_dist == 0) &&
				(v3_dist == 0))
			{
				//跳过面片与切片重合的面片
			}
			else
				status.push_back(m_vecpSliceTris[i]);
		}
	}

	// 对相交面片进行面片分类
	unsigned int szStatus = status.size();
	for (unsigned int i = 0; i<szStatus; i++)    
	{
		JudgeFaceType(layer, status[i]);
	}

	// 求切片平面与三角面片交点
	CLTriangle* pCurFace = new CLTriangle();

	//选第一个面片作为起始面片
	pCurFace = status[0];
	pCurFace->m_bUse = true;
	
	while (true)
	{
		CBoundary* m_boundary = new CBoundary();
		CPoint3D* tmpstartPoint = new CPoint3D();
		CPoint3D* tmpendPoint = new CPoint3D();

		//设定起始相交边，并存入m_Slice_edge容器	
		GetInterSectEdge(layer, pCurFace);  
		unsigned int sz0 = m_vecpSliceEdge.size();
		pCurFace->m_pIntersectLine1 = m_vecpSliceEdge[sz0 - 1];

		//由起始相交边求交点，并存入m_LinkPoint容器
		CalIntersectPoint(layer, pCurFace->m_pIntersectLine1, pCurFace, tmpstartPoint);

		//第一个面片的另一相交边求交点，并存入m_LinkPoint容器
		JudgeOtherLine(layer, pCurFace);
		CalIntersectPoint(layer, pCurFace->m_pIntersectLine2, pCurFace, tmpendPoint);

		CSegment* tmpSegment = new CSegment(*tmpstartPoint, *tmpendPoint, pCurFace);
		m_boundary->m_vecpSegments.push_back(new CSegment(*tmpSegment));
		//上面是轮廓第一个面片的特殊处理

		while (true)
		{
			//由最后存入相交边数组的边的伙伴半边寻找下一相交面片
			unsigned int sz = m_vecpSliceEdge.size();
			pCurFace = m_vecpSliceEdge[sz - 1]->e_adja->t;
			pCurFace->m_bUse = true;//把面片置为当前使用面片，标志位置为真
								 
			pCurFace->m_pIntersectLine1 = m_vecpSliceEdge[sz - 1]->e_adja;
			//判断当前面片中的另一相交边，保存边
			JudgeOtherLine(layer, pCurFace);
			CalIntersectPoint(layer, pCurFace->m_pIntersectLine2, pCurFace, tmpendPoint);
			*tmpSegment = CSegment(m_boundary->m_vecpSegments[m_boundary->m_vecpSegments.size() - 1]->m_ptEnd, *tmpendPoint, pCurFace);

			m_boundary->m_vecpSegments.push_back(new CSegment(*tmpSegment));

			unsigned int szlinkline = m_boundary->m_vecpSegments.size();

			//判断轮廓是否闭合
			if ((m_boundary->m_vecpSegments[0]->m_ptStart) ^= (m_boundary->m_vecpSegments[szlinkline - 1]->m_ptEnd))
			{
				layer->m_vecpBoundaries.push_back(m_boundary);
				break;
			}
		}

		//跳出内层循环来到这，判断status中是否还有未被使用的面片，
		//如有，把面片置为当前，开始下一轮廓寻点；如没有，跳出外层循环
		break;
	}
}

void CSlice::GetInterSectEdge(CSliceLayer* layer, CLTriangle* pCurFace)   //只要找到一条相交的边就可以跳出
{
	int v1_plane, v2_plane, v3_plane;

	CPoint3D v1_tmp = CPoint3D(*pCurFace->v1);
	CPoint3D v2_tmp = CPoint3D(*pCurFace->v2);
	CPoint3D v3_tmp = CPoint3D(*pCurFace->v3);

	v1_plane = PointtoPlane(v1_tmp, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);
	v2_plane = PointtoPlane(v2_tmp, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);
	v3_plane = PointtoPlane(v3_tmp, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);

	switch (pCurFace->m_nFaceType)
	{
	case NO_POINT_ON_SURFACE:   //不相交的边不能选
	{
		if ((v1_plane*v2_plane)<0)   //说明e1边可以作为第一条相交的边
		{
			m_vecpSliceEdge.push_back(pCurFace->e1);
			goto ENDINIT1;
		}
		if ((v2_plane*v3_plane)<0)   //说明e2边可以作为第一条相交的边,第一条边不相交，第二条边肯定相交
		{
			m_vecpSliceEdge.push_back(pCurFace->e2);
			goto ENDINIT1;
		}
	ENDINIT1:
		break;
	}
	case ONE_POINT_ON_SURFACE:  //因为三条边都相交，随便一条都可以作为起始边，选e1好了
	{
		m_vecpSliceEdge.push_back(pCurFace->e1);
		break;
	}
	case EDGE_ON_SURFACE:  //不能选在切平面上的边
	{
		if (v1_plane != 0)   //v1不在切平面上，与之相连的两条边e1\e3都可选
		{
			m_vecpSliceEdge.push_back(pCurFace->e1);
			goto ENDINIT2;
		}
		if (v2_plane != 0)   //v2不在切平面上，与之相连的两条边e1\e2都可选
		{
			m_vecpSliceEdge.push_back(pCurFace->e2);
			goto ENDINIT2;
		}
		if (v3_plane != 0)   //v3不在切平面上，与之相连的两条边e2\e3都可选
		{
			m_vecpSliceEdge.push_back(pCurFace->e3);
			goto ENDINIT2;
		}
	ENDINIT2:
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:
	{
		if (v1_plane == 0)   //v1在切平面上，与之相连的两条边e1\e3都可选
		{
			m_vecpSliceEdge.push_back(pCurFace->e1);
			goto ENDINIT3;
		}
		if (v2_plane == 0)   //v2在切平面上，与之相连的两条边e1\e2都可选
		{
			m_vecpSliceEdge.push_back(pCurFace->e2);
			goto ENDINIT3;
		}
		if (v3_plane == 0)   //v3在切平面上，与之相连的两条边e2\e3都可选
		{
			m_vecpSliceEdge.push_back(pCurFace->e3);
			goto ENDINIT3;
		}
	ENDINIT3:
		break;
	}
	}
}

void CSlice::JudgeFaceType(CSliceLayer* layer, CLTriangle* pCurFace)
{
	double min_dist, mid_dist, max_dist, v1_dist, v2_dist, v3_dist;
	v1_dist = CalPointtoPlane(*pCurFace->v1, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);
	v2_dist = CalPointtoPlane(*pCurFace->v2, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);
	v3_dist = CalPointtoPlane(*pCurFace->v3, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);

	min_dist = ::CompareThreeNumber(v1_dist, v2_dist, v3_dist, MIN);
	mid_dist = ::CompareThreeNumber(v1_dist, v2_dist, v3_dist, MID);
	max_dist = ::CompareThreeNumber(v1_dist, v2_dist, v3_dist, MAX);

	//判断三角面片和切平面的相交情况,四种情况
	if ((min_dist < 0) && (max_dist > 0) && (mid_dist != 0))
		pCurFace->m_nFaceType = NO_POINT_ON_SURFACE;
	if ((min_dist < 0) && (max_dist > 0) && (mid_dist == 0))
		pCurFace->m_nFaceType = ONE_POINT_ON_SURFACE;
	if (((min_dist < 0) && (max_dist == 0) && (mid_dist == 0)) || 
		((min_dist == 0) && (max_dist > 0) && (mid_dist == 0)))
		pCurFace->m_nFaceType = EDGE_ON_SURFACE;
	if ((((min_dist < 0) && (max_dist == 0) && (mid_dist != 0))) || 
		(((min_dist == 0) && (max_dist > 0) && (mid_dist != 0))))
		pCurFace->m_nFaceType = ONLY_ONE_POINT_ON_SURFACE;
}

void CSlice::JudgeOtherLine(CSliceLayer* layer, CLTriangle* pCurFace)
{
	int v1_plane, v2_plane;
	CPoint3D v1_tmp, v2_tmp;

	v1_tmp.x = pCurFace->m_pIntersectLine1->e_prev->v1->x;
	v1_tmp.y = pCurFace->m_pIntersectLine1->e_prev->v1->y;
	v1_tmp.z = pCurFace->m_pIntersectLine1->e_prev->v1->z;

	v2_tmp.x = pCurFace->m_pIntersectLine1->e_prev->v2->x;
	v2_tmp.y = pCurFace->m_pIntersectLine1->e_prev->v2->y;
	v2_tmp.z = pCurFace->m_pIntersectLine1->e_prev->v2->z;

	v1_plane = PointtoPlane(v1_tmp, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);
	v2_plane = PointtoPlane(v2_tmp, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);

	switch (pCurFace->m_nFaceType)
	{
	case NO_POINT_ON_SURFACE:
	{
		//已选的相交边的相邻前一边判断是否相交
		if ((v1_plane * v2_plane)<0)
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
		else//否则另一条就是相交边
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_next;
		break;
	}
	case ONE_POINT_ON_SURFACE:  //这种情况比较特殊，要先判断已选的相交边是哪种类型
	{
		//已选边的一端点在切平面上
		if ((v1_plane*v2_plane) == 0)
		{
			//已选的相交边的相邻前一边判断是否相交
			if ((v1_plane*v2_plane)<0)
				pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
			else//否则另一条就是相交边
				pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_next;
		}
		else//已选边的一端点不在切平面上，/任意一条相邻的边都可以作为另一相交边
		{
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
		}
		break;
	}
	case EDGE_ON_SURFACE:  //相交不能选在切平面上的边
	{
		//已选的相交边的相邻前一边判断是否有一个点不在z切平面上
		if ((v1_plane != 0) || (v2_plane != 0))
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
		else//否则另一条就是相交边
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_next;
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:  //哪一条边其中的一个端点在切平面上，就是相交边
	{
		//已选的相交边的相邻前一边判断是否有一个点不在z切平面上
		if (v1_plane == 0)
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
		else//否则另一条就是相交边
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_next;
		break;
	}
	}
	m_vecpSliceEdge.push_back(pCurFace->m_pIntersectLine2);
}


void CSlice::CalIntersectPoint(CSliceLayer* layer, CLEdge * edge, CLTriangle*pCurFace, CPoint3D* point)
{
	int e1_plane, e2_plane;
	CPoint3D e1_tmp, e2_tmp;

	e1_tmp.x = edge->v1->x;
	e1_tmp.y = edge->v1->y;
	e1_tmp.z = edge->v1->z;

	e2_tmp.x = edge->v2->x;
	e2_tmp.y = edge->v2->y;
	e2_tmp.z = edge->v2->z;

	e1_plane = PointtoPlane(e1_tmp, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);
	e2_plane = PointtoPlane(e2_tmp, layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint);

	if (pCurFace->m_nFaceType != ONLY_ONE_POINT_ON_SURFACE) {
		if (e1_plane==0)   //如果点和切面重合，把端点赋值给交点
		{
			point->x = edge->v1->x;
			point->y = edge->v1->y;
			point->z = edge->v1->z;
		}
		else if (e2_plane==0)
		{
			point->x = edge->v2->x;
			point->y = edge->v2->y;
			point->z = edge->v2->z;
		}
		else    //两端点都不在切片面上
		{
			CPoint3D p = CPoint3D();
			CVector3D vec = CVector3D(e1_tmp, e2_tmp);

			p = ::CalPlaneLineIntersectPoint(layer->m_vLayerCoordinate[2], layer->m_ptLayerPoint, vec, e1_tmp);

			point->x = p.x;
			point->y = p.y;
			point->z = p.z;
		}
	}
}

void CSlice::GetAddedLayerBoundary(CSliceLayer * layer, CSegment turnlayer_seg, CSegment layer_seg)
{
	CSegment tmp_seg = CSegment(turnlayer_seg.m_ptEnd, turnlayer_seg.m_ptStart, NULL);
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(tmp_seg));
	tmp_seg = CSegment(turnlayer_seg.m_ptStart, layer_seg.m_ptEnd, NULL);
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(tmp_seg));
	tmp_seg = CSegment(layer_seg.m_ptEnd, layer_seg.m_ptStart, NULL);
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(tmp_seg));
	tmp_seg = CSegment(layer_seg.m_ptStart, turnlayer_seg.m_ptEnd, NULL);
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(tmp_seg));
}

void CSlice::GetAddedLayerBoundary(CSliceLayer * layer, CSliceLayer * turn_layer, CSliceLayer * z_layer)
{
	layer->m_vLayerCoordinate[1] = turn_layer->m_vLayerCoordinate[1];
	CVector3D vec1 = turn_layer->m_vecpBoundaries[0]->m_vecpSegments[2]->m_vSegmentVec;
	CVector3D vec2 = CVector3D(z_layer->m_vecpBoundaries[0]->m_vecpSegments[1]->m_ptEnd, turn_layer->m_vecpBoundaries[0]->m_vecpSegments[2]->m_ptStart);
	layer->m_vLayerCoordinate[2] = vec2*vec1;
	layer->m_vLayerCoordinate[2].Normalize();
	layer->m_vLayerCoordinate[0] = turn_layer->m_vLayerCoordinate[1] * turn_layer->m_vLayerCoordinate[2];
	layer->m_vLayerCoordinate[0].Normalize();
	layer->m_ptLayerPoint = turn_layer->m_vecpBoundaries[0]->m_vecpSegments[2]->m_ptStart;
	
	layer->m_vecpBoundaries.push_back(new CBoundary());
	CSegment tmp_seg = CSegment(turn_layer->m_vecpBoundaries[0]->m_vecpSegments[2]->m_ptEnd, turn_layer->m_vecpBoundaries[0]->m_vecpSegments[2]->m_ptStart, NULL);
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(tmp_seg));
	tmp_seg = CSegment(turn_layer->m_vecpBoundaries[0]->m_vecpSegments[2]->m_ptStart, z_layer->m_vecpBoundaries[0]->m_vecpSegments[1]->m_ptEnd, NULL);
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(tmp_seg));
	tmp_seg = CSegment(z_layer->m_vecpBoundaries[0]->m_vecpSegments[1]->m_ptEnd, z_layer->m_vecpBoundaries[0]->m_vecpSegments[1]->m_ptStart, NULL);
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(tmp_seg));
	tmp_seg = CSegment(z_layer->m_vecpBoundaries[0]->m_vecpSegments[1]->m_ptStart, turn_layer->m_vecpBoundaries[0]->m_vecpSegments[2]->m_ptEnd, NULL);
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(tmp_seg));
}

void CSlice::ModifyTurnLayer(CSliceLayer* layer)
{
	vector<CSegment*> tmp_boundary;
	for (unsigned int i = 0; i < layer->m_vecpBoundaries[0]->m_vecpSegments.size(); i++)
	{
		tmp_boundary.push_back(layer->m_vecpBoundaries[0]->m_vecpSegments[i]);
	}

	int seg_index = layer->FindLowestSegment();
	CSegment* start_seg = new CSegment(*layer->m_vecpBoundaries[0]->m_vecpSegments[seg_index]);
	layer->m_vecpBoundaries[0]->m_vecpSegments.clear();

	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(start_seg);
	CPoint3D intersect;
	//确定变法向切平面的宽度
	MoveSegment(tmp_boundary[seg_index], layer->m_vLayerCoordinate[0] * (-2));
	CPoint3D tmp_swap = tmp_boundary[seg_index]->m_ptStart;
	tmp_boundary[seg_index]->m_ptStart = tmp_boundary[seg_index]->m_ptEnd;
	tmp_boundary[seg_index]->m_ptEnd = tmp_swap;
	tmp_boundary[seg_index]->m_vSegmentVec = CVector3D(0, 0, 0) - tmp_boundary[seg_index]->m_vSegmentVec;

	GetCrossPoint(intersect, *tmp_boundary[(seg_index + 1) % tmp_boundary.size()], *tmp_boundary[seg_index]);
	tmp_boundary[(seg_index + 1) % tmp_boundary.size()]->m_ptEnd = intersect;
	tmp_boundary[seg_index]->m_ptStart = intersect;
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(tmp_boundary[(seg_index + 1) % tmp_boundary.size()]);
	GetCrossPoint(intersect, *tmp_boundary[seg_index], *tmp_boundary[(seg_index - 1 + tmp_boundary.size()) % tmp_boundary.size()]);
	tmp_boundary[seg_index]->m_ptEnd = intersect;
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(tmp_boundary[seg_index]);
	tmp_boundary[(seg_index - 1 + tmp_boundary.size()) % tmp_boundary.size()]->m_ptStart = intersect;
	layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(tmp_boundary[(seg_index - 1 + tmp_boundary.size()) % tmp_boundary.size()]);
	tmp_boundary.clear();
}
