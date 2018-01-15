#include "stdafx.h"
#include "Sweep.h"



CSweep::CSweep():distance(10)
{

}
CSweep::~CSweep()
{
	unsigned int szL = m_slice_layers.size();
	for (unsigned int i = 0; i < szL; i++)
	{
		delete m_slice_layers[i];
		m_slice_layers[i] = NULL;
	}
	m_slice_layers.clear();

	unsigned int szW = m_sweep_layers.size();
	for (unsigned int i = 0; i < szW; i++)
	{
		delete m_sweep_layers[i];
		m_sweep_layers[i] = NULL;
	}
	m_sweep_layers.clear();

/*	unsigned int szT = temp_layers.size();
	for (unsigned int i = 0; i < szT; i++)
	{
		delete temp_layers[i];
		temp_layers[i] = NULL;
	}
	temp_layers.clear();*/
}


void CSweep::sweep()
{
	double x_min, x_max, x, dx, z;
	//dx = distance;
	CPoint3D tmp_point;
	for (unsigned int i = 0; i < m_sweep_layers.size(); i++)
	{
		//���������������·����
		unsigned int szB = m_sweep_layers[i]->m_Boundaries[0]->m_segments.size();
		m_sweep_layers[i]->m_Route.push_back(new CPoint3D(m_sweep_layers[i]->m_Boundaries[0]->m_segments[0]->pstart));
		for (unsigned int j = 0; j < szB; j++)
		{
			m_sweep_layers[i]->m_Route.push_back(new CPoint3D(m_sweep_layers[i]->m_Boundaries[0]->m_segments[j]->pend));
		}

		//�䷨��Ĳ�������Ϊ̫С���Բ��У�ֻ������
		if (m_sweep_layers[i]->layer_coordinate[2].dz == 1.0)
		{
			offSet(m_sweep_layers[i], 1.0);
			//Ѱ������ x ����ļ�ֵ
			unsigned int sz = m_sweep_layers[i]->offsetBoundaries[0]->m_segments.size();
			x_min = x_max = m_sweep_layers[i]->offsetBoundaries[0]->m_segments[0]->pstart.x;
			for (unsigned int j = 0; j < sz; j++)
			{
				if (x_max <= m_sweep_layers[i]->offsetBoundaries[0]->m_segments[j]->pend.x)
					x_max = m_sweep_layers[i]->offsetBoundaries[0]->m_segments[j]->pend.x;
				if (x_min >= m_sweep_layers[i]->offsetBoundaries[0]->m_segments[j]->pend.x)
				{
					x_min = m_sweep_layers[i]->offsetBoundaries[0]->m_segments[j]->pend.x;
					z = m_sweep_layers[i]->offsetBoundaries[0]->m_segments[j]->pend.z;
				}
			}

			//����һ��ɨ���߸���ֵ
			x = x_min + 0.1;
			//z = m_sweep_layers[i]->layerPoint.z;
			while (true)
			{
				SweepLine* tmp_sweep = new SweepLine();
				tmp_sweep->line_point = CPoint3D(x, 0, z);
				tmp_sweep->line_vec = CVector3D(0, 1, 0);
				tmp_sweep->line_normal = m_sweep_layers[i]->layer_coordinate[2];
				m_sweep_layers[i]->m_sweeplines.push_back(tmp_sweep);
				yaxisSweep(m_sweep_layers[i]);

				if (x >= (x_max - distance))
				{  
					/*SweepLine* tmp_sweep = new SweepLine();
					tmp_sweep->line_point = CPoint3D(x_max, 0, z);
					tmp_sweep->line_vec = CVector3D(0, 1, 0);
					tmp_sweep->line_normal = m_sweep_layers[i]->layer_coordinate[2];
					m_sweep_layers[i]->m_sweeplines.push_back(tmp_sweep);
					yaxisSweep(m_sweep_layers[i]);*/
					break;
				}
				x = x + m_sweep_layers[i]->layer_coordinate[0].dx*distance;
				z = z + m_sweep_layers[i]->layer_coordinate[0].dz*distance;
			}
		}
		else
		{
			unsigned szT = m_sweep_layers[i]->m_Route.size();
			for (unsigned int j = 0; j < szT; j++)
			{
				m_sweep_layers[i]->m_turnRoute.push_back(new FPoint(*m_sweep_layers[i]->m_Route[(j+1)%szT]));
			}
			cal_AC_angle(m_sweep_layers[i]);
			calFiveAxisValue(m_sweep_layers[i]);
		}
	}
}

