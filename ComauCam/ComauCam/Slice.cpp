#include "stdafx.h"
#include "Slice.h"
#include <algorithm>

LPoint::LPoint()
{
	p_prev = NULL;
	p_next = NULL;
}
LPoint::~LPoint()
{
}
LPoint::LPoint(const LPoint& lpoint)
{
	x = lpoint.x;
	y = lpoint.y;
	z = lpoint.z;
}
LPoint::LPoint(const CPoint3D & pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
}
LPoint::LPoint(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

LLine::LLine()
{
	pstart = LPoint(0,0,0);
	pend = LPoint(0,0,0);
	layer = NULL;
}
LLine::LLine(const LPoint* startpoint, const LPoint* endpoint)
{
	pstart = *startpoint;
	pend = *endpoint;
}

LLine::~LLine()
{
}

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
	layer_coordinate[0] = CVector3D(1, 0, 0);
	layer_coordinate[1] = CVector3D(0, 1, 0);
	layer_coordinate[2] = CVector3D(0, 0, 1);
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

void CSlice::loadSTLModel(CSTLModel* model)//����stlģ��
{
	model->Topologize();//��ģ�ͽ������˽ṹ
	unsigned int szTri = model->m_ltris.size();
	for (unsigned int j = 0; j<szTri; j++)
	{
		m_tris_slice.push_back(model->m_ltris[j]);     //ΪCSlice���󿽱�һ��������Ƭ����
	}
}

double CSlice::getTurnHeight()
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
		//zmin_turn = ReturnZmin(m_tris_slice[0]);
		zmin_turn = compareThreeNumber(m_tris_slice[0]->v1->z, m_tris_slice[0]->v2->z, m_tris_slice[0]->v3->z, MIN);
		return zmin_turn;
	}
}

// ��ȡ����̬��ķ�����
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
	double z_Min, z_Max, z, dz, zmin_turn; //ģ�� z �������Сֵ����ǰ��Ƭƽ�� z ֵ�����и߶ȣ�ģ��ת�۴��� z ֵ������ж��ת�ۣ�������Ҫһ�� zmin_turn[]
	double ex[6];
	model->FindExtreme(ex); //����ģ�͵ļ��޳ߴ�
	z_Min = ex[4];
	dz = height;
	z = z_Min + dz;  //��һ����Ƭ�㣬�޷���Ϊ z_Min

	zmin_turn = getTurnHeight();
	if (zmin_turn != 0.0)
	{
		need_support = true;
	}
	if (!need_support)
	{
		z_Max = ex[5];
		begin3DSlice(z_Min, z_Max, z, dz);
	}
	else
	{
		z_Max = zmin_turn;
		begin3DSlice(z_Min, z_Max, z, dz);

		z_Min = zmin_turn + 0.1;
		z_Max = ex[5];

		while (z < z_Min)
		{
			z += dz;
		}
		begin5DSlice(z_Min, z_Max, z, dz);
	}
}
 
void CSlice::begin3DSlice(double z_min, double z_max, double &z, double dz)
{
	while (true)
	{
		Layer* tmp_layer = new Layer();
		CPoint3D tmp_layer_point = CPoint3D(0, 0, z);

		tmp_layer->layerPoint = tmp_layer_point;

		m_layers.push_back(tmp_layer);

		getPolylinePoints(m_layers[m_layers.size() - 1]);

		bool isCCW = isBoundaryCCW(m_layers[m_layers.size() - 1]);
		if (!isCCW)
		{
			makeBoundaryCCW(m_layers[m_layers.size() - 1]);
		}
		deletePoints(m_layers[m_layers.size() - 1]);

		if (z >= (z_max - dz))
		{
			break;
		}
		z += dz;
	}
}

