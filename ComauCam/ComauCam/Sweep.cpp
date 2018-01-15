#include "stdafx.h"
#include "Sweep.h"



CSweep::CSweep():m_dDistance(10)
{

}
CSweep::~CSweep()
{
	unsigned int szL = m_vecpSliceLayers.size();
	for (unsigned int i = 0; i < szL; i++)
	{
		delete m_vecpSliceLayers[i];
		m_vecpSliceLayers[i] = NULL;
	}
	m_vecpSliceLayers.clear();

	unsigned int szW = m_vecpSweepLayers.size();
	for (unsigned int i = 0; i < szW; i++)
	{
		delete m_vecpSweepLayers[i];
		m_vecpSweepLayers[i] = NULL;
	}
	m_vecpSweepLayers.clear();

/*	unsigned int szT = temp_layers.size();
	for (unsigned int i = 0; i < szT; i++)
	{
		delete temp_layers[i];
		temp_layers[i] = NULL;
	}
	temp_layers.clear();*/
}


void CSweep::Sweep()
{
	double x_min, x_max, x, z;
	CPoint3D tmp_point;
	for (unsigned int i = 0; i < m_vecpSweepLayers.size(); i++)
	{
		if (i == 0)
		{
			CSweepLayer* layer =new CSweepLayer(*m_vecpSweepLayers[i]);
			Offset(layer, -5);
			unsigned int szB = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments.size();
			m_vecpSweepLayers[i]->m_vecpRoute.push_back(new CPoint3D(layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[0]->m_ptStart));
			for (unsigned int j = 0; j < szB; j++)
			{
				m_vecpSweepLayers[i]->m_vecpRoute.push_back(new CPoint3D(layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[j]->m_ptEnd));
			}
		}
		//将最外层轮廓存入路径中
		unsigned int szB = m_vecpSweepLayers[i]->m_vecpBoundaries[0]->m_vecpSegments.size();
		m_vecpSweepLayers[i]->m_vecpRoute.push_back(new CPoint3D(m_vecpSweepLayers[i]->m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptStart));
		for (unsigned int j = 0; j < szB; j++)
		{
			m_vecpSweepLayers[i]->m_vecpRoute.push_back(new CPoint3D(m_vecpSweepLayers[i]->m_vecpBoundaries[0]->m_vecpSegments[j]->m_ptEnd));
		}

		//变法向的层切面因为太小所以不切，只走轮廓
		if (m_vecpSweepLayers[i]->m_vLayerCoordinate[2].dz == 1.0)
		{
			//设置偏置大小
			Offset(m_vecpSweepLayers[i], 0.9);
			//寻找轮廓 x 方向的极值
			unsigned int sz = m_vecpSweepLayers[i]->m_vecpOffsetBoundaries[0]->m_vecpSegments.size();
			x_min = x_max = m_vecpSweepLayers[i]->m_vecpOffsetBoundaries[0]->m_vecpSegments[0]->m_ptStart.x;
			for (unsigned int j = 0; j < sz; j++)
			{
				if (x_max <= m_vecpSweepLayers[i]->m_vecpOffsetBoundaries[0]->m_vecpSegments[j]->m_ptEnd.x)
					x_max = m_vecpSweepLayers[i]->m_vecpOffsetBoundaries[0]->m_vecpSegments[j]->m_ptEnd.x;
				if (x_min >= m_vecpSweepLayers[i]->m_vecpOffsetBoundaries[0]->m_vecpSegments[j]->m_ptEnd.x)
				{
					x_min = m_vecpSweepLayers[i]->m_vecpOffsetBoundaries[0]->m_vecpSegments[j]->m_ptEnd.x;
					z = m_vecpSweepLayers[i]->m_vecpOffsetBoundaries[0]->m_vecpSegments[j]->m_ptEnd.z;
				}
			}

			//给第一条扫描线赋初值
			x = x_min + 0.1;
			//z = m_vecpSweepLayers[i]->layerPoint.z;
			while (true)
			{
				CSweepLine* tmp_sweep = new CSweepLine();
				tmp_sweep->m_ptLinePoint = CPoint3D(x, 0, z);
				tmp_sweep->m_vLineVec = CVector3D(0, 1, 0);
				tmp_sweep->m_vLineNormal = m_vecpSweepLayers[i]->m_vLayerCoordinate[2];
				m_vecpSweepLayers[i]->m_vecpSweepLines.push_back(tmp_sweep);
				YaxisSweep(m_vecpSweepLayers[i]);

				if (x >= (x_max - m_dDistance))
				{  
					/*SweepLine* tmp_sweep = new SweepLine();
					tmp_sweep->m_ptLinePoint = CPoint3D(x_max, 0, z);
					tmp_sweep->m_vLineVec = CVector3D(0, 1, 0);
					tmp_sweep->line_normal = m_vecpSweepLayers[i]->m_vLayerCoordinate[2];
					m_vecpSweepLayers[i]->m_vecpSweepLines.push_back(tmp_sweep);
					yaxisSweep(m_vecpSweepLayers[i]);*/
					break;
				}
				x = x + m_vecpSweepLayers[i]->m_vLayerCoordinate[0].dx*m_dDistance;
				z = z + m_vecpSweepLayers[i]->m_vLayerCoordinate[0].dz*m_dDistance;
			}
		}
		else
		{
			unsigned szT = m_vecpSweepLayers[i]->m_vecpRoute.size();
			for (unsigned int j = 0; j < szT; j++)
			{
				m_vecpSweepLayers[i]->m_vecpTurnRoute.push_back(new CFPoint(*m_vecpSweepLayers[i]->m_vecpRoute[(j+1)%szT]));
			}
			CalACAngle(m_vecpSweepLayers[i]);
			CalFiveAxisValue(m_vecpSweepLayers[i]);
		}
	}
}

