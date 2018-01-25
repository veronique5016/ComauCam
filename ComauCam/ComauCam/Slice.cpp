#include "stdafx.h"
#include "Slice.h"
#include <algorithm>

CSlice::CSlice(void)
{
	m_dHeight = 5;
}

CSlice::~CSlice(void)
{
	int szL = m_vpLayers.size();
	for (int i = 0; i < szL; i++)
	{
		delete m_vpLayers[i];
		m_vpLayers[i] = NULL;
	}
	m_vpLayers.clear();

	int sz = m_vpSliceTris.size();
	for (int i = 0; i < sz; i++)
	{
		delete m_vpSliceTris[i];
		m_vpSliceTris[i] = NULL;
	}
	m_vpSliceTris.clear();
}

void CSlice::LoadSTLModel(CSTLModel* model)//����stlģ��
{
	model->Topologize();//��ģ�ͽ������˽ṹ
	int szTri = model->m_vecpLTris.size();
	for (int j = 0; j<szTri; j++)
	{
		m_vpSliceTris.push_back(model->m_vecpLTris[j]);     //ΪCSlice���󿽱�һ��������Ƭ����
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
		m_vpLayers.push_back(pLayer);

		int index_layer = m_vpLayers.size() - 1;//index_layer ָ��ǰ����ƽ��,����ȷ����ƽ�淨������ԶΪ z ������
		GetBoundaryPoints(m_vpLayers[index_layer]);

		//��������ʱ�����в�ɾ�����ߵ�
		m_vpLayers[index_layer]->OptimizeBoundary();
		m_vpLayers[index_layer]->RearrangeBoundary();
		
		CSliceLayer* pTurnLayer = new CSliceLayer();

		//�ж��Ƿ���Ҫ�䷨����Ƭ
		int szL = m_vpLayers[index_layer]->m_vpBoundaries[0]->m_vpSegments.size();
		for (int i = 0; i < szL; i++)
		{
			double angle = GetAngle(CVector3D(0, 0, 1), CVector3D(*m_vpLayers[index_layer]->m_vpBoundaries[0]->m_vpSegments[i]->m_pTriangle->n));
			//��׼��̬�����Ƕ�Ϊ128��
			if (angle > 100.0*PI / 180.0)
			{
				//ƫ������
				double offset_dist = 2.0;
				Offset(m_vpLayers[index_layer]->m_vpBoundaries[0], offset_dist, m_vpLayers[index_layer]->m_vCoordinate);

				//����䷨����ƽ�������ϵ��ƽ����һ��
				CPoint3D layer_point = CPoint3D(m_vpLayers[index_layer]->m_vpBoundaries[0]->m_vpSegments[i]->m_ptStart);
				layer_point.z = layer_point.z - offset_dist*cos(angle - PI / 2.0)*sin(angle - PI / 2.0);
				pTurnLayer->m_ptLayerPoint = layer_point;

				GetBoundaryPoints(pTurnLayer);
				pTurnLayer->OptimizeBoundary();	

				int szSeg = m_vpLayers[index_layer]->m_vpBoundaries[0]->m_vpSegments.size();
				for (int i = 0; i < szSeg; i++)
				{
					CPoint3D pStart = CPoint3D(m_vpLayers[index_layer]->m_vpBoundaries[0]->m_vpSegments[i]->m_ptStart.x,
						m_vpLayers[index_layer]->m_vpBoundaries[0]->m_vpSegments[i]->m_ptStart.y,
						0);
					CPoint3D pEnd = CPoint3D(m_vpLayers[index_layer]->m_vpBoundaries[0]->m_vpSegments[i]->m_ptEnd.x,
						m_vpLayers[index_layer]->m_vpBoundaries[0]->m_vpSegments[i]->m_ptEnd.y,
						0);
					CPoint3D crosspoint;
					vector<CPoint3D> crosspoints;
					CVector3D normal = m_vpLayers[index_layer]->m_vpBoundaries[0]->m_vpSegments[i]->m_vSegmentVec * CVector3D(0, 0, 1);
					normal.Normalize();

					for (int j = 0; j < pTurnLayer->m_vpBoundaries[0]->m_vpSegments.size(); j++)
					{
						CPoint3D segSp = CPoint3D(pTurnLayer->m_vpBoundaries[0]->m_vpSegments[j]->m_ptStart.x, pTurnLayer->m_vpBoundaries[0]->m_vpSegments[j]->m_ptStart.y, 0);
						CPoint3D segEp = CPoint3D(pTurnLayer->m_vpBoundaries[0]->m_vpSegments[j]->m_ptEnd.x, pTurnLayer->m_vpBoundaries[0]->m_vpSegments[j]->m_ptEnd.y, 0);

						::GetCrossPoint(crosspoint, pStart, normal, segSp, segEp);
						
						if (::IsPointInSeg(crosspoint, segSp, segEp))
						{
							crosspoints.push_back(crosspoint);
						}
					}
					int min_index = 0;
					for (int j = 0; j < crosspoints.size(); j++)
					{
						CVector3D tmp_vec = CVector3D(pStart, crosspoints[j]);
						double tmp_angle = ::GetAngle(normal, tmp_vec);
						if (tmp_angle < 0.0001)
						{
							if (::GetDistance(pStart, crosspoints[j]) <= GetDistance(pStart, crosspoints[min_index]))
								min_index = j;
						}
					}

					CSliceFrag* tmpFrag = new CSliceFrag();
					tmpFrag->m_ptBoundary[0] = CPoint3D(pStart.x, pStart.y, m_vpLayers[index_layer]->m_ptLayerPoint.z);
					tmpFrag->m_ptBoundary[1] = CPoint3D(crosspoints[min_index].x, crosspoints[min_index].y, pTurnLayer->m_ptLayerPoint.z);
					crosspoints.clear();

					for (int j = 0; j < pTurnLayer->m_vpBoundaries[0]->m_vpSegments.size(); j++)
					{
						CPoint3D segSp = CPoint3D(pTurnLayer->m_vpBoundaries[0]->m_vpSegments[j]->m_ptStart.x, pTurnLayer->m_vpBoundaries[0]->m_vpSegments[j]->m_ptStart.y, 0);
						CPoint3D segEp = CPoint3D(pTurnLayer->m_vpBoundaries[0]->m_vpSegments[j]->m_ptEnd.x, pTurnLayer->m_vpBoundaries[0]->m_vpSegments[j]->m_ptEnd.y, 0);

						::GetCrossPoint(crosspoint, pEnd, normal, segSp, segEp);
						
						if (::IsPointInSeg(crosspoint, segSp, segEp))
						{
							crosspoints.push_back(crosspoint);
						}
					}
					min_index = 0;
					for (int j = 0; j < crosspoints.size(); j++)
					{
						CVector3D tmp_vec = CVector3D(pEnd, crosspoints[j]);
						double tmp_angle = ::GetAngle(normal, tmp_vec);
						if (tmp_angle < 0.0001)
						{
							if (::GetDistance(pEnd, crosspoints[j]) <= GetDistance(pEnd, crosspoints[min_index]))
								min_index = j;
						}
					}
					tmpFrag->m_ptBoundary[2] = CPoint3D(crosspoints[min_index].x, crosspoints[min_index].y, pTurnLayer->m_ptLayerPoint.z);
					tmpFrag->m_ptBoundary[3] = CPoint3D(pEnd.x, pEnd.y, m_vpLayers[index_layer]->m_ptLayerPoint.z);
					crosspoints.clear();
					m_vpLayers[index_layer]->m_vpFragments.push_back(tmpFrag);
				}
				break;
			}
		}
		//m_vpLayers.push_back(pTurnLayer);
		z += dz;
	}
}

