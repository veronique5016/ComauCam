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

void CSlice::LoadSTLModel(CSTLModel* model)//����stlģ��
{
	model->Topologize();//��ģ�ͽ������˽ṹ
	unsigned int szTri = model->m_vecpLTris.size();
	for (unsigned int j = 0; j<szTri; j++)
	{
		m_vecpSliceTris.push_back(model->m_vecpLTris[j]);     //ΪCSlice���󿽱�һ��������Ƭ����
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

		int index = m_vecpLayers.size() - 1;//index ָ��ǰ����ƽ��,����ȷ����ƽ�淨������ԶΪ z ������
		GetBoundaryPoints(m_vecpLayers[index]);

		//��������ʱ�����в�ɾ�����ߵ�
		m_vecpLayers[index]->OptimizeBoundary();
		m_vecpLayers[index]->RearrangeBoundary();

		//�ж��Ƿ���Ҫ�䷨����Ƭ
		unsigned int szL = m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments.size();		
		for (unsigned int i = 0; i < szL; i++)
		{
			double angle = GetAngle(CVector3D(0, 0, 1), CVector3D(*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_pTriangle->n))*180.0 / PI;
			if (angle > 128.0)
			{
				//ƫ�Ƹ��߶�
				CVector3D offset_vec = m_vecpLayers[index]->m_vLayerCoordinate[2] * m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec;
				offset_vec.Normalize();
				//�˶��� bug ���ڣ���һ���������ж����߶���Ҫ�䷨�����ҷ�������ͬʱ��zaxis_distance Ӧ�ò�ͬ�����˴���ͬ��
				double zmin_tmp = m_vecpLayers[index]->ZminofLayer();
				double zmin_prev = m_vecpLayers[index - 1]->ZminofLayer();
				double zaxis_distance = zmin_tmp - zmin_prev;
				double offset = zaxis_distance * tan(GetAngle(CVector3D(0,0,1), CVector3D(*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_pTriangle->n*m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec)));
				MoveSegment(m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i], CVector3D(offset_vec*offset));

				//��ƫ�ƺ���߶���ǰ���߶εĽ���,�γ��µ�����
				CPoint3D point_out;
				GetCrossPoint(point_out, *m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i], *m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i - 1) % szL]);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart = CPoint3D(point_out);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i - 1) % szL]->m_ptEnd = CPoint3D(point_out);
				GetCrossPoint(point_out, *m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i], *m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i + 1) % szL]);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd = CPoint3D(point_out);
				m_vecpLayers[index]->m_vecpBoundaries[0]->m_vecpSegments[(i + 1) % szL]->m_ptStart = CPoint3D(point_out);

				//����䷨����ƽ�������ϵ��ƽ����һ��
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
	// ɸѡ�ཻ��Ƭ
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
				//������Ƭ����Ƭ�غϵ���Ƭ
			}
			else
				status.push_back(m_vecpSliceTris[i]);
		}
	}

	// ���ཻ��Ƭ������Ƭ����
	unsigned int szStatus = status.size();
	for (unsigned int i = 0; i<szStatus; i++)    
	{
		JudgeFaceType(layer, status[i]);
	}

	// ����Ƭƽ����������Ƭ����
	CLTriangle* pCurFace = new CLTriangle();

	//ѡ��һ����Ƭ��Ϊ��ʼ��Ƭ
	pCurFace = status[0];
	pCurFace->m_bUse = true;
	
	while (true)
	{
		CBoundary* m_boundary = new CBoundary();
		CPoint3D* tmpstartPoint = new CPoint3D();
		CPoint3D* tmpendPoint = new CPoint3D();

		//�趨��ʼ�ཻ�ߣ�������m_Slice_edge����	
		GetInterSectEdge(layer, pCurFace);  
		unsigned int sz0 = m_vecpSliceEdge.size();
		pCurFace->m_pIntersectLine1 = m_vecpSliceEdge[sz0 - 1];

		//����ʼ�ཻ���󽻵㣬������m_LinkPoint����
		CalIntersectPoint(layer, pCurFace->m_pIntersectLine1, pCurFace, tmpstartPoint);

		//��һ����Ƭ����һ�ཻ���󽻵㣬������m_LinkPoint����
		JudgeOtherLine(layer, pCurFace);
		CalIntersectPoint(layer, pCurFace->m_pIntersectLine2, pCurFace, tmpendPoint);

		CSegment* tmpSegment = new CSegment(*tmpstartPoint, *tmpendPoint, pCurFace);
		m_boundary->m_vecpSegments.push_back(new CSegment(*tmpSegment));
		//������������һ����Ƭ�����⴦��

		while (true)
		{
			//���������ཻ������ıߵĻ����Ѱ����һ�ཻ��Ƭ
			unsigned int sz = m_vecpSliceEdge.size();
			pCurFace = m_vecpSliceEdge[sz - 1]->e_adja->t;
			pCurFace->m_bUse = true;//����Ƭ��Ϊ��ǰʹ����Ƭ����־λ��Ϊ��
								 
			pCurFace->m_pIntersectLine1 = m_vecpSliceEdge[sz - 1]->e_adja;
			//�жϵ�ǰ��Ƭ�е���һ�ཻ�ߣ������
			JudgeOtherLine(layer, pCurFace);
			CalIntersectPoint(layer, pCurFace->m_pIntersectLine2, pCurFace, tmpendPoint);
			*tmpSegment = CSegment(m_boundary->m_vecpSegments[m_boundary->m_vecpSegments.size() - 1]->m_ptEnd, *tmpendPoint, pCurFace);

			m_boundary->m_vecpSegments.push_back(new CSegment(*tmpSegment));

			unsigned int szlinkline = m_boundary->m_vecpSegments.size();

			//�ж������Ƿ�պ�
			if ((m_boundary->m_vecpSegments[0]->m_ptStart) ^= (m_boundary->m_vecpSegments[szlinkline - 1]->m_ptEnd))
			{
				layer->m_vecpBoundaries.push_back(m_boundary);
				break;
			}
		}

		//�����ڲ�ѭ�������⣬�ж�status���Ƿ���δ��ʹ�õ���Ƭ��
		//���У�����Ƭ��Ϊ��ǰ����ʼ��һ����Ѱ�㣻��û�У��������ѭ��
		break;
	}
}

