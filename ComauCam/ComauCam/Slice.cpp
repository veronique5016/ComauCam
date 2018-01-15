#include "stdafx.h"
#include "Slice.h"
#include <algorithm>

PolyLine::PolyLine()
{
}
PolyLine::~PolyLine()
{
	unsigned int szP = m_Linkpoints.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_Linkpoints[i];
		m_Linkpoints[i] = NULL;
	}
	m_Linkpoints.clear();
}

Layer::Layer()
{
	layer_gravity = CVector3D(0, 0, 1);
}
Layer::~Layer()
{
	unsigned int szP = m_Polylines.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_Polylines[i];
		m_Polylines[i] = NULL;
	}
	m_Polylines.clear();
}

CSlice::CSlice(void)
{
	height = 10;
	gravity = CVector3D(0, 0, 1);
	need_support = false;
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

void CSlice::LoadSTLModel(CSTLModel* model)//����stlģ��
{
	model->Topologize();//��ģ�ͽ������˽ṹ
	unsigned int szTri = model->m_ltris.size();
	for (unsigned int j = 0; j<szTri; j++)
	{
		m_tris_slice.push_back(model->m_ltris[j]);     //ΪCSlice���󿽱�һ��������Ƭ����
	}
}

double CSlice::getTurnPoint()
{

	unsigned int sz = m_tris_slice.size();
	CVector3D* tmp_normal = NULL;	

	for (unsigned int i = 0; i < sz; i++)
	{
		tmp_normal = m_tris_slice[i]->n;
		if ((tmp_normal->dz < 0) && (tmp_normal->dz>-0.999))
			z_tris.push_back(m_tris_slice[i]);
	}

	unsigned int szTri = z_tris.size();
	double zmin_turn = 0;
	
	if (z_tris.size() == 0)
	{
		return zmin_turn = 0.0; //��� zmin_turn Ϊ 0����˵��û��ת�۵㣬��������ܴ�������ģ��
	}
	else
	{
		zmin_turn = ReturnZmin(m_tris_slice[0]);
		return zmin_turn;
	}
}

CVector3D CSlice::getTurnVec()
{
	CVector3D vec = CVector3D(0, 0, 1);
	CPoint3D p1;
	CPoint3D p2;
	p1.x = z_tris[0]->v1->x;
	p1.y = z_tris[0]->v1->y;
	p1.z = z_tris[0]->v1->z;

	p2.z = p1.z;
	if (z_tris[0]->n->dy <= 0.00001 && z_tris[0]->n->dy >= -0.00001)
	{
		p2.x = p1.x;
		p2.y = p1.y+1.0;
	}
	else if (z_tris[0]->n->dx <= 0.00001 && z_tris[0]->n->dx >= -0.00001)
	{
		p2.x = p1.x + 1.0;
		p2.y = p1.y;
	}
	else
	{
		p2.x = p1.x - 1.0;
		p2.y = p1.y + p1.x*z_tris[0]->n->dx / z_tris[0]->n->dy;
	}
	CVector3D tmp_vec = CVector3D(p2, p1);
	tmp_vec.Normalize();
	double result = tmp_vec | *z_tris[0]->n;
	vec = *z_tris[0]->n*tmp_vec;
	vec.Normalize();
	if (vec.dz <= 0.0)
		vec = CVector3D(0.0, 0.0, 0.0) - vec;
	return vec;
}

void CSlice::slice(CSTLModel* model)
{
	gravity = CVector3D(0, 0, 1);
	double z_Min, z_Max;  //ģ�ͼ��޳ߴ�
	double z;  //��Ƭ�߶�
	double dz;   //��Ƭ���
	double ex[6];
	model->FindExtreme(ex);   //����ģ�͵ļ��޳ߴ�
	z_Min = ex[4];
	z_Max = ex[5];
	z = z_Min + 0.1;  //��һ����Ƭ�㣬�޷���Ϊ z_Min
	dz = height;   //���


/////////////////////////////////
	//��ȡ����̬����ƽ��ķ�����
/////////////////////////////////

	LTriangle* tmp_tri = NULL;
	LTriangle* tmp_triangle = new LTriangle();

	tmp_tri = m_tris_slice[0];
	tmp_triangle->v1 = m_tris_slice[0]->v1;
	tmp_triangle->v2 = m_tris_slice[0]->v3;

	double pt_x = 10.0;
	double pt_z = 0.0;
	pt_z = 50.0 - 30 * m_tris_slice[0]->n->dz / m_tris_slice[0]->n->dx;
	CPoint3D* tmp_point = new CPoint3D(pt_x, 0.0, pt_z);
	LVertex* tmp = new LVertex(*tmp_point);
	tmp_triangle->v3 = tmp;

	double norm_dx = 1.0; 
	double norm_dz = -(m_tris_slice[0]->n->dx*norm_dx) / (m_tris_slice[0]->n->dz);
	double norm_dy = 0.0;
	CVector3D* tmp_norm = new CVector3D(norm_dx, norm_dy, norm_dz);
	tmp_triangle->n = tmp_norm;
	

/////////////////////////////////
// �е�һ����
/////////////////////////////////
	double zmin_turn;
	zmin_turn = getTurnPoint();
	CVector3D turn_vec = getTurnVec();
	if (zmin_turn != 0.0)
	{
		need_support = true;
	}
	if (!need_support)
	{
		begin3DSlice(z_Min, z_Max, z, dz);
	}
	else
	{
		z_Max = zmin_turn;
		begin3DSlice(z_Min, z_Max, z, dz);


		z_Min = zmin_turn + 0.1;
		z_Max = 90.0;
		//z = z_Min;
		while (z < z_Min)
		{
			z += dz;
		}
		begin5DSlice(z_Min, z_Max, z, dz);
	}

/////////////////////////////////
// �������Σ����������д���
/////////////////////////////////
/*	CPoint3D tmp_cal;
	CPoint3D planepoint = CPoint3D(30.0, 0.0, 50.0);
	CVector3D lineVector = CVector3D(3.0, 0.0, 4.0);
	CPoint3D linepoint = CPoint3D(0.0, 0.0, 50.0);
	tmp_cal = ::CalPlaneLineIntersectPoint(*tmp_norm, planepoint, lineVector, linepoint);

	z_Max = tmp_cal.z;

	z = z + dz;
	CPoint3D* tmp_point1 = new CPoint3D();
	while (true)
	{
		Layer* tmp_layer = new Layer();
		CPoint3D tmp_layer_point = CPoint3D(0, 0, z);
		tmp_layer->layer_gravity = gravity;
		tmp_layer->layerPoint = tmp_layer_point;

		m_layers.push_back(tmp_layer);

		getpolylinePoints(m_layers[m_layers.size() - 1]);

////////���������д���
/*		pt_x = 0.0;
		pt_z = z;
		pt_x = 30.0 - (50 - pt_z)* m_tris_slice[0]->n->dx / m_tris_slice[0]->n->dz;
		tmp_point->x = pt_x;
		tmp_point->z = pt_z;
		tmp_point->y = 0.0;
		tmp_point1->x = pt_x;
		tmp_point1->y = 30.0;
		tmp_point1->z = pt_z;
		int j = 0;
		unsigned int szl = m_layers.size();
		for (unsigned int i = 0; i < m_layers[szl - 1]->m_Polylines[0]->m_Linkpoints.size()-1; i++)
		{
			if (m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i]->x == m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i+1]->x
				&&m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i]->y == m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i + 1]->y
				&&m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i]->z == m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i + 1]->z)
			{
				m_layers[szl-1]->m_Polylines[0]->m_Linkpoints.erase(m_layers[szl-1]->m_Polylines[0]->m_Linkpoints.begin()+i+1);
				i = 0;
			}
		}

    	for (unsigned int i = 0; i < m_layers[szl-1]->m_Polylines[0]->m_Linkpoints.size(); i++)
		{
			if (m_layers[szl - 1]->m_Polylines[0]->m_Linkpoints[i]->x > tmp_point->x)
			{
				m_layers[szl - 1]->m_Polylines[0]->m_Linkpoints[i]->x = tmp_point->x;
			}
		}

		for (unsigned int i = 0; i < m_layers[szl-1]->m_Polylines[0]->m_Linkpoints.size() - 1; i++)
		{
			if (m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i]->x == m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i + 1]->x
				&&m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i]->y == m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i + 1]->y
				&&m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i]->z == m_layers[szl-1]->m_Polylines[0]->m_Linkpoints[i + 1]->z)
			{
				m_layers[szl-1]->m_Polylines[0]->m_Linkpoints.erase(m_layers[szl-1]->m_Polylines[0]->m_Linkpoints.begin() + i + 1);
				i = 0;
			}
		}
*/
/*	if (z >= (z_Max - dz))
		{
			break;
		}
		z += dz;
	}*/

}

void CSlice::getpolylinePoints(Layer* layer)
{
	vector<LTriangle*> status; //������ϲ�ߵģ�ɸѡ��������Ƭ
	double v1_dist, v2_dist, v3_dist;


	unsigned int szTri = m_tris_slice.size();
	for (unsigned int i = 0; i < szTri; i++)         //����������Ƭ,ɸѡ���ཻ��Ƭ
	{
		//////ReturnZtype �޷���б��ƽ����Ч���ú����ɿ�����ȥ����Ϊ���������þ���Ϊ���ж�������Ƭ����ƽ���Ƿ��ཻ

		v1_dist = ::CalPointtoPlane(*m_tris_slice[i]->v1, layer->layer_gravity, layer->layerPoint);
		v2_dist = ::CalPointtoPlane(*m_tris_slice[i]->v2, layer->layer_gravity, layer->layerPoint);
		v3_dist = ::CalPointtoPlane(*m_tris_slice[i]->v3, layer->layer_gravity, layer->layerPoint);


		if(!((v1_dist<0&&v2_dist<0&&v3_dist<0)||(v1_dist>0&&v2_dist>0&&v3_dist>0)))
		{
			if ((v1_dist == 0) &&
				(v2_dist == 0) &&
				(v3_dist == 0))
			{
				//������Ƭ����Ƭ�غϵ���Ƭ
			}
			else
			{
				status.push_back(m_tris_slice[i]);
			}
		}
	}

	unsigned int szStatus = status.size();
	for (unsigned int i = 0; i<szStatus; i++)       //���ཻ��Ƭ������Ƭ����
	{
		JudgeFaceType(layer, status[i]);
	}
	LTriangle* pCurFace = new LTriangle();
	//ѡ��һ����Ƭ��Ϊ��ʼ��Ƭ
	pCurFace = status[0];
	pCurFace->b_use = true;//����Ƭ��Ϊ��ǰʹ����Ƭ����־λ��Ϊ��
	while (true)
	{
		PolyLine* m_polyline = new PolyLine();
		InterSect(layer, pCurFace);  //�趨��ʼ�ཻ�ߣ�������m_Slice_edge����
		CPoint3D* tmpLinkPoint = new CPoint3D();
		unsigned int sz0 = m_Slice_edge.size();
		pCurFace->SelectIntersectLine = m_Slice_edge[sz0 - 1];
		//����ʼ�ཻ���󽻵㣬������m_LinkPoint����
		CalIntersectPoint(layer, pCurFace->SelectIntersectLine, pCurFace, tmpLinkPoint);
		m_polyline->m_Linkpoints.push_back(new CPoint3D(*tmpLinkPoint));
		//��һ����Ƭ����һ�ཻ���󽻵㣬������m_LinkPoint����
		JudgeOtherLine(layer, pCurFace);
		CalIntersectPoint(layer, pCurFace->OtherIntersectLine, pCurFace, tmpLinkPoint);
		m_polyline->m_Linkpoints.push_back(new CPoint3D(*tmpLinkPoint));
		//������������һ����Ƭ�����⴦��

		while (true)
		{
			//���������ཻ������ıߵĻ����Ѱ����һ�ཻ��Ƭ
			unsigned int sz = m_Slice_edge.size();
			pCurFace = m_Slice_edge[sz - 1]->e_adja->t;
			pCurFace->b_use = true;//����Ƭ��Ϊ��ǰʹ����Ƭ����־λ��Ϊ��
								   //������m_Slice_edge���ཻ������һ��Ƭ�ĵڶ����ཻ�ߣ����Ļ���߾��ǵ�ǰ��Ƭ��ѡ���ཻ��
			pCurFace->SelectIntersectLine = m_Slice_edge[sz - 1]->e_adja;
			//�жϵ�ǰ��Ƭ�е���һ�ཻ�ߣ������
			JudgeOtherLine(layer, pCurFace);
			//�󽻵㣬�����ֵ�


			CalIntersectPoint(layer, pCurFace->OtherIntersectLine, pCurFace, tmpLinkPoint);
			m_polyline->m_Linkpoints.push_back(new CPoint3D(*tmpLinkPoint));

			unsigned int szlinkPoint = m_polyline->m_Linkpoints.size();
			//�жϽ����Ƿ�Ϊ��ʼ�㣬���ǣ������ڲ�ѭ������ʱ����һ�������㼯������ֱ�ӷ����ڲ�ѭ��
			if (*(m_polyline->m_Linkpoints[0]) ^= *(m_polyline->m_Linkpoints[szlinkPoint - 1]))
			{
				Layer* pLayer = new Layer();
				//pLayer->z = z;
				pLayer->m_Polylines.push_back(m_polyline);
				m_layers[m_layers.size()-1] = pLayer;
//				int szpoint = m_polyline->m_Linkpoints.size();
				break;
			}
		}

		//�����ڲ�ѭ�������⣬�ж�status���Ƿ���δ��ʹ�õ���Ƭ��
		//���У�����Ƭ��Ϊ��ǰ����ʼ��һ����Ѱ�㣻��û�У��������ѭ��
		break;
	}

}

void CSlice::drawpolyline(double color[])
{
	unsigned int szlayer = m_layers.size();
	for (unsigned int i = 0; i<szlayer; i++)
	{
		unsigned int szpolyline = m_layers[i]->m_Polylines.size();
		for (unsigned int j = 0; j<szpolyline; j++)
		{
			unsigned int szPoint = m_layers[i]->m_Polylines[j]->m_Linkpoints.size();
			for (unsigned int k = 0; k<(szPoint-1); k++)
			{
				CPoint3D point1 = CPoint3D(*(m_layers[i]->m_Polylines[j]->m_Linkpoints[k]));
				CPoint3D point2 = CPoint3D(*(m_layers[i]->m_Polylines[j]->m_Linkpoints[k + 1]));
				glLineWidth(1.5f);
				glBegin(GL_LINES);
				glColor3f(color[0], color[1], color[2]);
				glVertex3f(point1.x, point1.y, point1.z);
				glVertex3f(point2.x, point2.y, point2.z);
				glEnd();
			}
		}
	}

}

void CSlice::InterSect(Layer* layer, LTriangle* pCurFace)   //ֻҪ�ҵ�һ���ཻ�ı߾Ϳ�������
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

	v1_plane = PointtoPlane(v1_tmp, layer->layer_gravity, layer->layerPoint);
	v2_plane = PointtoPlane(v2_tmp, layer->layer_gravity, layer->layerPoint);
	v3_plane = PointtoPlane(v3_tmp, layer->layer_gravity, layer->layerPoint);

	switch (pCurFace->FaceType)
	{
	case NO_POINT_ON_SURFACE:   //���ཻ�ı߲���ѡ
	{
		if ((v1_plane*v2_plane)<0)   //˵��e1�߿�����Ϊ��һ���ཻ�ı�
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT1;
		}
		if ((v2_plane*v3_plane)<0)   //˵��e2�߿�����Ϊ��һ���ཻ�ı�,��һ���߲��ཻ���ڶ����߿϶��ཻ
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
		if (v1_plane != 0)   //v1������ƽ���ϣ���֮������������e1\e3����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT2;
		}
		if (v2_plane != 0)   //v2������ƽ���ϣ���֮������������e1\e2����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT2;
		}
		if (v3_plane != 0)   //v3������ƽ���ϣ���֮������������e2\e3����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e3);
			goto ENDINIT2;
		}
	ENDINIT2:
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:
	{
		if (v1_plane == 0)   //v1����ƽ���ϣ���֮������������e1\e3����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT3;
		}
		if (v2_plane == 0)   //v2����ƽ���ϣ���֮������������e1\e2����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT3;
		}
		if (v3_plane == 0)   //v3����ƽ���ϣ���֮������������e2\e3����ѡ
		{
			m_Slice_edge.push_back(pCurFace->e3);
			goto ENDINIT3;
		}
	ENDINIT3:
		break;
	}
	}

}

