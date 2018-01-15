#include "stdafx.h"
#include "Slice.h"
#include <algorithm>

PolyLine::PolyLine()
{
}
PolyLine::~PolyLine()
{
}

Layer::Layer()
{
}
Layer::~Layer()
{
}

CSlice::CSlice(void)
{

}

CSlice::~CSlice(void)
{

}

void CSlice::LoadSTLModel(CSTLModel* model)//����stlģ��
{
	model->Topologize();//��ģ�ͽ������˽ṹ
	int szTri = model->m_ltris.size();
	for (int j = 0; j<szTri; j++)
	{
		m_tris_slice.push_back(model->m_ltris[j]);     //ΪCSlice���󿽱�һ��������Ƭ����
	}
}
void CSlice::slice(CSTLModel* model)
{
	AfxMessageBox(_T("Slice Begin!!"), MB_OK, 0);
	double z_Min, z_Max;  //ģ�ͼ��޳ߴ�
	double z;  //��Ƭ�߶�
	double dz;   //��Ƭ���
	double ex[6];
	model->FindExtreme(ex);   //����ģ�͵ļ��޳ߴ�
	z_Min = ex[4];
	z_Max = ex[5];
	z = z_Min + 0.1;  //��һ����Ƭ��
	dz = 0.2;   //���
	while (true)
	{
		getpolylinePoints(z);
		if (z >= (z_Max - dz))
		{
			break;
		}
		z += dz;
	}

}

void CSlice::getpolylinePoints(double z)
{
	vector<LTriangle*> status; //������ϲ�ߵģ�ɸѡ��������Ƭ
	int szTri = m_tris_slice.size();
	for (int i = 0; i<szTri; i++)         //����������Ƭ,ɸѡ���ཻ��Ƭ
	{
		double z_min = ReturnZmin(m_tris_slice[i]);
		double z_max = ReturnZmax(m_tris_slice[i]);
		if (z_min <= z&&z_max >= z)
		{
			if (z_min == z_max)
			{
				//������Ƭ����Ƭ�غϵ���Ƭ
			}
			else {
				status.push_back(m_tris_slice[i]);
			}       //�洢����Ƭ���ཻ��Ƭ
		}
	}
	int szStatus = status.size();
	for (int i = 0; i<szStatus; i++)       //���ཻ��Ƭ������Ƭ����
	{
		JudgeFaceType(z, status[i]);
	}
	LTriangle* pCurFace = new LTriangle();
	//ѡ��һ����Ƭ��Ϊ��ʼ��Ƭ
	pCurFace = status[0];
	pCurFace->b_use = true;//����Ƭ��Ϊ��ǰʹ����Ƭ����־λ��Ϊ��
	while (true)
	{
		PolyLine* m_polyline = new PolyLine();
		InterSect(z, pCurFace);  //�趨��ʼ�ཻ�ߣ�������m_Slice_edge����
		CPoint3D* tmpLinkPoint = new CPoint3D();
		int sz0 = m_Slice_edge.size();
		pCurFace->SelectIntersectLine = m_Slice_edge[sz0 - 1];
		//����ʼ�ཻ���󽻵㣬������m_LinkPoint����
		CalInerSectPoint(z, pCurFace->SelectIntersectLine, pCurFace, tmpLinkPoint);
		m_polyline->m_Linkpoints.push_back(new CPoint3D(*tmpLinkPoint));
		//��һ����Ƭ����һ�ཻ���󽻵㣬������m_LinkPoint����
		JudgeOtherLine(z, pCurFace);
		CalInerSectPoint(z, pCurFace->OtherIntersectLine, pCurFace, tmpLinkPoint);
		m_polyline->m_Linkpoints.push_back(new CPoint3D(*tmpLinkPoint));
		//������������һ����Ƭ�����⴦��
		while (true)
		{
			//���������ཻ������ıߵĻ����Ѱ����һ�ཻ��Ƭ
			int sz = m_Slice_edge.size();
			pCurFace = m_Slice_edge[sz - 1]->e_adja->t;
			pCurFace->b_use = true;//����Ƭ��Ϊ��ǰʹ����Ƭ����־λ��Ϊ��
								   //������m_Slice_edge���ཻ������һ��Ƭ�ĵڶ����ཻ�ߣ����Ļ���߾��ǵ�ǰ��Ƭ��ѡ���ཻ��
			pCurFace->SelectIntersectLine = m_Slice_edge[sz - 1]->e_adja;
			//�жϵ�ǰ��Ƭ�е���һ�ཻ�ߣ������
			JudgeOtherLine(z, pCurFace);
			//�󽻵㣬�����ֵ�
			CalInerSectPoint(z, pCurFace->OtherIntersectLine, pCurFace, tmpLinkPoint);
			m_polyline->m_Linkpoints.push_back(new CPoint3D(*tmpLinkPoint));
			int szlinkPoint = m_polyline->m_Linkpoints.size();
			//�жϽ����Ƿ�Ϊ��ʼ�㣬���ǣ������ڲ�ѭ������ʱ����һ�������㼯������ֱ�ӷ����ڲ�ѭ��
			if (*(m_polyline->m_Linkpoints[0]) ^= *(m_polyline->m_Linkpoints[szlinkPoint - 1]))
			{
				Layer* pLayer = new Layer();
				pLayer->z = z;
				pLayer->m_Polylines.push_back(m_polyline);
				m_layers.push_back(pLayer);
				int szpoint = m_polyline->m_Linkpoints.size();
				break;
			}
		}
		//�����ڲ�ѭ�������⣬�ж�status���Ƿ���δ��ʹ�õ���Ƭ��
		//���У�����Ƭ��Ϊ��ǰ����ʼ��һ����Ѱ�㣻��û�У��������ѭ��
		break;
	}

}