void CSlice::GetInterSectEdge(CSliceLayer* layer, CLTriangle* pCurFace)   //ֻҪ�ҵ�һ���ཻ�ı߾Ϳ�������
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
	case NO_POINT_ON_SURFACE:   //���ཻ�ı߲���ѡ
	{
		if ((v1_plane*v2_plane)<0)   //˵��e1�߿�����Ϊ��һ���ཻ�ı�
		{
			m_vecpSliceEdge.push_back(pCurFace->e1);
			goto ENDINIT1;
		}
		if ((v2_plane*v3_plane)<0)   //˵��e2�߿�����Ϊ��һ���ཻ�ı�,��һ���߲��ཻ���ڶ����߿϶��ཻ
		{
			m_vecpSliceEdge.push_back(pCurFace->e2);
			goto ENDINIT1;
		}
	ENDINIT1:
		break;
	}
	case ONE_POINT_ON_SURFACE:  //��Ϊ�����߶��ཻ�����һ����������Ϊ��ʼ�ߣ�ѡe1����
	{
		m_vecpSliceEdge.push_back(pCurFace->e1);
		break;
	}
	case EDGE_ON_SURFACE:  //����ѡ����ƽ���ϵı�
	{
		if (v1_plane != 0)   //v1������ƽ���ϣ���֮������������e1\e3����ѡ
		{
			m_vecpSliceEdge.push_back(pCurFace->e1);
			goto ENDINIT2;
		}
		if (v2_plane != 0)   //v2������ƽ���ϣ���֮������������e1\e2����ѡ
		{
			m_vecpSliceEdge.push_back(pCurFace->e2);
			goto ENDINIT2;
		}
		if (v3_plane != 0)   //v3������ƽ���ϣ���֮������������e2\e3����ѡ
		{
			m_vecpSliceEdge.push_back(pCurFace->e3);
			goto ENDINIT2;
		}
	ENDINIT2:
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:
	{
		if (v1_plane == 0)   //v1����ƽ���ϣ���֮������������e1\e3����ѡ
		{
			m_vecpSliceEdge.push_back(pCurFace->e1);
			goto ENDINIT3;
		}
		if (v2_plane == 0)   //v2����ƽ���ϣ���֮������������e1\e2����ѡ
		{
			m_vecpSliceEdge.push_back(pCurFace->e2);
			goto ENDINIT3;
		}
		if (v3_plane == 0)   //v3����ƽ���ϣ���֮������������e2\e3����ѡ
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

	//�ж�������Ƭ����ƽ����ཻ���,�������
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
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
		if ((v1_plane * v2_plane)<0)
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
		else//������һ�������ཻ��
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_next;
		break;
	}
	case ONE_POINT_ON_SURFACE:  //��������Ƚ����⣬Ҫ���ж���ѡ���ཻ������������
	{
		//��ѡ�ߵ�һ�˵�����ƽ����
		if ((v1_plane*v2_plane) == 0)
		{
			//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
			if ((v1_plane*v2_plane)<0)
				pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
			else//������һ�������ཻ��
				pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_next;
		}
		else//��ѡ�ߵ�һ�˵㲻����ƽ���ϣ�/����һ�����ڵı߶�������Ϊ��һ�ཻ��
		{
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
		}
		break;
	}
	case EDGE_ON_SURFACE:  //�ཻ����ѡ����ƽ���ϵı�
	{
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ���һ���㲻��z��ƽ����
		if ((v1_plane != 0) || (v2_plane != 0))
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
		else//������һ�������ཻ��
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_next;
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:  //��һ�������е�һ���˵�����ƽ���ϣ������ཻ��
	{
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ���һ���㲻��z��ƽ����
		if (v1_plane == 0)
			pCurFace->m_pIntersectLine2= pCurFace->m_pIntersectLine1->e_prev;
		else//������һ�������ཻ��
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
		if (e1_plane==0)   //�����������غϣ��Ѷ˵㸳ֵ������
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
		else    //���˵㶼������Ƭ����
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
	//ȷ���䷨����ƽ��Ŀ��
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