void CSlice::begin5DSlice(double z_min, double z_max, double& z, double dz)
{

	CPoint3D tmp_layer_point = CPoint3D(0, 0, 0);
	vector<LPoint*> cross_point;	// ���ڴ�� z ����ƽ���Ͻ��й�ƫ�Ƶ�������
	double x_max;
	while (true)
	{
		Layer* tmp_layer = new Layer();
		tmp_layer_point = CPoint3D(0, 0, z);
//		tmp_layer->layer_gravity = gravity;
		tmp_layer->layerPoint = tmp_layer_point;

		m_layers.push_back(tmp_layer);

		getPolylinePoints(m_layers[m_layers.size() - 1]);
		deletePoints(m_layers[m_layers.size() - 1]);

		// ��ȡ�������Բ������������ƫ��
		unsigned int sz = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.size();
		x_max = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]->x;
		for (unsigned int i = 0; i < sz; i++)
		{
			if (x_max <= m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x)
				x_max = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x;
		}

		// �������ϵĵ���ʱ���Ų�
		bool isCCW = isBoundaryCCW(m_layers[m_layers.size() - 1]);
		if (!isCCW)
		{
			makeBoundaryCCW(m_layers[m_layers.size() - 1]);
		}

		double offset = EXTRUDER_DIAMETER * 2 * 5.0 / 4.0;

		for (unsigned int i = 0; i < sz; i++)
		{
			if (m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x >=30)// �ж���Ҫ�޸�
			{
				m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x = 30;
				cross_point.push_back(m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]);
				tmp_layer_point = *m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i];
			}
		}
//		cross_point.erase(cross_point.begin(), cross_point.begin() + 1);
		LPoint* y_min = new LPoint();
//		*y_min = LPoint(*cross_point[0]);
		LPoint* y_max = new LPoint();
//		*y_max = LPoint(*cross_point[0]);
		y_min->x = cross_point[0]->x;
		y_min->y = cross_point[0]->y;
		y_min->z = cross_point[0]->z;
		y_max->x = cross_point[0]->x;
		y_max->y = cross_point[0]->y;
		y_max->z = cross_point[0]->z;
		for (unsigned int i = 1; i < cross_point.size(); i++)
		{
			if (y_min->y > cross_point[i]->y)
			{
				y_min->x = cross_point[i]->x;
				y_min->y = cross_point[i]->y;
				y_min->z = cross_point[i]->z;
			}
			if (y_max->y < cross_point[i]->y)
			{
				y_max->x = cross_point[i]->x;
				y_max->y = cross_point[i]->y;
				y_max->z = cross_point[i]->z;
			}
		}

		// б���������Ƭ
		Layer* turn_layer = new Layer();
		turn_layer->layer_coordinate[2] = getTurnVec();
		turn_layer->layer_coordinate[1] = CVector3D(0, 1, 0);
		turn_layer->layer_coordinate[0] = turn_layer->layer_coordinate[1] * turn_layer->layer_coordinate[2];
		turn_layer->layerPoint = tmp_layer_point;

		m_layers.push_back(turn_layer);

		getPolylinePoints(m_layers[m_layers.size() - 1]);

		if (!isBoundaryCCW(m_layers[m_layers.size() - 1]))
		{
			makeBoundaryCCW(m_layers[m_layers.size() - 1]);
		}
		deletePoints(m_layers[m_layers.size() - 1]);

		// ��������Ƭƽ�����ɵ������ϲ�
		unsigned int tmp_sz = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.size();
		for (unsigned int i = 0; i < m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.size(); i++)
		{
			if(m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->z > tmp_layer_point.z)
			{
				m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.erase(m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.begin() + i);
				i = 0;
			}
		}

		LPoint* turn_min = new LPoint();
		LPoint* turn_max = new LPoint();

		turn_min->x = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]->x;
		turn_min->y = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]->y;
		turn_min->z = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]->z;
		turn_max->x = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]->x;
		turn_max->y = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]->y;
		turn_max->z = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]->z;
		for (unsigned int i = 1; i < m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.size(); i++)
		{
			if (turn_min->y > m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->y)
			{
				turn_min->x = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x;
				turn_min->y = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->y;
				turn_min->z = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->z;
			}
			if (turn_max->y < m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->y)
			{
				turn_max->x = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->x;
				turn_max->y = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->y;
				turn_max->z = m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[i]->z;
			}
		}
		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.clear();
		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(y_min);
		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(turn_min);
		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(turn_max);
		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(y_max);
		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(y_min);