void CSlice::JudgeFaceType(Layer* layer, LTriangle* pCurFace)
{
	double min_dist, mid_dist, max_dist, v1_dist, v2_dist, v3_dist;
	v1_dist = CalPointtoPlane(*pCurFace->v1, layer->layer_gravity, layer->layerPoint);
	v2_dist = CalPointtoPlane(*pCurFace->v2, layer->layer_gravity, layer->layerPoint);
	v3_dist = CalPointtoPlane(*pCurFace->v3, layer->layer_gravity, layer->layerPoint);

	min_dist = ReturnZtype(v1_dist, v2_dist, v3_dist, Z_MIN);
	mid_dist = ReturnZtype(v1_dist, v2_dist, v3_dist, Z_MID);
	max_dist = ReturnZtype(v1_dist, v2_dist, v3_dist, Z_MAX);

	//�ж�������Ƭ����ƽ����ཻ���,�������
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

void CSlice::JudgeOtherLine(Layer* layer, LTriangle* pCurFace)
{
	int v1_plane, v2_plane;
	CPoint3D v1_tmp, v2_tmp;

	v1_tmp.x = pCurFace->SelectIntersectLine->e_prev->v1->x;
	v1_tmp.y = pCurFace->SelectIntersectLine->e_prev->v1->y;
	v1_tmp.z = pCurFace->SelectIntersectLine->e_prev->v1->z;

	v2_tmp.x = pCurFace->SelectIntersectLine->e_prev->v2->x;
	v2_tmp.y = pCurFace->SelectIntersectLine->e_prev->v2->y;
	v2_tmp.z = pCurFace->SelectIntersectLine->e_prev->v2->z;

	v1_plane = PointtoPlane(v1_tmp, layer->layer_gravity, layer->layerPoint);
	v2_plane = PointtoPlane(v2_tmp, layer->layer_gravity, layer->layerPoint);

	switch (pCurFace->FaceType)
	{
	case NO_POINT_ON_SURFACE:
	{
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
		if ((v1_plane * v2_plane)<0)
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		else//������һ�������ཻ��
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		break;
	}
	case ONE_POINT_ON_SURFACE:  //��������Ƚ����⣬Ҫ���ж���ѡ���ཻ������������
	{
		//��ѡ�ߵ�һ�˵�����ƽ����
		if ((v1_plane*v2_plane) == 0)
		{
			//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
			if ((v1_plane*v2_plane)<0)
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
		if ((v1_plane != 0) || (v2_plane != 0))
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		else//������һ�������ཻ��
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:  //��һ�������е�һ���˵�����ƽ���ϣ������ཻ��
	{

		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ���һ���㲻��z��ƽ����
		if (v1_plane == 0)
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		else//������һ�������ཻ��
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		break;
	}
	}
	m_Slice_edge.push_back(pCurFace->OtherIntersectLine);
}


void CSlice::CalIntersectPoint(Layer* layer, LEdge * edge, LTriangle*pCurFace, CPoint3D* point)
{
	int e1_plane, e2_plane;
	CPoint3D e1_tmp, e2_tmp;

	e1_tmp.x = edge->v1->x;
	e1_tmp.y = edge->v1->y;
	e1_tmp.z = edge->v1->z;

	e2_tmp.x = edge->v2->x;
	e2_tmp.y = edge->v2->y;
	e2_tmp.z = edge->v2->z;

	e1_plane = PointtoPlane(e1_tmp, layer->layer_gravity, layer->layerPoint);
	e2_plane = PointtoPlane(e2_tmp, layer->layer_gravity, layer->layerPoint);

	if (pCurFace->FaceType != ONLY_ONE_POINT_ON_SURFACE) {
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

			p = ::CalPlaneLineIntersectPoint(layer->layer_gravity, layer->layerPoint, vec, e1_tmp);

			point->x = p.x;
			point->y = p.y;
			point->z = p.z;
		}

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

double CSlice::ReturnZtype(double v1, double v2, double v3, int ztype)
{
	double z1, z2, z3;
	z1 = v1;
	z2 = v2;
	z3 = v3;
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
	if (ztype == 1)
	{
		return z1;
	}
	else if (ztype == 2)
	{
		return z2;
	}
	else
	{
		return z3;
	}
}

void CSlice::begin3DSlice(double z_min, double z_max, double &z, double dz)
{
	while (true)
	{
		Layer* tmp_layer = new Layer();
		CPoint3D tmp_layer_point = CPoint3D(0, 0, z);
		tmp_layer->layer_gravity = gravity;
		tmp_layer->layerPoint = tmp_layer_point;

		m_layers.push_back(tmp_layer);

		getpolylinePoints(m_layers[m_layers.size() - 1]);

		if (z >= (z_max - dz))
		{
			break;
		}
		z += dz;
	}
}

void CSlice::begin5DSlice(double z_min, double z_max, double& z, double dz)
{
	Layer* tmp_layer = new Layer();
	CPoint3D tmp_layer_point = CPoint3D(0, 0, 0);
	vector<CPoint3D*> cross_point;
	double x_max;
	while (true)
	{
		tmp_layer_point = CPoint3D(0, 0, z);
		tmp_layer->layer_gravity = gravity;
		tmp_layer->layerPoint = tmp_layer_point;

		m_layers.push_back(tmp_layer);

		getpolylinePoints(m_layers[m_layers.size() - 1]);

		// ��ȡ�������Բ������������ƫ��
		unsigned int sz = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.size();
		x_max = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]->x;
		for (unsigned int i = 0; i < sz; i++)
		{
			if (x_max <= m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x)
				x_max = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x;
		}

		bool isCCW = isBoundaryCCW(m_layers[m_layers.size() - 1]);
		if (!isCCW)
		{
			makeBoundaryCCW(m_layers[m_layers.size() - 1]);
		}

		for (unsigned int i = 0; i < sz; i++)
		{
			if (m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x == x_max)// �ж���Ҫ�޸�
			{
				m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x -= 2.0;
				cross_point.push_back(m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]);
				tmp_layer_point = *m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i];
			}
		}
		cross_point.erase(cross_point.begin(),cross_point.begin()+1);

		// б���������Ƭ
		tmp_layer->layer_gravity = CVector3D(3,0,4);
		tmp_layer->layerPoint = tmp_layer_point;

		m_layers.push_back(tmp_layer);

		getpolylinePoints(m_layers[m_layers.size() - 1]);

		if (!isBoundaryCCW(m_layers[m_layers.size() - 1]))
		{
			makeBoundaryCCW(m_layers[m_layers.size() - 1]);
		}

		unsigned int tmp_sz = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.size();
		for (unsigned int i = 0; i < m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.size(); i++)
		{
			if (m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->z > tmp_layer_point.z)
			{
				m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.erase(m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.begin() + i);
				i = 0;
			}
		}

		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.erase(m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.begin(), m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.begin() + 1);
		for (unsigned int i = 0; i < cross_point.size(); i++)
		{
			m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(cross_point[cross_point.size()-i-1]);
		}
		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]);


		cross_point.clear();

		if (z >= (z_max - dz))
		{
			break;
		}
		z += dz;
	}
}