void CSlice::drawpolyline()
{
	int szlayer = m_layers.size();
	for (int i = 0; i<szlayer; i++)
	{
		int szpolyline = m_layers[i]->m_Polylines.size();
		for (int j = 0; j<szpolyline; j++)
		{
			int szPoint = m_layers[i]->m_Polylines[j]->m_Linkpoints.size();
			for (int k = 0; k<(szPoint-1); k++)
			{
				CPoint3D* point1 = new CPoint3D(*(m_layers[i]->m_Polylines[j]->m_Linkpoints[k]));
				CPoint3D* point2 = new CPoint3D(*(m_layers[i]->m_Polylines[j]->m_Linkpoints[k + 1]));
				glLineWidth(1.5f);
				glBegin(GL_LINES);
				glColor3f(0.0f, 1.0f, 0.0f);
				glVertex3f(point1->x, point1->y, point1->z);
				glVertex3f(point2->x, point2->y, point2->z);
				glEnd();
			}
		}
	}

}

void CSlice::InterSect(double z, LTriangle* pCurFace)   //ֻҪ�ҵ�һ���ཻ�ı߾Ϳ�������
{
	switch (pCurFace->FaceType)
	{
	case NO_POINT_ON_SURFACE:   //���ཻ�ı߲���ѡ
	{
		if ((pCurFace->v1->z - z)*(pCurFace->v2->z - z)<0)   //˵��e1�߿�����Ϊ��һ���ཻ�ı�
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT1;
		}
		if ((pCurFace->v2->z - z)*(pCurFace->v3->z - z)<0)   //˵��e2�߿�����Ϊ��һ���ཻ�ı�,��һ���߲��ཻ���ڶ����߿϶��ཻ
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT1;
		}
	ENDINIT1:
		break;
	}
	case ONE_POINT_ON_SURFACE:  //��Ϊ�����߶��ཻ�����һ����������Ϊ��ʼ�ߣ�ѡe1����
	{
		m_Slice_edge.push_back(pCurFace->e1);
		break;
	}
	case EDGE_ON_SURFACE:  //����ѡ����ƽ���ϵı�
	{
		if ((pCurFace->v1->z - z) != 0)   //v1������ƽ���ϣ���֮������������e1\e3����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT2;
		}
		if ((pCurFace->v2->z - z) != 0)   //v2������ƽ���ϣ���֮������������e1\e2����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT2;
		}
		if ((pCurFace->v3->z - z) != 0)   //v3������ƽ���ϣ���֮������������e2\e3����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e3);
			goto ENDINIT2;
		}
	ENDINIT2:
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:
	{
		if ((pCurFace->v1->z - z) == 0)   //v1����ƽ���ϣ���֮������������e1\e3����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT3;
		}
		if ((pCurFace->v2->z - z) == 0)   //v2����ƽ���ϣ���֮������������e1\e2����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT3;
		}
		if ((pCurFace->v3->z - z) == 0)   //v3����ƽ���ϣ���֮������������e2\e3����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e3);
			goto ENDINIT3;
		}
	ENDINIT3:
		break;
	}
	}

}

void CSlice::JudgeFaceType(double z, LTriangle* pCurFace)
{
	double z_min = ReturnZmin(pCurFace);
	double z_max = ReturnZmax(pCurFace);
	double z_mid = ReturnZmid(pCurFace);
	//�ж�������Ƭ����ƽ����ཻ���,�������
	if (z_min<z&&z_max>z&&z_mid != z)
		pCurFace->FaceType = NO_POINT_ON_SURFACE;
	if (z_min<z&&z_max>z&&z_mid == z)
		pCurFace->FaceType = ONE_POINT_ON_SURFACE;
	if ((z_min<z&&z_max == z&&z_mid == z) || (z_min == z&&z_max>z&&z_mid == z))
		pCurFace->FaceType = EDGE_ON_SURFACE;
	if ((z_min<z&&z_max == z&&z_mid != z) || (z_min == z&&z_max>z&&z_mid != z))
		pCurFace->FaceType = ONLY_ONE_POINT_ON_SURFACE;
}

