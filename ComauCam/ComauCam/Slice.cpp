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
	unsigned int szS = m_Slice_edge.size();
	for (unsigned int i = 0; i < szS; i++)
	{
		delete m_Slice_edge[i];
		m_Slice_edge[i] = NULL;
	}
	m_Slice_edge.clear();

	unsigned int szL = m_layers.size();
	for (unsigned int i = 0; i < szL; i++)
	{
		delete m_layers[i];
		m_layers[i] = NULL;
	}
	m_layers.clear();

	unsigned int sz = m_tris_slice.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		delete m_tris_slice[i];
		m_tris_slice[i] = NULL;
	}
	m_tris_slice.clear();
}

void CSlice::loadSTLModel(CSTLModel* model)//载入stl模型
{
	model->Topologize();//给模型建立拓扑结构
	unsigned int szTri = model->m_ltris.size();
	for (unsigned int j = 0; j<szTri; j++)
	{
		m_tris_slice.push_back(model->m_ltris[j]);     //为CSlice对象拷贝一份三角面片数据
	}
}

void CSlice::slice(CSTLModel* model)
{
	double model_ex[6];
	model->FindExtreme(model_ex);
	double z_min = model_ex[4];
	double z_max = model_ex[5];
	double dz = height;
	double z = z_min + dz;

	while (z<z_max)
	{
		SliceLayer* tmp_layer = new SliceLayer();
		tmp_layer->layerPoint = CPoint3D(0, 0, z);
		m_layers.push_back(tmp_layer);

		int index = m_layers.size() - 1;//index 指向当前的切平面
		getBoundaryPoints(m_layers[index]);

		//将轮廓逆时针排列并删除共线点
		optimizeBoundary(m_layers[index]);

		//判断是否需要变法向切片
		unsigned int szL = m_layers[index]->m_Boundaries[0]->m_segments.size();		
		for (unsigned int i = 0; i < szL; i++)
		{
			double angle = GetAngle(CVector3D(0, 0, 1), CVector3D(*m_layers[index]->m_Boundaries[0]->m_segments[i]->triangle->n))*180.0 / PI;
			if (angle > 128.0)
			{
				//偏移该线段
				CVector3D offset_vec = m_layers[index]->layer_coordinate[2] * m_layers[index]->m_Boundaries[0]->m_segments[i]->segment_vec;
				offset_vec.Normalize();
				double zmin_tmp = zminofLayer(m_layers[index]);
				double zmin_prev = zminofLayer(m_layers[index - 1]);
				double zaxis_distance = zmin_tmp - zmin_prev;
				double offset = zaxis_distance * tan(GetAngle(CVector3D(0,0,1), CVector3D(*m_layers[index]->m_Boundaries[0]->m_segments[i]->triangle->n*m_layers[index]->m_Boundaries[0]->m_segments[i]->segment_vec)));
				m_layers[index]->m_Boundaries[0]->m_segments[i]->pstart = m_layers[index]->m_Boundaries[0]->m_segments[i]->pstart + CVector3D(offset_vec*offset);
				m_layers[index]->m_Boundaries[0]->m_segments[i]->pend = m_layers[index]->m_Boundaries[0]->m_segments[i]->pend + CVector3D(offset_vec*offset);
				
				//定义变法向切平面的坐标系和平面上一点
				SliceLayer* turn_layer = new SliceLayer();
				turn_layer->layer_coordinate[0] = CVector3D(*m_layers[index]->m_Boundaries[0]->m_segments[i]->triangle->n);
				turn_layer->layer_coordinate[1] = CVector3D(m_layers[index]->m_Boundaries[0]->m_segments[i]->segment_vec);
				turn_layer->layer_coordinate[2] = turn_layer->layer_coordinate[0] * turn_layer->layer_coordinate[1];
				turn_layer->layerPoint = CPoint3D(m_layers[index]->m_Boundaries[0]->m_segments[i]->pstart);

				m_layers.push_back(turn_layer);

				getBoundaryPoints(m_layers[m_layers.size() - 1]);
				optimizeBoundary(m_layers[m_layers.size() - 1]);
			}
		}
		z += dz;
	}
}