bool CSlice::isBoundaryCCW(Layer* layer)
{
	unsigned int sz = layer->m_Polylines[0]->m_Linkpoints.size();
	int i = 0;
	double angle = 0.0;
	CVector3D vec, vec1, vec2;
	if (sz < 3)
	{
		return false;
	}
	else
	{
		while (angle==0.0)
		{
			vec1 = CVector3D(*layer->m_Polylines[0]->m_Linkpoints[i], *layer->m_Polylines[0]->m_Linkpoints[i+1]);
			vec2 = CVector3D(*layer->m_Polylines[0]->m_Linkpoints[i+1], *layer->m_Polylines[0]->m_Linkpoints[i+2]);
			if (vec1.dz < 0.00001&&vec1.dz>-0.00001)
			{
				vec1.dz = 0;
			}
			if (vec2.dz < 0.00001&&vec2.dz>-0.00001)
			{
				vec2.dz = 0;
			}
			angle = ::GetAngle(vec1, vec2);
			i += 1;
		}
		vec = vec1*vec2;
		if (vec.dz < 0.00001&&vec.dz>-0.00001)
		{
			vec.dz = 0;
		}
		angle = ::GetAngle(vec, layer->layer_gravity);
		if (angle > (PI / 2))
			return false;
		else
			return true;
	}
}

void CSlice::makeBoundaryCCW(Layer* layer)
{
	unsigned int sz = layer->m_Polylines[0]->m_Linkpoints.size();
	vector<CPoint3D*> tmp;
	for (unsigned int i = 0; i < sz; i++)
	{
		tmp.push_back(layer->m_Polylines[0]->m_Linkpoints[sz - 1 - i]);
	}
	for (unsigned int i = 0; i < sz; i++)
	{
		layer->m_Polylines[0]->m_Linkpoints[i] = tmp[i];
	}

}

bool CSlice::lineNeedSupport()
{
	return false;
}




