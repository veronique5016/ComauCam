#include "stdafx.h"
#include "Slice.h"
#include <algorithm>
#include <math.h>

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

//��ȡ��Сת�۵�� z ����
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
		zmin_turn = compareThreeNumber(z_tris[0]->v1->z, z_tris[0]->v2->z, z_tris[0]->v3->z, MIN);
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
	int test = 0;
}
 
void CSlice::begin3DSlice(double z_min, double z_max, double &z, double dz)
{
	while (true)
	{
		SliceLayer* tmp_layer = new SliceLayer();

		tmp_layer->layerPoint = CPoint3D(0, 0, z);

		m_layers.push_back(tmp_layer);

		getBoundaryPoints(m_layers[m_layers.size() - 1]);

		bool isCCW = isBoundaryCCW(m_layers[m_layers.size() - 1]);
		if (!isCCW)
		{
			makeBoundaryCCW(m_layers[m_layers.size() - 1]);
		}
		deletePoints(m_layers[m_layers.size() - 1]);

		if (z >= (z_max -dz) )
		{
			SliceLayer* tmp_layer = new SliceLayer();
			CPoint3D tmp_layer_point = CPoint3D(0, 0, z_max);

			tmp_layer->layerPoint = CPoint3D(0, 0, z_max);

			m_layers.push_back(tmp_layer);

			getBoundaryPoints(m_layers[m_layers.size() - 1]);

			bool isCCW = isBoundaryCCW(m_layers[m_layers.size() - 1]);
			if (!isCCW)
			{
				makeBoundaryCCW(m_layers[m_layers.size() - 1]);
			}
			deletePoints(m_layers[m_layers.size() - 1]);
			break;
		}
		z += dz;
	}
}

void CSlice::begin5DSlice(double z_min, double z_max, double& z, double dz)
{

	CPoint3D tmp_layer_point = CPoint3D(0, 0, 0);
	double x_max;
	while (true)
	{
		SliceLayer* tmp_layer = new SliceLayer();

		tmp_layer->layerPoint = CPoint3D(0, 0, z);

		m_layers.push_back(tmp_layer);

		getBoundaryPoints(m_layers[m_layers.size() - 1]);
		//ɾ�����ߵ�
		deletePoints(m_layers[m_layers.size() - 1]);

	 // ��ȡ�������Բ������������ƫ��
		//��ȡ��һ����Ƭ������ x �����ֵ
		unsigned int sz = m_layers[m_layers.size() - 2]->m_Boundaries[0]->m_segments.size();
		x_max = m_layers[m_layers.size() - 2]->m_Boundaries[0]->m_segments[0]->pstart.x;
		for (unsigned int i = 0; i < sz; i++)
		{
			if (x_max <= m_layers[m_layers.size() - 2]->m_Boundaries[0]->m_segments[i]->pend.x)
				x_max = m_layers[m_layers.size() - 2]->m_Boundaries[0]->m_segments[i]->pend.x;
		}

		// �������ϵĵ���ʱ���Ų�
		bool isCCW = isBoundaryCCW(m_layers[m_layers.size() - 1]);
		if (!isCCW)
		{
			makeBoundaryCCW(m_layers[m_layers.size() - 1]);
		}

		//������һ�� x �����ֵȷ��ƫ��ֵ
		unsigned int sztemp = m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments.size();
		for (unsigned int i = 0; i < sztemp; i++)
		{
			if (m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[i]->pstart.x >=x_max)// �ж���Ҫ�޸�
			{
				m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[i]->pstart.x = x_max;
				tmp_layer_point = m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[i]->pstart;
			}
			if (m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[i]->pend.x >= x_max)
			{
				m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[i]->pend.x = x_max;
				tmp_layer_point = m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[i]->pend;
			}
		}

		// б���������Ƭ
		SliceLayer* turn_layer = new SliceLayer();
		turn_layer->layer_coordinate[2] = getTurnVec();
		turn_layer->layer_coordinate[1] = CVector3D(0, 1, 0);
		turn_layer->layer_coordinate[0] = turn_layer->layer_coordinate[1] * turn_layer->layer_coordinate[2];
		turn_layer->layerPoint = CPoint3D(tmp_layer_point);

		m_layers.push_back(turn_layer);

		getBoundaryPoints(m_layers[m_layers.size() - 1]);

		if (!isBoundaryCCW(m_layers[m_layers.size() - 1]))
		{
			makeBoundaryCCW(m_layers[m_layers.size() - 1]);
		}
		deletePoints(m_layers[m_layers.size() - 1]);


		CPoint3D p1, p2;
		int index = 0;
		// ��������Ƭƽ�����ɵ������ϲ�
		unsigned int szTemp= m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments.size();
		for (unsigned int i = 0; i < szTemp; i++)
		{
			p1 = m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[i]->pstart;
			p2 = m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[i]->pend;
			if (CalPointtoLine(tmp_layer_point, p1, p2)!=0.0)
				continue;
			index = i;
			break;
		}
		int sum = 0;
		double z_last = tmp_layer_point.z;

		while (sum < szTemp)
		{
			if (m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pstart.z <= z_last
				&&m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pend.z <= z_last)
			{
				//*m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pstart = tmp_layer_point;
			}
			else if (m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pstart.z <= z_last
				&&m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pend.z >= z_last)
			{
				m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pend = LPoint(tmp_layer_point);
			}
			else if (m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pstart.z >= z_last
				&&m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pend.z >= z_last)
			{
				m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pstart = LPoint(tmp_layer_point);
				m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index + sum) % szTemp]->pend = LPoint(tmp_layer_point);
				m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index + sum) % szTemp]->pend.y -= 30.0;
			}			
			else if (m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index + sum) % szTemp]->pstart.z >= z_last
				&&m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index + sum) % szTemp]->pend.z <= z_last)
			{
				m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index+sum)%szTemp]->pstart = LPoint(tmp_layer_point);
				m_layers[m_layers.size() - 1]->m_Boundaries[0]->m_segments[(index + sum) % szTemp]->pstart.y -= 30.0;
			}
			sum += 1;
		}

		if (z >= (z_max - dz))
		{
			break;
		}
		z += dz;
	}
}