void CSlice::getBoundaryPoints(SliceLayer* layer)
{
	// 筛选相交面片
	vector<LTriangle*> status;
	double v1_dist, v2_dist, v3_dist;

	unsigned int szTri = m_tris_slice.size();
	for (unsigned int i = 0; i < szTri; i++)
	{
		v1_dist = ::CalPointtoPlane(*m_tris_slice[i]->v1, layer->layer_coordinate[2], layer->layerPoint);
		v2_dist = ::CalPointtoPlane(*m_tris_slice[i]->v2, layer->layer_coordinate[2], layer->layerPoint);
		v3_dist = ::CalPointtoPlane(*m_tris_slice[i]->v3, layer->layer_coordinate[2], layer->layerPoint);

		if(!((v1_dist<0&&v2_dist<0&&v3_dist<0)||(v1_dist>0&&v2_dist>0&&v3_dist>0)))
		{
			if ((v1_dist == 0) &&
				(v2_dist == 0) &&
				(v3_dist == 0))
			{
				//跳过面片与切片重合的面片
			}
			else
				status.push_back(m_tris_slice[i]);
		}
	}

	// 对相交面片进行面片分类
	unsigned int szStatus = status.size();
	for (unsigned int i = 0; i<szStatus; i++)    
	{
		judgeFaceType(layer, status[i]);
	}

	// 求切片平面与三角面片交点
	LTriangle* pCurFace = new LTriangle();

	//选第一个面片作为起始面片
	pCurFace = status[0];
	pCurFace->b_use = true;
	
	while (true)
	{
		Boundary* m_boundary = new Boundary();
		LPoint* tmpstartPoint = new LPoint();
		LPoint* tmpendPoint = new LPoint();
		//Segment* tmpSegment = new Segment();

		//设定起始相交边，并存入m_Slice_edge容器	
		getInterSectEdge(layer, pCurFace);  
		unsigned int sz0 = m_Slice_edge.size();
		pCurFace->IntersectLine1 = m_Slice_edge[sz0 - 1];

		//由起始相交边求交点，并存入m_LinkPoint容器
		calIntersectPoint(layer, pCurFace->IntersectLine1, pCurFace, tmpstartPoint);

		//第一个面片的另一相交边求交点，并存入m_LinkPoint容器
		judgeOtherLine(layer, pCurFace);
		calIntersectPoint(layer, pCurFace->IntersectLine2, pCurFace, tmpendPoint);

		Segment* tmpSegment = new Segment(*tmpstartPoint, *tmpendPoint, pCurFace);
		m_boundary->m_segments.push_back(new Segment(*tmpSegment));
		//上面是轮廓第一个面片的特殊处理

		while (true)
		{
			//由最后存入相交边数组的边的伙伴半边寻找下一相交面片
			unsigned int sz = m_Slice_edge.size();
			pCurFace = m_Slice_edge[sz - 1]->e_adja->t;
			pCurFace->b_use = true;//把面片置为当前使用面片，标志位置为真
								 
			pCurFace->IntersectLine1 = m_Slice_edge[sz - 1]->e_adja;
			//判断当前面片中的另一相交边，保存边
			judgeOtherLine(layer, pCurFace);
			calIntersectPoint(layer, pCurFace->IntersectLine2, pCurFace, tmpendPoint);
			*tmpSegment = Segment(m_boundary->m_segments[m_boundary->m_segments.size() - 1]->pend, *tmpendPoint, pCurFace);

			m_boundary->m_segments.push_back(new Segment(*tmpSegment));

			unsigned int szlinkline = m_boundary->m_segments.size();

			//判断轮廓是否闭合
			if ((m_boundary->m_segments[0]->pstart) ^= (m_boundary->m_segments[szlinkline - 1]->pend))
			{
				m_layers[m_layers.size() - 1]->m_Boundaries.push_back(m_boundary);
				break;
			}
		}

		//跳出内层循环来到这，判断status中是否还有未被使用的面片，
		//如有，把面片置为当前，开始下一轮廓寻点；如没有，跳出外层循环
		break;
	}

}