void CSweep::yaxisSweep(SweepLayer * layer)
{
	unsigned int szL = layer->offsetBoundaries[0]->m_segments.size();
	CPoint3D line1_p1, line1_p2, line2_p1, line2_p2;
	CVector3D vec1, vec2;
	vector<CPoint3D> crosspoint;
	CPoint3D pout;
	line1_p1 = layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_point;
	line1_p2 = line1_p1 + layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_vec;
	for (unsigned int i = 0; i < szL; i++)
	{
		//vec1 = layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_vec;
		vec1 = CVector3D(line1_p1, layer->offsetBoundaries[0]->m_segments[i]->pstart);
		vec2 = CVector3D(line1_p1, layer->offsetBoundaries[0]->m_segments[i]->pend);
		if ((layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_vec*vec1).dz 
			*(layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_vec*vec2).dz <0)
		{
			line2_p1 = layer->offsetBoundaries[0]->m_segments[i]->pstart;
			line2_p2 = layer->offsetBoundaries[0]->m_segments[i]->pend;
			GetCrossPoint(pout, line1_p1, line1_p2, line2_p1, line2_p2);
			crosspoint.push_back(CPoint3D(pout));
		}
	}

	///���� zigzag ·�����㷨��Ҫ�ܴ�ĸĶ�
	unsigned int szP = crosspoint.size();

	if (layer->m_Route.size() == 0)
	{
		layer->m_Route.push_back(new CPoint3D(crosspoint[0]));
		layer->m_Route.push_back(new CPoint3D(crosspoint[1]));
	}
	else
	{
		double dist1 = GetDistance(*layer->m_Route[layer->m_Route.size() - 1], crosspoint[0]);
		double dist2 = GetDistance(*layer->m_Route[layer->m_Route.size() - 1], crosspoint[1]);
		if (dist1 < dist2)
		{
			layer->m_Route.push_back(new CPoint3D(crosspoint[0]));
			layer->m_Route.push_back(new CPoint3D(crosspoint[1]));
		}
		else
		{
			layer->m_Route.push_back(new CPoint3D(crosspoint[1]));
			layer->m_Route.push_back(new CPoint3D(crosspoint[0]));
		}
	}	
	/////////
}

void CSweep::offSet(SweepLayer* layer, double offset)
{
	CPoint3D point_out;
	unsigned int sz = layer->m_Boundaries[0]->m_segments.size();
	layer->offsetBoundaries.push_back(new Boundary());
	for (unsigned int i = 0; i < sz; i++)
	{
		layer->offsetBoundaries[0]->m_segments.push_back(new Segment(*layer->m_Boundaries[0]->m_segments[i]));
	}
	CVector3D offset_vec;
	CPoint3D tmp_point;
	Segment line1, line2;

	//�����������е��߶�����ƫ��һ������
	for (unsigned int i = 0; i < sz; i++)
	{
		offset_vec = layer->layer_coordinate[2] * layer->offsetBoundaries[0]->m_segments[i]->segment_vec;
		offset_vec.Normalize();
		layer->offsetBoundaries[0]->m_segments[i]->pstart = layer->offsetBoundaries[0]->m_segments[i]->pstart + CVector3D(offset_vec*offset);
		layer->offsetBoundaries[0]->m_segments[i]->pend = layer->offsetBoundaries[0]->m_segments[i]->pend + CVector3D(offset_vec*offset);
	}

	//��ƫ�ƺ���߶��󽻵㣬�γ�ƫ�ƺ������
	for (unsigned int i = 0; i < sz; i++)
	{
		line1.pstart = layer->offsetBoundaries[0]->m_segments[i]->pstart;
		line1.pend = layer->offsetBoundaries[0]->m_segments[i]->pend;

		line2.pstart = layer->offsetBoundaries[0]->m_segments[(i + 1) % sz]->pstart;
		line2.pend = layer->offsetBoundaries[0]->m_segments[(i + 1) % sz]->pend;

		GetCrossPoint(point_out, line1.pstart, line1.pend, line2.pstart, line2.pend);
		layer->offsetBoundaries[0]->m_segments[i]->pend = LPoint(point_out);
		layer->offsetBoundaries[0]->m_segments[(i + 1) % sz]->pstart = LPoint(point_out);
	}
}

void CSweep::loadSliceModel(CSlice* slicemodel)
{	
	unsigned int szLay = slicemodel->m_layers.size();
	for (unsigned int i = 0; i < szLay; i++)
	{
		m_slice_layers.push_back(slicemodel->m_layers[i]);		//ΪCSweep���󿽱�һ�ݲ���������
		m_sweep_layers.push_back(new SweepLayer(*slicemodel->m_layers[i]));
	}
}