void CSweep::YaxisSweep(CSweepLayer* layer)
{
	unsigned int szL = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments.size();
	CPoint3D line1_p1, line1_p2, line2_p1, line2_p2;
	CVector3D vec1, vec2;
	vector<CPoint3D> crosspoint;
	CPoint3D pout;
	line1_p1 = layer->m_vecpSweepLines[layer->m_vecpSweepLines.size() - 1]->m_ptLinePoint;
	line1_p2 = line1_p1 + layer->m_vecpSweepLines[layer->m_vecpSweepLines.size() - 1]->m_vLineVec;
	for (unsigned int i = 0; i < szL; i++)
	{
		//vec1 = layer->m_vecpSweepLines[layer->m_vecpSweepLines.size() - 1]->m_vLineVec;
		vec1 = CVector3D(line1_p1, layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptStart);
		vec2 = CVector3D(line1_p1, layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptEnd);
		if ((layer->m_vecpSweepLines[layer->m_vecpSweepLines.size() - 1]->m_vLineVec*vec1).dz 
			*(layer->m_vecpSweepLines[layer->m_vecpSweepLines.size() - 1]->m_vLineVec*vec2).dz <0)
		{
			line2_p1 = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptStart;
			line2_p2 = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptEnd;
			GetCrossPoint(pout, line1_p1, line1_p2, line2_p1, line2_p2);
			crosspoint.push_back(CPoint3D(pout));
		}
	}

	///连成 zigzag 路径的算法需要很大的改动
	unsigned int szP = crosspoint.size();

	if (layer->m_vecpRoute.size() == 0)
	{
		layer->m_vecpRoute.push_back(new CPoint3D(crosspoint[0]));
		layer->m_vecpRoute.push_back(new CPoint3D(crosspoint[1]));
	}
	else
	{
		double dist1 = GetDistance(*layer->m_vecpRoute[layer->m_vecpRoute.size() - 1], crosspoint[0]);
		double dist2 = GetDistance(*layer->m_vecpRoute[layer->m_vecpRoute.size() - 1], crosspoint[1]);
		if (dist1 < dist2)
		{
			layer->m_vecpRoute.push_back(new CPoint3D(crosspoint[0]));
			layer->m_vecpRoute.push_back(new CPoint3D(crosspoint[1]));
		}
		else
		{
			layer->m_vecpRoute.push_back(new CPoint3D(crosspoint[1]));
			layer->m_vecpRoute.push_back(new CPoint3D(crosspoint[0]));
		}
	}	
	/////////
}