void CSlice::getInterSectEdge(SliceLayer* layer, LTriangle* pCurFace)   //只要找到一条相交的边就可以跳出
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

	v1_plane = PointtoPlane(v1_tmp, layer->layer_coordinate[2], layer->layerPoint);
	v2_plane = PointtoPlane(v2_tmp, layer->layer_coordinate[2], layer->layerPoint);
	v3_plane = PointtoPlane(v3_tmp, layer->layer_coordinate[2], layer->layerPoint);

	switch (pCurFace->FaceType)
	{
	case NO_POINT_ON_SURFACE:   //不相交的边不能选
	{
		if ((v1_plane*v2_plane)<0)   //说明e1边可以作为第一条相交的边
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT1;
		}
		if ((v2_plane*v3_plane)<0)   //说明e2边可以作为第一条相交的边,第一条边不相交，第二条边肯定相交
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT1;
		}
	ENDINIT1:
		break;
	}
	case ONE_POINT_ON_SURFACE:  //因为三条边都相交，随便一条都可以作为起始边，选e1好了
	{
		m_Slice_edge.push_back(pCurFace->e1);
		break;
	}
	case EDGE_ON_SURFACE:  //不能选在切平面上的边
	{
		if (v1_plane != 0)   //v1不在切平面上，与之相连的两条边e1\e3都可选
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT2;
		}
		if (v2_plane != 0)   //v2不在切平面上，与之相连的两条边e1\e2都可选
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT2;
		}
		if (v3_plane != 0)   //v3不在切平面上，与之相连的两条边e2\e3都可选
		{
			m_Slice_edge.push_back(pCurFace->e3);
			goto ENDINIT2;
		}
	ENDINIT2:
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:
	{
		if (v1_plane == 0)   //v1在切平面上，与之相连的两条边e1\e3都可选
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT3;
		}
		if (v2_plane == 0)   //v2在切平面上，与之相连的两条边e1\e2都可选
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT3;
		}
		if (v3_plane == 0)   //v3在切平面上，与之相连的两条边e2\e3都可选
		{
			m_Slice_edge.push_back(pCurFace->e3);
			goto ENDINIT3;
		}
	ENDINIT3:
		break;
	}
	}

}

void CSlice::judgeFaceType(SliceLayer* layer, LTriangle* pCurFace)
{
	double min_dist, mid_dist, max_dist, v1_dist, v2_dist, v3_dist;
	v1_dist = CalPointtoPlane(*pCurFace->v1, layer->layer_coordinate[2], layer->layerPoint);
	v2_dist = CalPointtoPlane(*pCurFace->v2, layer->layer_coordinate[2], layer->layerPoint);
	v3_dist = CalPointtoPlane(*pCurFace->v3, layer->layer_coordinate[2], layer->layerPoint);

	min_dist = compareThreeNumber(v1_dist, v2_dist, v3_dist, MIN);
	mid_dist = compareThreeNumber(v1_dist, v2_dist, v3_dist, MID);
	max_dist = compareThreeNumber(v1_dist, v2_dist, v3_dist, MAX);

	//判断三角面片和切平面的相交情况,四种情况
	if ((min_dist < 0) && (max_dist > 0) && (mid_dist != 0))
		pCurFace->FaceType = NO_POINT_ON_SURFACE;
	if ((min_dist < 0) && (max_dist > 0) && (mid_dist == 0))
		pCurFace->FaceType = ONE_POINT_ON_SURFACE;
	if (((min_dist < 0) && (max_dist == 0) && (mid_dist == 0)) || 
		((min_dist == 0) && (max_dist > 0) && (mid_dist == 0)))
		pCurFace->FaceType = EDGE_ON_SURFACE;
	if ((((min_dist < 0) && (max_dist == 0) && (mid_dist != 0))) || 
		(((min_dist == 0) && (max_dist > 0) && (mid_dist != 0))))
		pCurFace->FaceType = ONLY_ONE_POINT_ON_SURFACE;
}

