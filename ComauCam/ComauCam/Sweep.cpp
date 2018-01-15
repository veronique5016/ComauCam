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
		//将最外层轮廓存入路径中
		unsigned int szB = m_sweep_layers[i]->m_Boundaries[0]->m_segments.size();
		m_sweep_layers[i]->m_Route.push_back(new CPoint3D(m_sweep_layers[i]->m_Boundaries[0]->m_segments[0]->pstart));
		for (unsigned int j = 0; j < szB; j++)
		{
			m_sweep_layers[i]->m_Route.push_back(new CPoint3D(m_sweep_layers[i]->m_Boundaries[0]->m_segments[j]->pend));
		}


		//变法向的层切面因为太小所以不切，只走轮廓
		if (m_sweep_layers[i]->layer_coordinate[2].dz == 1.0)
		{
			offSet(m_sweep_layers[i], 1.0);
			//寻找轮廓 x 方向的极值
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

			//给第一条扫描线赋初值
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
					break;
				x = x + m_sweep_layers[i]->layer_coordinate[0].dx*distance;
				z = z + m_sweep_layers[i]->layer_coordinate[0].dz*distance;
			}
		}
		else
		{
			unsigned szT = m_sweep_layers[i]->m_Route.size();
			for (unsigned int j = 1; j < szT + 1; j++)
			{
				m_sweep_layers[i]->m_turnRoute.push_back(new FPoint(*m_sweep_layers[i]->m_Route[j%szT]));
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

	///连成 zigzag 路径的算法需要很大的改动
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
	CVector3D tmp_norm;
	CPoint3D tmp_point;
	Segment line1, line2;

	//将轮廓中所有的线段往内偏移一个距离
	for (unsigned int i = 0; i < sz; i++)
	{
		tmp_norm = layer->layer_coordinate[2] * layer->offsetBoundaries[0]->m_segments[i]->segment_vec;
		tmp_norm.Normalize();
		layer->offsetBoundaries[0]->m_segments[i]->pstart = layer->offsetBoundaries[0]->m_segments[i]->pstart + CVector3D(tmp_norm*offset);
		layer->offsetBoundaries[0]->m_segments[i]->pend = layer->offsetBoundaries[0]->m_segments[i]->pend + CVector3D(tmp_norm*offset);
	}

	//对偏移后的线段求交点，形成偏移后的轮廓
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
		m_slice_layers.push_back(slicemodel->m_layers[i]);		//为CSweep对象拷贝一份层切面数据
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

void CSweep::writeGCode(CString sFilePath)
{
	CStdioFile fileout;
	if (fileout.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		fileout.WriteString(_T("G28 ;\n"));
		fileout.WriteString(_T("G1 Z15 F1500 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("M82 ;\n"));
		fileout.WriteString(_T("M190 S50 ;\n"));
		fileout.WriteString(_T("M104 T0 S210 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M109 T0 S210 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		CString str;
		double length = 0.0000;
		double volumn = 0.0000;
		unsigned int szLayer = m_sweep_layers.size();
		for (unsigned int i = 0; i < szLayer; i++)
		{
			str.Format(_T("G0 X%.3f Y%.3f z%.3f\n"),
				m_sweep_layers[i]->m_Route[0]->x, 
				m_sweep_layers[i]->m_Route[0]->y, 
				m_sweep_layers[i]->m_Route[0]->z);
			fileout.WriteString(str);
			unsigned int sz = m_sweep_layers[i]->m_Route.size();
			
			for (unsigned int j = 1; j < sz; j++)
			{
				length = GetDistance(*m_sweep_layers[i]->m_Route[j - 1], *m_sweep_layers[i]->m_Route[j]);
				volumn += length * 0.2;
				str.Format(_T("G1 X%.3f Y%.3f Z%.3f i%.3f j%.3f k%.3f E%.5f\n"),
					m_sweep_layers[i]->m_Route[j]->x, m_sweep_layers[i]->m_Route[j]->y, m_sweep_layers[i]->m_Route[j]->z, 
					m_sweep_layers[i]->layer_coordinate[2].dx,
					m_sweep_layers[i]->layer_coordinate[2].dy,
					m_sweep_layers[i]->layer_coordinate[2].dz,
					(volumn / 6.0));
				fileout.WriteString(str);
			}
			if (i != szLayer - 1)
			{
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f i%.3f j%.3f k%.3f\n"),
					m_sweep_layers[i + 1]->m_Route[0]->x, m_sweep_layers[i]->m_Route[0]->y, m_sweep_layers[i]->m_Route[0]->z,
					m_sweep_layers[i]->layer_coordinate[2].dx,
					m_sweep_layers[i]->layer_coordinate[2].dy,
					m_sweep_layers[i]->layer_coordinate[2].dz);
					fileout.WriteString(str);
			}
		}
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G91 ;\n"));
		fileout.WriteString(_T("T0 ;\n"));
		fileout.WriteString(_T("G1 E-1 ;\n"));
		fileout.WriteString(_T("M104 T0 S0 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M140 S0 ;\n"));
		fileout.WriteString(_T("M84 ;\n"));
		fileout.Close();
	}
}

void CSweep::writeFiveAxisGCode(CString sFilePath)
{
	CStdioFile fileout;
	if (fileout.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		fileout.WriteString(_T("G28 ;\n"));
		fileout.WriteString(_T("G280 ;\n"));
		fileout.WriteString(_T("G1 Z300 F2000 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("M82 ;\n"));
		fileout.WriteString(_T("M104 T0 S210 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M109 T0 S210 ;\n"));
		fileout.WriteString(_T("G1 X12.85 Y117.03 Z167.2 ;\n")); 
		fileout.WriteString(_T("G92 X0 Y0 Z0 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		CString str;
		double length = 0.0000;
		double volumn = 0.0000;
		unsigned int szLayer = m_sweep_layers.size();
		for (unsigned int i = 0; i < szLayer; i++)
		{
			if (m_sweep_layers[i]->layer_coordinate[2].dz == 1.0)
			{
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
					m_sweep_layers[i]->m_Route[0]->x,
					m_sweep_layers[i]->m_Route[0]->y,
					m_sweep_layers[i]->m_Route[0]->z);
				fileout.WriteString(str);
				unsigned int sz = m_sweep_layers[i]->m_Route.size();

				for (unsigned int j = 1; j < sz; j++)
				{
					length = GetDistance(*m_sweep_layers[i]->m_Route[j - 1], *m_sweep_layers[i]->m_Route[j]);
					volumn += length * 0.2;
					str.Format(_T("G1 X%.3f Y%.3f Z%.3f E%.5f\n"),
						m_sweep_layers[i]->m_Route[j]->x, m_sweep_layers[i]->m_Route[j]->y, m_sweep_layers[i]->m_Route[j]->z,
						(volumn / 6.0));
					fileout.WriteString(str);
				}
/*				if (i != szLayer - 1)
				{
					str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
						m_sweep_layers[i + 1]->m_Route[0]->x, m_sweep_layers[i]->m_Route[0]->y, m_sweep_layers[i]->m_Route[0]->z);
					fileout.WriteString(str);
				}*/
			}
			else
			{
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
					m_sweep_layers[i]->m_Route[1]->x,
					m_sweep_layers[i]->m_Route[1]->y,
					m_sweep_layers[i]->m_Route[1]->z+50.0);
				fileout.WriteString(str);

				str.Format(_T("G0 X%.3f Y%.3f Z%.3f A%.3f C%.3f\n"),
					m_sweep_layers[i]->m_turnRoute[0]->x,
					m_sweep_layers[i]->m_turnRoute[0]->y,
					m_sweep_layers[i]->m_turnRoute[0]->z,
					m_sweep_layers[i]->m_turnRoute[0]->A,
					m_sweep_layers[i]->m_turnRoute[0]->C);
				fileout.WriteString(str);

				for (unsigned int j = 1; j < m_sweep_layers[i]->m_turnRoute.size(); j++)
				{
					length = GetDistance(*m_sweep_layers[i]->m_Route[j - 1], *m_sweep_layers[i]->m_Route[j]);
					volumn += length * 0.2*1.25;
					str.Format(_T("G1 X%.3f Y%.3f Z%.3f A%.3f C%.3f E%.5f\n"),
								m_sweep_layers[i]->m_turnRoute[j]->x,
								m_sweep_layers[i]->m_turnRoute[j]->y,
								m_sweep_layers[i]->m_turnRoute[j]->z,
								m_sweep_layers[i]->m_turnRoute[j]->A,
								m_sweep_layers[i]->m_turnRoute[j]->C,
								(volumn / 6.0));
					fileout.WriteString(str);
				}
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
					m_sweep_layers[i]->m_Route[1]->x,
					m_sweep_layers[i]->m_Route[1]->y,
					m_sweep_layers[i]->m_Route[1]->z);
				fileout.WriteString(str);
			}
		}
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G91 ;\n"));
		fileout.WriteString(_T("T0 ;\n"));
		fileout.WriteString(_T("G1 E-1 ;\n"));
		fileout.WriteString(_T("M104 T0 S0 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M140 S0 ;\n"));
		fileout.WriteString(_T("M84 ;\n"));
		fileout.Close();
	}
}

void CSweep::cal_AC_angle(SweepLayer* layer)
{
	int szpt = layer->m_turnRoute.size();
	CVector3D v_x(1.0, 0.0, 0.0);  //x轴向量
	CVector3D v_y(0.0, 1.0, 0.0);  //y轴向量，求法向量在X0Y用于判断C角大小；
	CVector3D v_z(0.0, 0.0, 1.0);  //z轴向量，求法向量与z向量的夹角为A角；
	for (int i = 0; i < szpt; i++)
	{
		//FPoint* ptFive = new FPoint();
		CVector3D v;
		v.ProjectionX0Y(layer->layer_coordinate[2]);   //投影到X0Z平面的向量
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
	CVector3D v_x(1.0, 0.0, 0.0);  //x轴向量
	CVector3D v_y(0.0, 1.0, 0.0);  //y轴向量
	CVector3D v_z(0.0, 0.0, 1.0);  //z轴向量

	//CVector3D v_move(0, 0, m_z_move);   //57.7,二维转台中心和二维转台平台中心的距离补偿，因为打印件在二维转台平台中心，旋转中心是二维转台中心
	CVector3D v_move(0, 0, 57.7);
	//CVector3D v_offset(m_x_offset, m_y_offset, m_z_offset);  //(1.39,-1.14,105.0)二维转台中心在delta世界坐标系中的坐标（世界坐标系的原点是喷绘头共点处），进行坐标补偿，生成在delta平台下xyz的坐标值
	CVector3D v_offset(1.39, -1.14, 105.0);
	//CVector3D v_offset1(m_x_offset, m_y_offset, m_z_offset + m_safeDistance); //保护调试距离
	CVector3D v_offset1(1.39, -1.14, 155.0);
	for (int i = 0; i < szpt; i++)
	{
		CMatrix3D matrix_rotate_z = CMatrix3D::CreateRotateMatrix(2 * PI - layer->m_turnRoute[i]->C, v_z);   //创建旋转平移矩阵
		CMatrix3D matrix_rotate_x = CMatrix3D::CreateRotateMatrix(layer->m_turnRoute[i]->A, v_x);
		CMatrix3D matrix_move = CMatrix3D::CreateTransferMatrix(v_move);
		CMatrix3D matrix_offset = CMatrix3D::CreateTransferMatrix(v_offset);
		CMatrix3D matrix_offset1 = CMatrix3D::CreateTransferMatrix(v_offset1);

		//*(fiveAxisPoints[i]->n) *= matrix_rotate_z;   //法向量变换
		//*(fiveAxisPoints[i]->n) *= matrix_rotate_x;
		*layer->m_turnRoute[i] *= matrix_move;
		*layer->m_turnRoute[i] *= matrix_rotate_z;
		*layer->m_turnRoute[i] *= matrix_rotate_x;
		*layer->m_turnRoute[i] *= matrix_offset1;   //安全距离进行测试
		//*fiveAxisPoints[i] *= matrix_offset;  //正常距离打印
	}
}