void CSlice::GetBoundaryPoints(CSliceLayer* layer)
{
	// ɸѡ�ཻ��Ƭ
	vector<CLTriangle*> status;
	double v1_dist, v2_dist, v3_dist;

	int szTri = m_vpSliceTris.size();
	for (int i = 0; i < szTri; i++)
	{
		v1_dist = ::CalPointtoPlane(*m_vpSliceTris[i]->v1, layer->m_vCoordinate[2], layer->m_ptLayerPoint);
		v2_dist = ::CalPointtoPlane(*m_vpSliceTris[i]->v2, layer->m_vCoordinate[2], layer->m_ptLayerPoint);
		v3_dist = ::CalPointtoPlane(*m_vpSliceTris[i]->v3, layer->m_vCoordinate[2], layer->m_ptLayerPoint);

		if (!((v1_dist<0 && v2_dist<0 && v3_dist<0) || (v1_dist>0 && v2_dist>0 && v3_dist>0)))
		{
			if ((v1_dist == 0) &&
				(v2_dist == 0) &&
				(v3_dist == 0))
			{
				//������Ƭ����Ƭ�غϵ���Ƭ
			}
			else
			{
				status.push_back(m_vpSliceTris[i]);
			}
		}
	}

	// ���ཻ��Ƭ������Ƭ����
	int szStatus = status.size();
	for (int i = 0; i<szStatus; i++)
	{
		JudgeFaceType(layer, status[i]);
	}

	// ����Ƭƽ����������Ƭ����
	CLTriangle* pCurFace = new CLTriangle();

	//ѡ��һ����Ƭ��Ϊ��ʼ��Ƭ
	int index = 0;
	while (status[index]->m_nFaceType == ONLY_ONE_POINT_ON_SURFACE)
	{
		index++;
	}
	pCurFace = status[index];
	pCurFace->m_bUse = true;


	//��while��ѭ�����ܲ�����Ҫ
	while (true)
	{
		CBoundary* m_boundary = new CBoundary();
		CPoint3D* tmpstartPoint = new CPoint3D();
		CPoint3D* tmpendPoint = new CPoint3D();
		CLEdge* intersectLine;
		//�趨��ʼ�ཻ�ߣ�������m_Slice_edge����	
		GetInterSectEdge(layer, pCurFace);
		intersectLine = pCurFace->m_pIntersectLine1;
		//����ʼ�ཻ���󽻵㣬������m_LinkPoint����
		CalIntersectPoint(layer, intersectLine, pCurFace, tmpstartPoint);

		//��һ����Ƭ����һ�ཻ���󽻵㣬������m_LinkPoint����
		JudgeOtherLine(layer, pCurFace);
		intersectLine = pCurFace->m_pIntersectLine2;
		CalIntersectPoint(layer, intersectLine, pCurFace, tmpendPoint);

		CSegment* tmpSegment = new CSegment(*tmpstartPoint, *tmpendPoint, pCurFace);
		m_boundary->m_vpSegments.push_back(new CSegment(*tmpSegment));
		//������������һ����Ƭ�����⴦��

		while (true)
		{
			//�ɻ����Ѱ����һ�ཻ��Ƭ
			pCurFace = intersectLine->e_adja->t;
			pCurFace->m_bUse = true;//����Ƭ��Ϊ��ǰʹ����Ƭ����־λ��Ϊ��								 
			pCurFace->m_pIntersectLine1 = intersectLine->e_adja;
			//�жϵ�ǰ��Ƭ�е���һ�ཻ�ߣ������

			JudgeOtherLine(layer, pCurFace);
			intersectLine = pCurFace->m_pIntersectLine2;
			CalIntersectPoint(layer, intersectLine, pCurFace, tmpendPoint);
			*tmpSegment = CSegment(m_boundary->m_vpSegments[m_boundary->m_vpSegments.size() - 1]->m_ptEnd, *tmpendPoint, pCurFace);
			m_boundary->m_vpSegments.push_back(new CSegment(*tmpSegment));
			int szlinkline = m_boundary->m_vpSegments.size();
			//�ж������Ƿ�պ�
			if ((m_boundary->m_vpSegments[0]->m_ptStart) ^= (m_boundary->m_vpSegments[szlinkline - 1]->m_ptEnd))
			{
				layer->m_vpBoundaries.push_back(m_boundary);
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

	v1_plane = PointtoPlane(v1_tmp, layer->m_vCoordinate[2], layer->m_ptLayerPoint);
	v2_plane = PointtoPlane(v2_tmp, layer->m_vCoordinate[2], layer->m_ptLayerPoint);
	v3_plane = PointtoPlane(v3_tmp, layer->m_vCoordinate[2], layer->m_ptLayerPoint);

	switch (pCurFace->m_nFaceType)
	{
	case NO_POINT_ON_SURFACE:   //���ཻ�ı߲���ѡ
	{
		if ((v1_plane*v2_plane)<0)   //˵��e1�߿�����Ϊ��һ���ཻ�ı�
		{
			pCurFace->m_pIntersectLine1 = pCurFace->e1;
			goto ENDINIT1;
		}
		if ((v2_plane*v3_plane)<0)   //˵��e2�߿�����Ϊ��һ���ཻ�ı�,��һ���߲��ཻ���ڶ����߿϶��ཻ
		{
			pCurFace->m_pIntersectLine1 = pCurFace->e2;
			goto ENDINIT1;
		}
	ENDINIT1:
		break;
	}
	case ONE_POINT_ON_SURFACE:  //��Ϊ�����߶��ཻ�����һ����������Ϊ��ʼ�ߣ�ѡe1����
	{
		pCurFace->m_pIntersectLine1 = pCurFace->e1;
		break;
	}
	case EDGE_ON_SURFACE:  //����ѡ����ƽ���ϵı�
	{
		if (v1_plane != 0)   //v1������ƽ���ϣ���֮������������e1\e3����ѡ
		{
			pCurFace->m_pIntersectLine1 = pCurFace->e1;
			goto ENDINIT2;
		}
		if (v2_plane != 0)   //v2������ƽ���ϣ���֮������������e1\e2����ѡ
		{
			pCurFace->m_pIntersectLine1 = pCurFace->e2;
			goto ENDINIT2;
		}
		if (v3_plane != 0)   //v3������ƽ���ϣ���֮������������e2\e3����ѡ
		{
			pCurFace->m_pIntersectLine1 = pCurFace->e3;
			goto ENDINIT2;
		}
	ENDINIT2:
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:
	{
		if (v1_plane == 0)   //v1����ƽ���ϣ���֮������������e1\e3����ѡ
		{
			pCurFace->m_pIntersectLine1 = pCurFace->e1;
			goto ENDINIT3;
		}
		if (v2_plane == 0)   //v2����ƽ���ϣ���֮������������e1\e2����ѡ
		{
			pCurFace->m_pIntersectLine1 = pCurFace->e2;
			goto ENDINIT3;
		}
		if (v3_plane == 0)   //v3����ƽ���ϣ���֮������������e2\e3����ѡ
		{
			pCurFace->m_pIntersectLine1 = pCurFace->e3;
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
	v1_dist = CalPointtoPlane(*pCurFace->v1, layer->m_vCoordinate[2], layer->m_ptLayerPoint);
	v2_dist = CalPointtoPlane(*pCurFace->v2, layer->m_vCoordinate[2], layer->m_ptLayerPoint);
	v3_dist = CalPointtoPlane(*pCurFace->v3, layer->m_vCoordinate[2], layer->m_ptLayerPoint);

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

	v1_tmp.x = pCurFace->m_pIntersectLine1->e_prev->v_start->x;
	v1_tmp.y = pCurFace->m_pIntersectLine1->e_prev->v_start->y;
	v1_tmp.z = pCurFace->m_pIntersectLine1->e_prev->v_start->z;

	v2_tmp.x = pCurFace->m_pIntersectLine1->e_prev->v_end->x;
	v2_tmp.y = pCurFace->m_pIntersectLine1->e_prev->v_end->y;
	v2_tmp.z = pCurFace->m_pIntersectLine1->e_prev->v_end->z;

	v1_plane = PointtoPlane(v1_tmp, layer->m_vCoordinate[2], layer->m_ptLayerPoint);
	v2_plane = PointtoPlane(v2_tmp, layer->m_vCoordinate[2], layer->m_ptLayerPoint);

	switch (pCurFace->m_nFaceType)
	{
	case NO_POINT_ON_SURFACE:
	{
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
		if ((v1_plane * v2_plane)<0)
			pCurFace->m_pIntersectLine2 = pCurFace->m_pIntersectLine1->e_prev;
		else//������һ�������ཻ��
			pCurFace->m_pIntersectLine2 = pCurFace->m_pIntersectLine1->e_next;
		break;
	}
	case ONE_POINT_ON_SURFACE:  //��������Ƚ����⣬Ҫ���ж���ѡ���ཻ������������
	{
		//��ѡ�ߵ�һ�˵�����ƽ����
		if ((v1_plane*v2_plane) == 0)
		{
			//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
			if ((v1_plane*v2_plane)<0)
				pCurFace->m_pIntersectLine2 = pCurFace->m_pIntersectLine1->e_prev;
			else//������һ�������ཻ��
				pCurFace->m_pIntersectLine2 = pCurFace->m_pIntersectLine1->e_next;
		}
		else//��ѡ�ߵ�һ�˵㲻����ƽ���ϣ�/����һ�����ڵı߶�������Ϊ��һ�ཻ��
		{
			pCurFace->m_pIntersectLine2 = pCurFace->m_pIntersectLine1->e_prev;
		}
		break;
	}
	case EDGE_ON_SURFACE:  //�ཻ����ѡ����ƽ���ϵı�
	{
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ���һ���㲻��z��ƽ����
		if ((v1_plane != 0) || (v2_plane != 0))
			pCurFace->m_pIntersectLine2 = pCurFace->m_pIntersectLine1->e_prev;
		else//������һ�������ཻ��
			pCurFace->m_pIntersectLine2 = pCurFace->m_pIntersectLine1->e_next;
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:  //��һ�������е�һ���˵�����ƽ���ϣ������ཻ��
	{
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ���һ���㲻��z��ƽ����
		if (v2_plane == 0)
			pCurFace->m_pIntersectLine2 = pCurFace->m_pIntersectLine1->e_prev;
		else//������һ�������ཻ��
			pCurFace->m_pIntersectLine2 = pCurFace->m_pIntersectLine1->e_next;
		break;
	}
	}
}


void CSlice::CalIntersectPoint(CSliceLayer* layer, CLEdge * edge, CLTriangle*pCurFace, CPoint3D* point)
{
	int e1_plane, e2_plane;
	CPoint3D e1_tmp, e2_tmp;

	e1_tmp.x = edge->v_start->x;
	e1_tmp.y = edge->v_start->y;
	e1_tmp.z = edge->v_start->z;

	e2_tmp.x = edge->v_end->x;
	e2_tmp.y = edge->v_end->y;
	e2_tmp.z = edge->v_end->z;

	e1_plane = PointtoPlane(e1_tmp, layer->m_vCoordinate[2], layer->m_ptLayerPoint);
	e2_plane = PointtoPlane(e2_tmp, layer->m_vCoordinate[2], layer->m_ptLayerPoint);

	if (pCurFace->m_nFaceType != ONLY_ONE_POINT_ON_SURFACE) {
		if (e1_plane == 0)   //�����������غϣ��Ѷ˵㸳ֵ������
		{
			point->x = edge->v_start->x;
			point->y = edge->v_start->y;
			point->z = edge->v_start->z;
		}
		else if (e2_plane == 0)
		{
			point->x = edge->v_end->x;
			point->y = edge->v_end->y;
			point->z = edge->v_end->z;
		}
		else    //���˵㶼������Ƭ����
		{
			CPoint3D p = CPoint3D();
			CVector3D vec = CVector3D(e1_tmp, e2_tmp);

			p = ::CalPlaneLineIntersectPoint(layer->m_vCoordinate[2], layer->m_ptLayerPoint, vec, e1_tmp);

			point->x = p.x;
			point->y = p.y;
			point->z = p.z;
		}
	}
}