void CSlice::JudgeOtherLine(double z, LTriangle* pCurFace)
{
	switch (pCurFace->FaceType)
	{
	case NO_POINT_ON_SURFACE:
	{
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
		if ((pCurFace->SelectIntersectLine->e_prev->v1->z - z)*(pCurFace->SelectIntersectLine->e_prev->v2->z - z)<0)
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		else//������һ�������ཻ��
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		break;
	}
	case ONE_POINT_ON_SURFACE:  //��������Ƚ����⣬Ҫ���ж���ѡ���ཻ������������
	{
		//��ѡ�ߵ�һ�˵�����ƽ����
		if ((pCurFace->SelectIntersectLine->v1->z - z)*(pCurFace->SelectIntersectLine->v2->z - z) == 0)
		{
			//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
			if ((pCurFace->SelectIntersectLine->e_prev->v1->z - z)*(pCurFace->SelectIntersectLine->e_prev->v2->z - z)<0)
				pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
			else//������һ�������ཻ��
				pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		}
		else//��ѡ�ߵ�һ�˵㲻����ƽ���ϣ�/����һ�����ڵı߶�������Ϊ��һ�ཻ��
		{
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		}
		break;
	}
	case EDGE_ON_SURFACE:  //�ཻ����ѡ����ƽ���ϵı�
	{
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ���һ���㲻��z��ƽ����
		if ((pCurFace->SelectIntersectLine->e_prev->v1->z - z) != 0 || (pCurFace->SelectIntersectLine->e_prev->v2->z - z) != 0)
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		else//������һ�������ཻ��
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:  //��һ�������е�һ���˵�����ƽ���ϣ������ཻ��
	{

		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ���һ���㲻��z��ƽ����
		if ((pCurFace->SelectIntersectLine->e_prev->v1->z - z) == 0)
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		else//������һ�������ཻ��
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		break;
	}
	}
	m_Slice_edge.push_back(pCurFace->OtherIntersectLine);
}


void CSlice::CalInerSectPoint(double z, LEdge * edge, LTriangle*pCurFace, CPoint3D* point)
{
	if (pCurFace->FaceType != ONLY_ONE_POINT_ON_SURFACE) {
		if (edge->v1->z == z)   //�����������غϣ��Ѷ˵㸳ֵ������
		{
			point->x = edge->v1->x;
			point->y = edge->v1->y;
		}
		else if (edge->v2->z == z)
		{
			point->x = edge->v2->x;
			point->y = edge->v2->y;
		}
		else    //���˵㶼������Ƭ����
		{
			double lamda;
			CPoint3D* vec = new CPoint3D();
			lamda = (z - edge->v1->z) / (edge->v2->z - edge->v1->z);
			vec->x = edge->v2->x - edge->v1->x;
			vec->y = edge->v2->y - edge->v1->y;
			point->x = edge->v1->x + lamda*vec->x;
			point->y = edge->v1->y + lamda*vec->y;
		}
		point->z = z;
	}
}

double CSlice::ReturnZmin(const LTriangle* Ltri)
{
	double z1, z2, z3;
	z1 = Ltri->v1->z;
	z2 = Ltri->v2->z;
	z3 = Ltri->v3->z;
	if (z1>z2)
	{
		swap(z1, z2);
	}
	if (z2>z3)
	{
		swap(z2, z3);
	}
	if (z1>z2)
	{
		swap(z1, z2);
	}
	return z1;

}
double CSlice::ReturnZmax(const LTriangle* Ltri)
{
	double z1, z2, z3;
	z1 = Ltri->v1->z;
	z2 = Ltri->v2->z;
	z3 = Ltri->v3->z;
	if (z1>z2)
	{
		swap(z1, z2);
	}
	if (z2>z3)
	{
		swap(z2, z3);
	}
	if (z1>z2)
	{
		swap(z1, z2);
	}
	return z3;
}

double CSlice::ReturnZmid(const LTriangle* Ltri)
{
	double z1, z2, z3;
	z1 = Ltri->v1->z;
	z2 = Ltri->v2->z;
	z3 = Ltri->v3->z;
	if (z1>z2)
	{
		swap(z1, z2);
	}
	if (z2>z3)
	{
		swap(z2, z3);
	}
	if (z1>z2)
	{
		swap(z1, z2);
	}
	return z2;
}

