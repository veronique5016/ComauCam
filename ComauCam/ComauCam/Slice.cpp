#include "stdafx.h"
#include "Slice.h"
#include <algorithm>
#include <math.h>

CSlice::CSlice(void)
{
	height = 10;
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
	double dz = height;
	double z = z_min + dz; 

	while (z<z_max)
	{
		CSliceLayer* pLayer = new CSliceLayer();
		pLayer->m_ptLayerPoint = CPoint3D(0, 0, z);
		m_vecpLayers.push_back(pLayer);

		int index = m_vecpLayers.size() - 1;//index 指向当前的切平面,这里确定的平面法向量永远为 z 轴正向
		GetBoundaryPoints(m_vecpLayers[index]);

		//将轮廓逆时针排列并删除共线点
		OptimizeBoundary(m_vecpLayers[index]);

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
				double zmin_tmp = ZminofLayer(m_vecpLayers[index]);
				double zmin_prev = ZminofLayer(m_vecpLayers[index - 1]);
				double zaxis_distance = zmin_tmp - zmin_prev;
				double offset = zaxis_distance * tan(GetAngle(CVector3D(0,0,1), CVector3D(*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_pTriangle->n*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec)));
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart = m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart + CVector3D(offset_vec*offset);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd = m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd + CVector3D(offset_vec*offset);
				
				//求偏移后的线段与前后线段的交点,形成新的轮廓
				CPoint3D point_out;
				GetCrossPoint(point_out, m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart, m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd,
					m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i - 1)%szL]->m_ptStart, m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i - 1) % szL]->m_ptEnd);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart = CLPoint(point_out);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i - 1) % szL]->m_ptEnd = CLPoint(point_out);
				GetCrossPoint(point_out, m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart, m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd,
					m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i + 1) % szL]->m_ptStart, m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i + 1) % szL]->m_ptEnd);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd = CLPoint(point_out);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i + 1) % szL]->m_ptStart = CLPoint(point_out);

				double tmpz = m_vecpLayers[index]->m_ptLayerPoint.z;
				//定义变法向切平面的坐标系和平面上一点
				CSliceLayer* pTurnLayer = new CSliceLayer();
				pTurnLayer->m_vLayerCoordinate[0] = CVector3D(*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_pTriangle->n);
				pTurnLayer->m_vLayerCoordinate[1] = CVector3D(m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec);
				pTurnLayer->m_vLayerCoordinate[2] = pTurnLayer->m_vLayerCoordinate[0] * pTurnLayer->m_vLayerCoordinate[1];
				pTurnLayer->m_ptLayerPoint = CPoint3D(m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart);

				m_vecpLayers.push_back(pTurnLayer);

				GetBoundaryPoints(m_vecpLayers[m_vecpLayers.size() - 1]);
				OptimizeBoundary(m_vecpLayers[m_vecpLayers.size() - 1]);

				//对斜切面的轮廓进行调整
				unsigned int szTurn = m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries[0]->m_vecpSegments.size();
				vector<CSegment*> tmp_boundary;
				int turnIndex = 0;
				for (unsigned int j = 0; j < szTurn; j++)
				{
					CPoint3D p1 = m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart;
					CPoint3D p2 = m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd;
					if (CalPointtoLine(m_vecpLayers[m_vecpLayers.size() - 1]->m_ptLayerPoint, p1, p2) != 0.0)
						continue;
					turnIndex = j;
					break;
				}
				int sum = 0;
				while (sum < szTurn)
				{
					tmp_boundary.push_back(new CSegment(*m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries[0]->m_vecpSegments[(turnIndex + sum) % szTurn]));
					sum += 1;
				}
				m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries[0]->m_vecpSegments.clear();
				for (unsigned int j = 0; j < tmp_boundary.size(); j++)
				{
					CPoint3D cross_point;
					if (tmp_boundary[j]->m_ptStart.z<=tmpz &&tmp_boundary[j]->m_ptEnd.z>=tmpz)
					{
						cross_point = CalPlaneLineIntersectPoint(m_vecpLayers[index]->m_vLayerCoordinate[2], m_vecpLayers[index]->m_ptLayerPoint, tmp_boundary[j]->m_vSegmentVec, tmp_boundary[j]->m_ptEnd);
						tmp_boundary[j]->m_ptEnd = CLPoint(cross_point);
						m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(*tmp_boundary[j]));
					}
					else if (tmp_boundary[j]->m_ptStart.z >= tmpz &&tmp_boundary[j]->m_ptEnd.z >= tmpz)
					{ 
						//舍弃整条线段均在 z 向切平面之上的线段
					}
					else if (tmp_boundary[j]->m_ptStart.z >= tmpz &&tmp_boundary[j]->m_ptEnd.z <= tmpz)
					{
						cross_point = CalPlaneLineIntersectPoint(m_vecpLayers[index]->m_vLayerCoordinate[2], m_vecpLayers[index]->m_ptLayerPoint, tmp_boundary[j]->m_vSegmentVec, tmp_boundary[j]->m_ptEnd);
						tmp_boundary[j]->m_ptStart = CLPoint(cross_point);
						m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(*tmp_boundary[j]));
					}
					else
					{
						m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(*tmp_boundary[j]));
					}
				}
				tmp_boundary.clear();
			}
		}
		z += dz;
	}
	int test = 0;
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
		CLPoint* tmpstartPoint = new CLPoint();
		CLPoint* tmpendPoint = new CLPoint();
		//Segment* tmpSegment = new Segment();

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
				m_vecpLayers[m_vecpLayers.size() - 1]->m_vecpBoundaries.push_back(m_boundary);
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
	CPoint3D v1_tmp, v2_tmp, v3_tmp;

	v1_tmp.x = pCurFace->v1->x;
	v1_tmp.y = pCurFace->v1->y;
	v1_tmp.z = pCurFace->v1->z;
	
	v2_tmp.x = pCurFace->v2->x;
	v2_tmp.y = pCurFace->v2->y;
	v2_tmp.z = pCurFace->v2->z;
	
	v3_tmp.z = pCurFace->v3->z;
	v3_tmp.x = pCurFace->v3->x;
	v3_tmp.y = pCurFace->v3->y;

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

	min_dist = CompareThreeNumber(v1_dist, v2_dist, v3_dist, MIN);
	mid_dist = CompareThreeNumber(v1_dist, v2_dist, v3_dist, MID);
	max_dist = CompareThreeNumber(v1_dist, v2_dist, v3_dist, MAX);

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