void CSweep::Offset(CSweepLayer* layer, double offset)
{
	CPoint3D point_out;
	unsigned int sz = layer->m_vecpBoundaries[0]->m_vecpSegments.size();
	layer->m_vecpOffsetBoundaries.push_back(new CBoundary());
	for (unsigned int i = 0; i < sz; i++)
	{
		layer->m_vecpOffsetBoundaries[0]->m_vecpSegments.push_back(new CSegment(*layer->m_vecpBoundaries[0]->m_vecpSegments[i]));
	}
	CVector3D offset_vec;
	CPoint3D tmp_point;
	CSegment line1, line2;

	//将轮廓中所有的线段往内偏移一个距离
	for (unsigned int i = 0; i < sz; i++)
	{
		offset_vec = layer->m_vLayerCoordinate[2] * layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec;
		offset_vec.Normalize();
		layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptStart = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptStart + CVector3D(offset_vec*offset);
		layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptEnd = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptEnd + CVector3D(offset_vec*offset);
	}

	//对偏移后的线段求交点，形成偏移后的轮廓
	for (unsigned int i = 0; i < sz; i++)
	{
		line1.m_ptStart = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptStart;
		line1.m_ptEnd = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptEnd;

		line2.m_ptStart = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[(i + 1) % sz]->m_ptStart;
		line2.m_ptEnd = layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[(i + 1) % sz]->m_ptEnd;

		GetCrossPoint(point_out, line1.m_ptStart, line1.m_ptEnd, line2.m_ptStart, line2.m_ptEnd);
		layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[i]->m_ptEnd = CLPoint(point_out);
		layer->m_vecpOffsetBoundaries[0]->m_vecpSegments[(i + 1) % sz]->m_ptStart = CLPoint(point_out);
	}
}

void CSweep::LoadSliceModel(CSlice* slicemodel)
{	
	unsigned int szLay = slicemodel->m_vecpLayers.size();
	for (unsigned int i = 0; i < szLay; i++)
	{
		m_vecpSliceLayers.push_back(slicemodel->m_vecpLayers[i]);		//为CSweep对象拷贝一份层切面数据
		m_vecpSweepLayers.push_back(new CSweepLayer(*slicemodel->m_vecpLayers[i]));
	}
}

void CSweep::DrawRoute(int begin, int end)
{
	unsigned int szL = m_vecpSweepLayers.size();
	for (unsigned int i = (begin-1); i < end; i++)
	{
		//画轮廓
		unsigned int szB = m_vecpSweepLayers[i]->m_vecpBoundaries[0]->m_vecpSegments.size();
		for (unsigned int j = 0; j < szB; j++)
		{
			CPoint3D p1 = CPoint3D(*m_vecpSweepLayers[i]->m_vecpRoute[j]);
			CPoint3D p2 = CPoint3D(*m_vecpSweepLayers[i]->m_vecpRoute[j+1]);
			/*glLineWidth(1.5f);
			glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glEnd();*/
			glColor3f(1.0f, 0.0f, 0.0f);
			//DrawCylinder(p1, p2);
			DrawCuboid(p1, p2, m_vecpSweepLayers[i]->m_vLayerCoordinate[2]);
		}
		//画路径
		unsigned int sz = m_vecpSweepLayers[i]->m_vecpRoute.size();
		for (unsigned int j = szB+1; j < sz-1; j++)
		{
			CPoint3D p1 = CPoint3D(*m_vecpSweepLayers[i]->m_vecpRoute[j]);
			CPoint3D p2 = CPoint3D(*m_vecpSweepLayers[i]->m_vecpRoute[j + 1]);
			/*glLineWidth(1.5f);
			glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glEnd();*/
			//DrawCylinder(p1, p2);
			DrawCuboid(p1, p2, m_vecpSweepLayers[i]->m_vLayerCoordinate[2]);
		}
	}
}