/*		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.erase(m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.begin(), m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.begin() + 1);
		for (unsigned int i = 0; i < cross_point.size(); i++)
		{
			//m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(cross_point[cross_point.size() - i - 1]);
			m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(y_max);
			m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(y_min);
		}*/

		m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints.push_back(m_layers[m_layers.size() - 1]->m_Polylines[0]->m_Linkpoints[0]);

		cross_point.clear();

		if (z >= (z_max - dz))
		{
			break;
		}
		z += dz;
	}
}

void CSlice::getPolylinePoints(Layer* layer)
{
	// ɸѡ�ཻ��Ƭ
	vector<LTriangle*> status;
	double v1_dist, v2_dist, v3_dist;

	unsigned int szTri = m_tris_slice.size();
	for (unsigned int i = 0; i < szTri; i++)         //����������Ƭ,ɸѡ���ཻ��Ƭ
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
				//������Ƭ����Ƭ�غϵ���Ƭ
			}
			else
			{
				status.push_back(m_tris_slice[i]);
			}
		}
	}

	// ���ཻ��Ƭ������Ƭ����
	unsigned int szStatus = status.size();
	for (unsigned int i = 0; i<szStatus; i++)    
	{
		judgeFaceType(layer, status[i]);
	}

	// ����Ƭƽ����������Ƭ����
	LTriangle* pCurFace = new LTriangle();

	//ѡ��һ����Ƭ��Ϊ��ʼ��Ƭ
	pCurFace = status[0];
	pCurFace->b_use = true;//����Ƭ��Ϊ��ǰʹ����Ƭ����־λ��Ϊ��
	
	while (true)
	{
		PolyLine* m_polyline = new PolyLine();
		LPoint* tmpLinkPoint = new LPoint();
		getInterSectEdge(layer, pCurFace);  //�趨��ʼ�ཻ�ߣ�������m_Slice_edge����	
		unsigned int sz0 = m_Slice_edge.size();
		pCurFace->IntersectLine1 = m_Slice_edge[sz0 - 1];
		//����ʼ�ཻ���󽻵㣬������m_LinkPoint����
		calIntersectPoint(layer, pCurFace->IntersectLine1, pCurFace, tmpLinkPoint);
		m_polyline->m_Linkpoints.push_back(new LPoint(*tmpLinkPoint));

		//��һ����Ƭ����һ�ཻ���󽻵㣬������m_LinkPoint����
		judgeOtherLine(layer, pCurFace);
		calIntersectPoint(layer, pCurFace->IntersectLine2, pCurFace, tmpLinkPoint);
		m_polyline->m_Linkpoints.push_back(new LPoint(*tmpLinkPoint));
		//������������һ����Ƭ�����⴦��

		while (true)
		{
			//���������ཻ������ıߵĻ����Ѱ����һ�ཻ��Ƭ
			unsigned int sz = m_Slice_edge.size();
			pCurFace = m_Slice_edge[sz - 1]->e_adja->t;
			pCurFace->b_use = true;//����Ƭ��Ϊ��ǰʹ����Ƭ����־λ��Ϊ��
								   //������m_Slice_edge���ཻ������һ��Ƭ�ĵڶ����ཻ�ߣ����Ļ���߾��ǵ�ǰ��Ƭ��ѡ���ཻ��
			pCurFace->IntersectLine1 = m_Slice_edge[sz - 1]->e_adja;
			//�жϵ�ǰ��Ƭ�е���һ�ཻ�ߣ������
			judgeOtherLine(layer, pCurFace);
			//�󽻵㣬�����ֵ�


			calIntersectPoint(layer, pCurFace->IntersectLine2, pCurFace, tmpLinkPoint);
			m_polyline->m_Linkpoints.push_back(new LPoint(*tmpLinkPoint));

			unsigned int szlinkPoint = m_polyline->m_Linkpoints.size();
			//�жϽ����Ƿ�Ϊ��ʼ�㣬���ǣ������ڲ�ѭ������ʱ����һ�������㼯������ֱ�ӷ����ڲ�ѭ��
			if (*(m_polyline->m_Linkpoints[0]) ^= *(m_polyline->m_Linkpoints[szlinkPoint - 1]))
			{
				Layer* pLayer = new Layer();
				
				pLayer->layer_coordinate[2] = m_layers[m_layers.size() - 1]->layer_coordinate[2];
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

void CSlice::drawLayer(bool showPolygon, int start, int end)
{
	double color[3];
	unsigned int szlayer = m_layers.size();
	LPoint point, point2;
	for (int i = (start-1); i<end; i++)
	{
		unsigned int szpolyline = m_layers[i]->m_Polylines.size();
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
			unsigned int szPoint = m_layers[i]->m_Polylines[j]->m_Linkpoints.size();

			glLineWidth(1.5f);
			if (showPolygon)
			{
				glBegin(GL_POLYGON);
//				glPolygonMode(GL_FRONT, GL_FILL);
//				glPolygonMode(GL_BACK, GL_LINE);
				glColor3f(color[0], color[1], color[2]);			
				for (unsigned int k = 0; k < szPoint; k++)
				{
					point = *m_layers[i]->m_Polylines[j]->m_Linkpoints[k];
					glVertex3f(point.x, point.y, point.z);
				}
				glEnd();
			}

			glBegin(GL_LINE_LOOP);
			glColor3f(color[0], color[1], color[2]);
			for (unsigned int k = 0; k<szPoint; k++)
			{
				point = *m_layers[i]->m_Polylines[j]->m_Linkpoints[k];				
				glVertex3f(point.x, point.y, point.z);				
			}
			glEnd();
		}
	}

}

void CSlice::getInterSectEdge(Layer* layer, LTriangle* pCurFace)   //ֻҪ�ҵ�һ���ཻ�ı߾Ϳ�������
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

void CSlice::judgeFaceType(Layer* layer, LTriangle* pCurFace)
{
	double min_dist, mid_dist, max_dist, v1_dist, v2_dist, v3_dist;
	v1_dist = CalPointtoPlane(*pCurFace->v1, layer->layer_coordinate[2], layer->layerPoint);
	v2_dist = CalPointtoPlane(*pCurFace->v2, layer->layer_coordinate[2], layer->layerPoint);
	v3_dist = CalPointtoPlane(*pCurFace->v3, layer->layer_coordinate[2], layer->layerPoint);

	min_dist = compareThreeNumber(v1_dist, v2_dist, v3_dist, MIN);
	mid_dist = compareThreeNumber(v1_dist, v2_dist, v3_dist, MID);
	max_dist = compareThreeNumber(v1_dist, v2_dist, v3_dist, MAX);

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

void CSlice::judgeOtherLine(Layer* layer, LTriangle* pCurFace)
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
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
		if ((v1_plane * v2_plane)<0)
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
		else//������һ�������ཻ��
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_next;
		break;
	}
	case ONE_POINT_ON_SURFACE:  //��������Ƚ����⣬Ҫ���ж���ѡ���ཻ������������
	{
		//��ѡ�ߵ�һ�˵�����ƽ����
		if ((v1_plane*v2_plane) == 0)
		{
			//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ��ཻ
			if ((v1_plane*v2_plane)<0)
				pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
			else//������һ�������ཻ��
				pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_next;
		}
		else//��ѡ�ߵ�һ�˵㲻����ƽ���ϣ�/����һ�����ڵı߶�������Ϊ��һ�ཻ��
		{
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
		}
		break;
	}
	case EDGE_ON_SURFACE:  //�ཻ����ѡ����ƽ���ϵı�
	{
		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ���һ���㲻��z��ƽ����
		if ((v1_plane != 0) || (v2_plane != 0))
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
		else//������һ�������ཻ��
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_next;
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:  //��һ�������е�һ���˵�����ƽ���ϣ������ཻ��
	{

		//��ѡ���ཻ�ߵ�����ǰһ���ж��Ƿ���һ���㲻��z��ƽ����
		if (v1_plane == 0)
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_prev;
		else//������һ�������ཻ��
			pCurFace->IntersectLine2= pCurFace->IntersectLine1->e_next;
		break;
	}
	}
	m_Slice_edge.push_back(pCurFace->IntersectLine2);
}


void CSlice::calIntersectPoint(Layer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point)
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

			p = ::CalPlaneLineIntersectPoint(layer->layer_coordinate[2], layer->layerPoint, vec, e1_tmp);

			point->x = p.x;
			point->y = p.y;
			point->z = p.z;
		}

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
		angle = ::GetAngle(vec, layer->layer_coordinate[2]);
		if (angle > (PI / 2))
			return false;
		else
			return true;
	}
}