void CSlice::CalIntersectPoint(CSliceLayer* layer, CLEdge * edge, CLTriangle*pCurFace, CLPoint* point)
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

double CSlice::ZminofLayer(CSliceLayer * layer)
{
	double zmin;
	zmin = layer->m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptStart.z;
	unsigned int sz = layer->m_vecpBoundaries[0]->m_vecpSegments.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		if (zmin > layer->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd.z)
			zmin = layer->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd.z;
	}
	return zmin;
}

void CSlice::OptimizeBoundary(CSliceLayer* layer)
{
	bool isCCW = IsBoundaryCCW(layer);
	if (!isCCW)
		MakeBoundaryCCW(layer);
	isCCW = IsBoundaryCCW(layer);
	DeletePoints(layer);
}

bool CSlice::IsBoundaryCCW(CSliceLayer* layer)
{
	unsigned int sz = layer->m_vecpBoundaries[0]->m_vecpSegments.size();
	double angle = -1.0;
	CPoint3D p1, p2;
	CVector3D vec1, vec2;
	CVector3D vec = CVector3D(0, 0, 0);
	if (sz < 3)
	{
		return false;
	}
	else
	{
		p1 = CPoint3D(layer->m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptStart);
		p2 = CPoint3D(layer->m_vecpBoundaries[0]->m_vecpSegments[sz/3]->m_ptStart);
		vec1 = CVector3D(p1, p2);
		p1 = CPoint3D(layer->m_vecpBoundaries[0]->m_vecpSegments[sz/3]->m_ptStart);
		p2 = CPoint3D(layer->m_vecpBoundaries[0]->m_vecpSegments[(sz/3)*2]->m_ptStart);
		vec2 = CVector3D(p1, p2);
		vec = vec1 * vec2;
		vec.Normalize();

		angle = GetAngle(vec, layer->m_vLayerCoordinate[2]);
		if(angle == 0.0)
			return true;
		else
			return false;
	}
}

void CSlice::MakeBoundaryCCW(CSliceLayer* layer)
{
	unsigned int sz = layer->m_vecpBoundaries[0]->m_vecpSegments.size();
	vector<CSegment*> tmp;
	for (unsigned int i = 0; i < sz; i++)
	{
		tmp.push_back(layer->m_vecpBoundaries[0]->m_vecpSegments[sz - 1 - i]);
	}
	layer->m_vecpBoundaries[0]->m_vecpSegments.clear();
	for (unsigned int i = 0; i < sz; i++)
	{
		layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(*tmp[i]));
		layer->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart = CLPoint(tmp[i]->m_ptEnd);
		layer->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd = CLPoint(tmp[i]->m_ptStart);
		layer->m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec = CVector3D(CPoint3D(layer->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart), CPoint3D(layer->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd));
		layer->m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec.Normalize();
	}
}