void CSlice::judgeOtherLine(SliceLayer* layer, LTriangle* pCurFace)
{
	int v1_plane, v2_plane;
	CPoint3D v1_tmp, v2_tmp;

	v1_tmp.x = pCurFace->IntersectLine1->e_prev->v1->x;
	v1_tmp.y = pCurFace->IntersectLine1->e_prev->v1->y;
	v1_tmp.z = pCurFace->IntersectLine1->e_prev->v1->z;

	v2_tmp.x = pCurFace->IntersectLine1->e_prev->v2->x;
	v2_tmp.y = pCurFace->IntersectLine1->e_prev->v2->y;
	v2_tmp.z = pCurFace->IntersectLine1->e_prev->v2->z;

	v1_plane = PointtoPlane(v1_tmp, layer->layer_coordinate[2], layer->layerPoint);
	v2_plane = PointtoPlane(v2_tmp, layer->layer_coordinate[2], layer->layerPoint);

	switch (pCurFace->FaceType)
	{
	case NO_POINT_ON_SURFACE:
	{
		//已选的相交边的相邻前一边判断是否相交
		if ((v1_plane * v2_plane)<0)
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
		else//否则另一条就是相交边
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_next;
		break;
	}
	case ONE_POINT_ON_SURFACE:  //这种情况比较特殊，要先判断已选的相交边是哪种类型
	{
		//已选边的一端点在切平面上
		if ((v1_plane*v2_plane) == 0)
		{
			//已选的相交边的相邻前一边判断是否相交
			if ((v1_plane*v2_plane)<0)
				pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
			else//否则另一条就是相交边
				pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_next;
		}
		else//已选边的一端点不在切平面上，/任意一条相邻的边都可以作为另一相交边
		{
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
		}
		break;
	}
	case EDGE_ON_SURFACE:  //相交不能选在切平面上的边
	{
		//已选的相交边的相邻前一边判断是否有一个点不在z切平面上
		if ((v1_plane != 0) || (v2_plane != 0))
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
		else//否则另一条就是相交边
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_next;
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:  //哪一条边其中的一个端点在切平面上，就是相交边
	{

		//已选的相交边的相邻前一边判断是否有一个点不在z切平面上
		if (v1_plane == 0)
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
		else//否则另一条就是相交边
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_next;
		break;
	}
	}
	m_Slice_edge.push_back(pCurFace->IntersectLine2);
}


void CSlice::calIntersectPoint(SliceLayer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point)
{
	int e1_plane, e2_plane;
	CPoint3D e1_tmp, e2_tmp;

	e1_tmp.x = edge->v1->x;
	e1_tmp.y = edge->v1->y;
	e1_tmp.z = edge->v1->z;

	e2_tmp.x = edge->v2->x;
	e2_tmp.y = edge->v2->y;
	e2_tmp.z = edge->v2->z;

	e1_plane = PointtoPlane(e1_tmp, layer->layer_coordinate[2], layer->layerPoint);
	e2_plane = PointtoPlane(e2_tmp, layer->layer_coordinate[2], layer->layerPoint);

	if (pCurFace->FaceType != ONLY_ONE_POINT_ON_SURFACE) {
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

			p = ::CalPlaneLineIntersectPoint(layer->layer_coordinate[2], layer->layerPoint, vec, e1_tmp);

			point->x = p.x;
			point->y = p.y;
			point->z = p.z;
		}
	}
}