void CSweep::DrawCylinder(CPoint3D p1, CPoint3D p2)
{
	GLdouble  dir_x = p2.x - p1.x;
	GLdouble  dir_y = p2.y - p1.y;
	GLdouble  dir_z = p2.z - p1.z;
	GLdouble  bone_length = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
	static GLUquadricObj *  quad_obj = NULL;
	if (quad_obj == NULL)
		quad_obj = gluNewQuadric();
	gluQuadricDrawStyle(quad_obj, GLU_FILL);
	gluQuadricNormals(quad_obj, GLU_SMOOTH);
	glPushMatrix();
	// 平移到起始点  
	glTranslated(p1.x, p1.y, p1.z);
	// 计算长度  
	double  length;
	length = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
	if (length < 0.0001) {
		dir_x = 0.0; dir_y = 0.0; dir_z = 1.0;  length = 1.0;
	}
	dir_x /= length;  dir_y /= length;  dir_z /= length;
	GLdouble  up_x, up_y, up_z;
	up_x = 0.0;
	up_y = 1.0;
	up_z = 0.0;
	double  side_x, side_y, side_z;
	side_x = up_y * dir_z - up_z * dir_y;
	side_y = up_z * dir_x - up_x * dir_z;
	side_z = up_x * dir_y - up_y * dir_x;
	length = sqrt(side_x*side_x + side_y*side_y + side_z*side_z);
	if (length < 0.0001) {
		side_x = 1.0; side_y = 0.0; side_z = 0.0;  length = 1.0;
	}
	side_x /= length;  side_y /= length;  side_z /= length;
	up_x = dir_y * side_z - dir_z * side_y;
	up_y = dir_z * side_x - dir_x * side_z;
	up_z = dir_x * side_y - dir_y * side_x;
	// 计算变换矩阵  
	GLdouble  m[16] = { side_x, side_y, side_z, 0.0,
		up_x,   up_y,   up_z,   0.0,
		dir_x,  dir_y,  dir_z,  0.0,
		0.0,    0.0,    0.0,    1.0 };
	glMultMatrixd(m);
	// 圆柱体参数  
	GLdouble radius = 2;        // 半径  
	GLdouble slices = 30.0;      //  段数  
	GLdouble stack = 3.0;       // 递归次数  
	gluCylinder(quad_obj, radius, radius, bone_length, slices, stack);
	glPopMatrix();
}