void CSlice::DeletePoints(CSliceLayer* layer)
{
	vector<CSegment*> tmp_segments;
	unsigned int szP = layer->m_vecpBoundaries[0]->m_vecpSegments.size();

	// 取出单个层切面轮廓中的所有线段
	for (unsigned int j = 0; j < szP; j++)
	{
		tmp_segments.push_back(layer->m_vecpBoundaries[0]->m_vecpSegments[j]);
	}
	layer->m_vecpBoundaries[0]->m_vecpSegments.clear();

	unsigned int szTemp = tmp_segments.size();
	for (unsigned int i = 0; i < szTemp; i++)
	{
		if (fabs(tmp_segments[i]->m_ptStart.x) <= 0.0000001)
			tmp_segments[i]->m_ptStart.x = 0;
		if (fabs(tmp_segments[i]->m_ptStart.y) <= 0.0000001)
			tmp_segments[i]->m_ptStart.y = 0;
		if (fabs(tmp_segments[i]->m_ptStart.z) <= 0.0000001)
			tmp_segments[i]->m_ptStart.z = 0;
		if (fabs(tmp_segments[i]->m_ptEnd.x) <= 0.0000001)
			tmp_segments[i]->m_ptEnd.x = 0;
		if (fabs(tmp_segments[i]->m_ptEnd.y) <= 0.0000001)
			tmp_segments[i]->m_ptEnd.y = 0;
		if (fabs(tmp_segments[i]->m_ptEnd.z) <= 0.0000001)
			tmp_segments[i]->m_ptEnd.z = 0;
	}

	CPoint3D pCur, pNext, pNextNext;
	while (szTemp >= 3)
	{
		for (unsigned int j = 0; j < szTemp; j++)
		{
			pCur = tmp_segments[j%szTemp]->m_ptStart;
			pNext = tmp_segments[j%szTemp]->m_ptEnd;
			pNextNext = tmp_segments[(j + 1) % szTemp]->m_ptEnd;
			double distance = CalPointtoLine(pNext, pCur, pNextNext);
			if (distance <= 0.0001)
			{
				tmp_segments[j%szTemp]->m_ptEnd = tmp_segments[(j + 1) % szTemp]->m_ptEnd;
				CPoint3D p1 = CPoint3D(tmp_segments[j%szTemp]->m_ptStart);
				CPoint3D p2 = CPoint3D(tmp_segments[j%szTemp]->m_ptEnd);
				tmp_segments[j%szTemp]->m_vSegmentVec = CVector3D(p1, p2);
				tmp_segments[j%szTemp]->m_vSegmentVec.Normalize();
				tmp_segments.erase(tmp_segments.begin() + (j + 1) % szTemp);
				szTemp = tmp_segments.size();
				j = -1;
			}
		}
		break;
	}

	//将删除共线点之后的线段存回 m_vecpSegments 中
	for (unsigned int i = 0; i < tmp_segments.size(); i++)
	{
		layer->m_vecpBoundaries[0]->m_vecpSegments.push_back(tmp_segments[i]);
	}
}

double CSlice::CompareThreeNumber(double v1, double v2, double v3, int type)
{
	double z1, z2, z3;
	z1 = v1;
	z2 = v2;
	z3 = v3;
	if (z1>z2) swap(z1, z2);
	if (z2>z3) swap(z2, z3);
	if (z1>z2) swap(z1, z2);
	if (type == 1) 
		return z1;	
	else if (type == 2) 
		return z2;
	else 
		return z3;
}

void CSlice::DrawLayer(bool showPolygon, int start, int end)
{
	double color[3];
	unsigned int szlayer = m_vecpLayers.size();
	CLPoint point, point2;
	for (int i = (start - 1); i<end; i++)
	{
		unsigned int szpolyline = m_vecpLayers[i]->m_vecpBoundaries.size();
		if (m_vecpLayers[i]->m_vLayerCoordinate[2].dz == 1.0)
		{
			color[0] = 0.0;
			color[1] = 1.0;
			color[2] = 0.0;
		}
		else
		{
			color[0] = 1.0;
			color[1] = 0.0;
			color[2] = 0.0;
		}
		for (unsigned int j = 0; j < szpolyline; j++)
		{
			unsigned int szLine = m_vecpLayers[i]->m_vecpBoundaries[j]->m_vecpSegments.size();
			glLineWidth(1.5f);

			//绘制平面轮廓
			if (showPolygon)
			{
				glBegin(GL_POLYGON);
				glColor3f(color[0], color[1], color[2]);
				for (unsigned int k = 0; k < szLine; k++)
				{
					point = m_vecpLayers[i]->m_vecpBoundaries[j]->m_vecpSegments[k]->m_ptStart;
					glVertex3f(point.x, point.y, point.z);
					point = m_vecpLayers[i]->m_vecpBoundaries[j]->m_vecpSegments[k]->m_ptEnd;
					glVertex3f(point.x, point.y, point.z);
				}
				glEnd();
			}

			//绘制线框轮廓
			glColor3f(color[0], color[1], color[2]);
			for(unsigned int k = 0; k<szLine; k++)
			{
				glBegin(GL_LINE_LOOP);
				point = m_vecpLayers[i]->m_vecpBoundaries[j]->m_vecpSegments[k]->m_ptStart;
				glVertex3f(point.x, point.y, point.z);
				point = m_vecpLayers[i]->m_vecpBoundaries[j]->m_vecpSegments[k]->m_ptEnd;
				glVertex3f(point.x, point.y, point.z);
				glEnd();
			}
		}
	}
}