double CSlice::zminofLayer(SliceLayer * layer)
{
	double zmin;
	zmin = layer->m_Boundaries[0]->m_segments[0]->pstart.z;
	unsigned int sz = layer->m_Boundaries[0]->m_segments.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		if (zmin > layer->m_Boundaries[0]->m_segments[i]->pend.z)
			zmin = layer->m_Boundaries[0]->m_segments[i]->pend.z;
	}
	return zmin;
}

void CSlice::optimizeBoundary(SliceLayer* layer)
{
	bool isCCW = isBoundaryCCW(layer);
	if (!isCCW)
		makeBoundaryCCW(layer);
	deletePoints(layer);
}

bool CSlice::isBoundaryCCW(SliceLayer* layer)
{
	unsigned int sz = layer->m_Boundaries[0]->m_segments.size();
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
		for (unsigned int i = 0; i < sz-1; i++)
		{
			p1.x = layer->m_Boundaries[0]->m_segments[i]->pstart.x;
			p1.y = layer->m_Boundaries[0]->m_segments[i]->pstart.y;
			p1.z = layer->m_Boundaries[0]->m_segments[i]->pstart.z;
			p2.x = layer->m_Boundaries[0]->m_segments[i]->pend.x;
			p2.y = layer->m_Boundaries[0]->m_segments[i]->pend.y;
			p2.z = layer->m_Boundaries[0]->m_segments[i]->pend.z;
			vec1 = CVector3D(p1, p2);
			p1.x = layer->m_Boundaries[0]->m_segments[i+1]->pstart.x;
			p1.y = layer->m_Boundaries[0]->m_segments[i+1]->pstart.y;
			p1.z = layer->m_Boundaries[0]->m_segments[i+1]->pstart.z;
			p2.x = layer->m_Boundaries[0]->m_segments[i+1]->pend.x;
			p2.y = layer->m_Boundaries[0]->m_segments[i+1]->pend.y;
			p2.z = layer->m_Boundaries[0]->m_segments[i+1]->pend.z;
			vec2 = CVector3D(p1, p2);
			vec = vec1 * vec2;
			vec.Normalize();
			if (!vec.IsZeroLength())
				break;
		}
		angle = GetAngle(vec, layer->layer_coordinate[2]);
		if(angle == 0.0)
			return true;
		else
			return false;
	}
}

void CSlice::makeBoundaryCCW(SliceLayer* layer)
{
	unsigned int sz = layer->m_Boundaries[0]->m_segments.size();
	vector<Segment*> tmp;
	for (unsigned int i = 0; i < sz; i++)
	{
		tmp.push_back(layer->m_Boundaries[0]->m_segments[sz - 1 - i]);
	}
	for (unsigned int i = 0; i < sz; i++)
	{
		layer->m_Boundaries[0]->m_segments[i] = tmp[i];
	}
	for (unsigned int i = 0; i < sz; i++)
	{
		LPoint tmp_point = layer->m_Boundaries[0]->m_segments[i]->pstart;
		layer->m_Boundaries[0]->m_segments[i]->pstart = layer->m_Boundaries[0]->m_segments[i]->pend;
		layer->m_Boundaries[0]->m_segments[i]->pend = tmp_point;
		layer->m_Boundaries[0]->m_segments[i]->segment_vec = CVector3D(CPoint3D(layer->m_Boundaries[0]->m_segments[i]->pstart), CPoint3D(layer->m_Boundaries[0]->m_segments[i]->pend));
		layer->m_Boundaries[0]->m_segments[i]->segment_vec.Normalize();
	}
}