void CSweep::DrawCuboid(CPoint3D pStart, CPoint3D pEnd, CVector3D normal)
{
	CVector3D xAxis = CVector3D(pStart, pEnd);
	xAxis.Normalize();
	CVector3D zAxis = normal;
	zAxis.Normalize();
	CVector3D yAxis = zAxis * xAxis;
	yAxis.Normalize();
	CPoint3D square1[4];
	CPoint3D tmp = pStart;
	CVector3D offset = yAxis + zAxis;
	offset.Normalize();
	tmp = pStart + offset*2;
	square1[0] = tmp;

	offset = zAxis - yAxis;
	offset.Normalize();
	tmp = pStart + offset*2;
	square1[1] = tmp;

	offset =CVector3D(0,0,0) - zAxis - yAxis;
	offset.Normalize();
	tmp = pStart + offset*2;
	square1[2] = tmp;

	offset = yAxis - zAxis;
	offset.Normalize();
	tmp = pStart + offset*2;
	square1[3] = tmp;

	CPoint3D square2[4];
	tmp = pEnd;
	offset = yAxis + zAxis;
	offset.Normalize();
	tmp = pEnd + offset*2;
	square2[0] = tmp;

	offset = zAxis - yAxis;
	offset.Normalize();
	tmp = pEnd + offset*2;
	square2[1] = tmp;

	offset = CVector3D(0, 0, 0) - zAxis - yAxis;
	offset.Normalize();
	tmp = pEnd + offset*2;
	square2[2] = tmp;

	offset = yAxis - zAxis;
	offset.Normalize();
	tmp = pEnd + offset*2;
	square2[3] = tmp;

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(square1[0].x, square1[0].y, square1[0].z);
	glVertex3f(square1[1].x, square1[1].y, square1[1].z);
	glVertex3f(square1[2].x, square1[2].y, square1[2].z);
	glVertex3f(square1[3].x, square1[3].y, square1[3].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(square2[0].x, square2[0].y, square2[0].z);
	glVertex3f(square2[1].x, square2[1].y, square2[1].z);
	glVertex3f(square2[2].x, square2[2].y, square2[2].z);
	glVertex3f(square2[3].x, square2[3].y, square2[3].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(square1[0].x, square1[0].y, square1[0].z);
	glVertex3f(square1[1].x, square1[1].y, square1[1].z);
	glVertex3f(square2[1].x, square2[1].y, square2[1].z);
	glVertex3f(square2[0].x, square2[0].y, square2[0].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(square1[1].x, square1[1].y, square1[1].z);
	glVertex3f(square1[2].x, square1[2].y, square1[2].z);
	glVertex3f(square2[2].x, square2[2].y, square2[2].z);
	glVertex3f(square2[1].x, square2[1].y, square2[1].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(square1[2].x, square1[2].y, square1[2].z);
	glVertex3f(square1[3].x, square1[3].y, square1[3].z);
	glVertex3f(square2[3].x, square2[3].y, square2[3].z);
	glVertex3f(square2[2].x, square2[2].y, square2[2].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(square1[3].x, square1[3].y, square1[3].z);
	glVertex3f(square1[0].x, square1[0].y, square1[0].z);
	glVertex3f(square2[0].x, square2[0].y, square2[0].z);
	glVertex3f(square2[3].x, square2[3].y, square2[3].z);
	glEnd();
}

void CSweep::CalACAngle(CSweepLayer* layer)
{
	int szpt = layer->m_vecpTurnRoute.size();
	CVector3D v_x(1.0, 0.0, 0.0);  //x轴向量
	CVector3D v_y(0.0, 1.0, 0.0);  //y轴向量，求法向量在X0Y用于判断C角大小；
	CVector3D v_z(0.0, 0.0, 1.0);  //z轴向量，求法向量与z向量的夹角为A角；
	for (int i = 0; i < szpt; i++)
	{
		//FPoint* ptFive = new FPoint();
		CVector3D v;
		v.ProjectionX0Y(layer->m_vLayerCoordinate[2]);   //投影到X0Z平面的向量
													//v = lawPrintPoints[i]->Ltri->n;
		double C = v.GetC_Angle(v, v_y);
		double A = v.GetAngle(layer->m_vLayerCoordinate[2], v_z);
		layer->m_vecpTurnRoute[i]->A = A;
		layer->m_vecpTurnRoute[i]->C = C;
	}
}

void CSweep::CalFiveAxisValue(CSweepLayer* layer)
{
	int szpt = layer->m_vecpRoute.size();
	CVector3D v_x(1.0, 0.0, 0.0);  //x轴向量
	CVector3D v_y(0.0, 1.0, 0.0);  //y轴向量
	CVector3D v_z(0.0, 0.0, 1.0);  //z轴向量

	//CVector3D v_move(0, 0, m_z_move);   //57.7,二维转台中心和二维转台平台中心的距离补偿，因为打印件在二维转台平台中心，旋转中心是二维转台中心
	CVector3D v_move(0, 0, 100);
	CVector3D v_countermove(0, 0, -100);
	//CVector3D v_offset(m_x_offset, m_y_offset, m_z_offset);  //(1.39,-1.14,105.0)二维转台中心在delta世界坐标系中的坐标（世界坐标系的原点是喷绘头共点处），进行坐标补偿，生成在delta平台下xyz的坐标值
	//CVector3D v_offset(-2.12, 109.40, 138.4);
	//CVector3D v_offset1(m_x_offset, m_y_offset, m_z_offset + m_safeDistance); //保护调试距离
	//CVector3D v_offset1(-2.12, 109.40, 180);
	for (int i = 0; i < szpt; i++)
	{
		CMatrix3D matrix_rotate_z = CMatrix3D::CreateRotateMatrix(2 * PI - layer->m_vecpTurnRoute[i]->C, v_z);   //创建旋转平移矩阵
		CMatrix3D matrix_rotate_x = CMatrix3D::CreateRotateMatrix(layer->m_vecpTurnRoute[i]->A, v_x);
		CMatrix3D matrix_move = CMatrix3D::CreateTransferMatrix(v_move);
		CMatrix3D matrix_countermove = CMatrix3D::CreateTransferMatrix(v_countermove);
		//CMatrix3D matrix_offset = CMatrix3D::CreateTransferMatrix(v_offset);
		//CMatrix3D matrix_offset1 = CMatrix3D::CreateTransferMatrix(v_offset1);

		//*(fiveAxisPoints[i]->n) *= matrix_rotate_z;   //法向量变换
		//*(fiveAxisPoints[i]->n) *= matrix_rotate_x;
		*layer->m_vecpTurnRoute[i] *= matrix_move;
		*layer->m_vecpTurnRoute[i] *= matrix_rotate_z;
		*layer->m_vecpTurnRoute[i] *= matrix_rotate_x;
		*layer->m_vecpTurnRoute[i] *= matrix_countermove;
		//*layer->m_turnRoute[i] *= matrix_offset1;   //安全距离进行测试
		//*fiveAxisPoints[i] *= matrix_offset;  //正常距离打印
	}
}