void CSweep::drawRoute(int begin, int end)
{
	unsigned int szL = m_sweep_layers.size();
	for (unsigned int i = (begin-1); i < end; i++)
	{
		unsigned int sz = m_sweep_layers[i]->m_Route.size();
		for (unsigned int j = 0; j < sz - 1; j++)
		{
			CPoint3D p1 = CPoint3D(*m_sweep_layers[i]->m_Route[j]);
			CPoint3D p2 = CPoint3D(*m_sweep_layers[i]->m_Route[j+1]);
			glLineWidth(1.5f);
			glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glEnd();
		}
	}
}

void CSweep::cal_AC_angle(SweepLayer* layer)
{
	int szpt = layer->m_turnRoute.size();
	CVector3D v_x(1.0, 0.0, 0.0);  //x������
	CVector3D v_y(0.0, 1.0, 0.0);  //y����������������X0Y�����ж�C�Ǵ�С��
	CVector3D v_z(0.0, 0.0, 1.0);  //z����������������z�����ļн�ΪA�ǣ�
	for (int i = 0; i < szpt; i++)
	{
		//FPoint* ptFive = new FPoint();
		CVector3D v;
		v.ProjectionX0Y(layer->layer_coordinate[2]);   //ͶӰ��X0Zƽ�������
													//v = lawPrintPoints[i]->Ltri->n;
		double C = v.GetC_Angle(v, v_y);
		double A = v.GetAngle(layer->layer_coordinate[2], v_z);
		layer->m_turnRoute[i]->A = A;
		layer->m_turnRoute[i]->C = C;
	}
}

void CSweep::calFiveAxisValue(SweepLayer* layer)
{
	int szpt = layer->m_Route.size();
	CVector3D v_x(1.0, 0.0, 0.0);  //x������
	CVector3D v_y(0.0, 1.0, 0.0);  //y������
	CVector3D v_z(0.0, 0.0, 1.0);  //z������

	//CVector3D v_move(0, 0, m_z_move);   //57.7,��άת̨���ĺͶ�άת̨ƽ̨���ĵľ��벹������Ϊ��ӡ���ڶ�άת̨ƽ̨���ģ���ת�����Ƕ�άת̨����
	CVector3D v_move(0, 0, 100);
	CVector3D v_countermove(0, 0, -100);
	//CVector3D v_offset(m_x_offset, m_y_offset, m_z_offset);  //(1.39,-1.14,105.0)��άת̨������delta��������ϵ�е����꣨��������ϵ��ԭ�������ͷ���㴦�����������겹����������deltaƽ̨��xyz������ֵ
	//CVector3D v_offset(-2.12, 109.40, 138.4);
	//CVector3D v_offset1(m_x_offset, m_y_offset, m_z_offset + m_safeDistance); //�������Ծ���
	//CVector3D v_offset1(-2.12, 109.40, 180);
	for (int i = 0; i < szpt; i++)
	{
		CMatrix3D matrix_rotate_z = CMatrix3D::CreateRotateMatrix(2 * PI - layer->m_turnRoute[i]->C, v_z);   //������תƽ�ƾ���
		CMatrix3D matrix_rotate_x = CMatrix3D::CreateRotateMatrix(layer->m_turnRoute[i]->A, v_x);
		CMatrix3D matrix_move = CMatrix3D::CreateTransferMatrix(v_move);
		CMatrix3D matrix_countermove = CMatrix3D::CreateTransferMatrix(v_countermove);
		//CMatrix3D matrix_offset = CMatrix3D::CreateTransferMatrix(v_offset);
		//CMatrix3D matrix_offset1 = CMatrix3D::CreateTransferMatrix(v_offset1);

		//*(fiveAxisPoints[i]->n) *= matrix_rotate_z;   //�������任
		//*(fiveAxisPoints[i]->n) *= matrix_rotate_x;
		*layer->m_turnRoute[i] *= matrix_move;
		*layer->m_turnRoute[i] *= matrix_rotate_z;
		*layer->m_turnRoute[i] *= matrix_rotate_x;
		*layer->m_turnRoute[i] *= matrix_countermove;
		//*layer->m_turnRoute[i] *= matrix_offset1;   //��ȫ������в���
		//*fiveAxisPoints[i] *= matrix_offset;  //���������ӡ
	}
}