void CSlice::deletePoints(SliceLayer * layer)
{
	vector<Segment*> tmp_segments;
	unsigned int szP = layer->m_Boundaries[0]->m_segments.size();

	// 取出单个层切面轮廓中的所有线段
	for (unsigned int j = 0; j < szP; j++)
	{
		tmp_segments.push_back(layer->m_Boundaries[0]->m_segments[j]);
	}
	layer->m_Boundaries[0]->m_segments.clear();

	unsigned int szTemp = tmp_segments.size();
	for (unsigned int i = 0; i < szTemp; i++)
	{
		if (fabs(tmp_segments[i]->pstart.x) <= 0.0000001)
			tmp_segments[i]->pstart.x = 0;
		if (fabs(tmp_segments[i]->pstart.y) <= 0.0000001)
			tmp_segments[i]->pstart.y = 0;
		if (fabs(tmp_segments[i]->pstart.z) <= 0.0000001)
			tmp_segments[i]->pstart.z = 0;
		if (fabs(tmp_segments[i]->pend.x) <= 0.0000001)
			tmp_segments[i]->pend.x = 0;
		if (fabs(tmp_segments[i]->pend.y) <= 0.0000001)
			tmp_segments[i]->pend.y = 0;
		if (fabs(tmp_segments[i]->pend.z) <= 0.0000001)
			tmp_segments[i]->pend.z = 0;
	}

	CPoint3D pCur, pNext, pNextNext;
	while (szTemp >= 3)
	{
		for (unsigned int j = 0; j < szTemp; j++)
		{
			pCur = tmp_segments[j%szTemp]->pstart;
			pNext = tmp_segments[j%szTemp]->pend;
			pNextNext = tmp_segments[(j + 1) % szTemp]->pend;
			double distance = CalPointtoLine(pNext, pCur, pNextNext);
			if (distance <= 0.0001)
			{
				tmp_segments[j%szTemp]->pend = tmp_segments[(j + 1) % szTemp]->pend;
				CPoint3D p1 = CPoint3D(tmp_segments[j%szTemp]->pstart);
				CPoint3D p2 = CPoint3D(tmp_segments[j%szTemp]->pend);
				tmp_segments[j%szTemp]->segment_vec = CVector3D(p1, p2);
				tmp_segments[j%szTemp]->segment_vec.Normalize();
				tmp_segments.erase(tmp_segments.begin() + (j + 1) % szTemp);
				szTemp = tmp_segments.size();
				j = -1;
			}
		}
		break;
	}

	//将删除共线点之后的线段存回 m_segments 中
	for (unsigned int i = 0; i < tmp_segments.size(); i++)
	{
		layer->m_Boundaries[0]->m_segments.push_back(tmp_segments[i]);
	}
}

double CSlice::compareThreeNumber(double v1, double v2, double v3, int type)
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

void CSlice::drawLayer(bool showPolygon, int start, int end)
{
	double color[3];
	unsigned int szlayer = m_layers.size();
	LPoint point, point2;
	for (int i = (start - 1); i<end; i++)
	{
		unsigned int szpolyline = m_layers[i]->m_Boundaries.size();
		if (m_layers[i]->layer_coordinate[2].dz == 1.0)
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
			unsigned int szLine = m_layers[i]->m_Boundaries[j]->m_segments.size();
			glLineWidth(1.5f);

			//绘制平面轮廓
			if (showPolygon)
			{
				glBegin(GL_POLYGON);
				glColor3f(color[0], color[1], color[2]);
				for (unsigned int k = 0; k < szLine; k++)
				{
					point = m_layers[i]->m_Boundaries[j]->m_segments[k]->pstart;
					glVertex3f(point.x, point.y, point.z);
					point = m_layers[i]->m_Boundaries[j]->m_segments[k]->pend;
					glVertex3f(point.x, point.y, point.z);
				}
				glEnd();
			}

			//绘制线框轮廓
			glColor3f(color[0], color[1], color[2]);
			for (unsigned int k = 0; k<szLine; k++)
			{
				glBegin(GL_LINE_LOOP);
				point = m_layers[i]->m_Boundaries[j]->m_segments[k]->pstart;
				glVertex3f(point.x, point.y, point.z);
				point = m_layers[i]->m_Boundaries[j]->m_segments[k]->pend;
				glVertex3f(point.x, point.y, point.z);
				glEnd();
			}
		}
	}
}