void CSlice::makeBoundaryCCW(Layer* layer)
{
	unsigned int sz = layer->m_Polylines[0]->m_Linkpoints.size();
	vector<LPoint*> tmp;
	for (unsigned int i = 0; i < sz; i++)
	{
		tmp.push_back(layer->m_Polylines[0]->m_Linkpoints[sz - 1 - i]);
	}
	for (unsigned int i = 0; i < sz; i++)
	{
		layer->m_Polylines[0]->m_Linkpoints[i] = tmp[i];
	}

}

void CSlice::deletePoints(Layer * layer)
{
	vector<LPoint*> tmp_points;
	unsigned int szP = layer->m_Polylines[0]->m_Linkpoints.size();

	// ȡ�����������������е����н���
	for (unsigned int j = 0; j < szP; j++)
	{
		tmp_points.push_back(layer->m_Polylines[0]->m_Linkpoints[j]);
	}
	layer->m_Polylines[0]->m_Linkpoints.clear();

	unsigned int szTemp = tmp_points.size();
	for (unsigned int i = 0; i < szTemp; i++)
	{
		if (tmp_points[i]->x <= 0.0000001)
			tmp_points[i]->x = 0;
		if (tmp_points[i]->y <= 0.0000001)
			tmp_points[i]->y = 0;
		if (tmp_points[i]->z <= 0.0000001)
			tmp_points[i]->z = 0;
	}
	CPoint3D pCur;
	CPoint3D pNext;
	CPoint3D pNextNext;
//	CVector3D vec1, vec2;
	while (szTemp >= 3)
	{
		for (unsigned int j = 0; j < szTemp; j++)
		{
			pCur = *tmp_points[j%szTemp];
			pNext = *tmp_points[(j + 1) % szTemp];
			pNextNext = *tmp_points[(j + 2) % szTemp];
			double distance = CalPointtoLine(pNext, pCur, pNextNext);
			if (distance <= 0.0000001)
			{
				tmp_points.erase(tmp_points.begin() + (j + 1) % szTemp);
				szTemp = tmp_points.size();
				j = -1;
//				continue;
			}
		}
		break;
	}

	//��ɾ�����ߵ�֮��ĵ��� m_Linkpoints ��
	for (unsigned int i = 0; i < tmp_points.size(); i++)
	{
		layer->m_Polylines[0]->m_Linkpoints.push_back(tmp_points[i]);
	}

}

bool CSlice::lineNeedSupport()
{
	return false;
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