void CSlice::getBoundaryPoints(SliceLayer* layer)
{
	// ɸѡ�ཻ��Ƭ
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
				//������Ƭ����Ƭ�غϵ���Ƭ
			}
			else
				status.push_back(m_tris_slice[i]);
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
	pCurFace->b_use = true;
	
	while (true)
	{
		Boundary* m_boundary = new Boundary();
		LPoint* tmpstartPoint = new LPoint();
		LPoint* tmpendPoint = new LPoint();
		//Segment* tmpSegment = new Segment();

		//�趨��ʼ�ཻ�ߣ�������m_Slice_edge����	
		getInterSectEdge(layer, pCurFace);  
		unsigned int sz0 = m_Slice_edge.size();
		pCurFace->IntersectLine1 = m_Slice_edge[sz0 - 1];

		//����ʼ�ཻ���󽻵㣬������m_LinkPoint����
		calIntersectPoint(layer, pCurFace->IntersectLine1, pCurFace, tmpstartPoint);

		//��һ����Ƭ����һ�ཻ���󽻵㣬������m_LinkPoint����
		judgeOtherLine(layer, pCurFace);
		calIntersectPoint(layer, pCurFace->IntersectLine2, pCurFace, tmpendPoint);

		Segment* tmpSegment = new Segment(*tmpstartPoint, *tmpendPoint, pCurFace);
		m_boundary->m_segments.push_back(new Segment(*tmpSegment));
		//������������һ����Ƭ�����⴦��

		while (true)
		{
			//���������ཻ������ıߵĻ����Ѱ����һ�ཻ��Ƭ
			unsigned int sz = m_Slice_edge.size();
			pCurFace = m_Slice_edge[sz - 1]->e_adja->t;
			pCurFace->b_use = true;//����Ƭ��Ϊ��ǰʹ����Ƭ����־λ��Ϊ��
								 
			pCurFace->IntersectLine1 = m_Slice_edge[sz - 1]->e_adja;
			//�жϵ�ǰ��Ƭ�е���һ�ཻ�ߣ������
			judgeOtherLine(layer, pCurFace);
			

			calIntersectPoint(layer, pCurFace->IntersectLine2, pCurFace, tmpendPoint);

			//tmpSegment->pstart =new LPoint(*m_boundary->m_segments[m_boundary->m_segments.size() - 1]->pend);
			//tmpSegment->pend = new LPoint(*tmpLinkPoint);
			//tmpSegment->triangle = pCurFace;
			*tmpSegment = Segment(m_boundary->m_segments[m_boundary->m_segments.size() - 1]->pend, *tmpendPoint, pCurFace);

			m_boundary->m_segments.push_back(new Segment(*tmpSegment));

			unsigned int szlinkline = m_boundary->m_segments.size();

			//�ж������Ƿ�պ�
			if ((m_boundary->m_segments[0]->pstart) ^= (m_boundary->m_segments[szlinkline - 1]->pend))
			{
				m_layers[m_layers.size() - 1]->m_Boundaries.push_back(m_boundary);
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

			//����ƽ������
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

			//�����߿�����
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

void CSlice::getInterSectEdge(SliceLayer* layer, LTriangle* pCurFace)   //ֻҪ�ҵ�һ���ཻ�ı߾Ϳ�������
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

void CSlice::judgeFaceType(SliceLayer* layer, LTriangle* pCurFace)
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

	// ȡ�����������������е������߶�
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

	//��ɾ�����ߵ�֮����߶δ�� m_segments ��
	for (unsigned int i = 0; i < tmp_segments.size(); i++)
	{
		layer->m_Boundaries[0]->m_segments.push_back(tmp_segments[i]);
	}
	int test = 0;
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


