#include "stdafx.h"
#include "OpenGL.h"


COpenGLDC::COpenGLDC()
{
}
COpenGLDC:: ~COpenGLDC()
{
}

void COpenGLDC::DrawAxis()
{
	glTranslatef(0.0f, -100.0f, -100.0f);//-290.16

    //地面		
	glLineWidth(2.0);
	int R = 300;
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);

	for (int i = 0; i <= 270; i += 10)
	{
		glVertex3f(R * cos(2 * PI*i / 360), R * sin(2 * PI*i / 360), 0.0);
		glVertex3f(-R * cos(2 * PI*i / 360), R * sin(2 * PI*i / 360), 0.0);
		glVertex3f(R * cos(2 * PI*i / 360), R * sin(2 * PI*i / 360), 0.0);
		glVertex3f(R * cos(2 * PI*i / 360), -R * sin(2 * PI*i / 360), 0.0);
	}

	glEnd();


	//画同心圆
	for (int j = 0; j < 1000; j += 600)
	{
		glBegin(GL_LINE_LOOP);
		glColor3f(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 200; i++)
		{
			glVertex3f(R * cos(2 * PI*i / 200), R * sin(2 * PI*i / 200), j);
		}
		glEnd();
		R += 50;
	}

	glBegin(GL_LINES);
	for (int i = 0; i < 360; i += 60)
	{
		glVertex3f((R-50)*cos(2*PI*i/360), (R-50)*sin(2 * PI*i / 360), 600.0);
		glVertex3f((R - 100)*cos(2 * PI*i / 360), (R - 100)*sin(2 * PI*i / 360), 0.0);
	}


	// x轴
	glColor3f(1.0F, 0.0F, 0.0F);
	glVertex3f(-90.0f, 0.0f, 0.0f);
	glVertex3f(90.0f, 0.0f, 0.0f);
	glVertex3f(75.0f, 10.0f, 0.0f);
	glVertex3f(90.0f, 0.0f, 0.0f);
	glVertex3f(75.0f, -10.0f, 0.0f);
	glVertex3f(90.0f, 0.0f, 0.0f);

	// y轴
	glColor3f(0.0F, 1.0F, 0.0F);
	glVertex3f(0.0f, -90.0f, 0.0f);
	glVertex3f(0.0f, 90.0f, 0.0f);
	glVertex3f(-10.0f, 75.0f, 0.0f);
	glVertex3f(0.0f, 90.0f, 0.0f);
	glVertex3f(10.0f, 75.0f, 0.0f);
	glVertex3f(0.0f, 90.0f, 0.0f);

	// z轴
	glColor3f(0.0F, 0.0F, 1.0F);
	glVertex3f(0.0f, 0.0f, -90.0f);
	glVertex3f(0.0f, 0.0f, 90.0f);
	glVertex3f(-10.0f, 0.0f, 75.0f);
	glVertex3f(0.0f, 0.0f, 90.0f);
	glVertex3f(10.0f, 0.0f, 75.0f);
	glVertex3f(0.0f, 0.0f, 90.0f);

	glEnd();
}


void COpenGLDC::DrawTriFace(CVector3D vec, CPoint3D pt1, CPoint3D pt2, CPoint3D pt3)
{
	glBegin(GL_TRIANGLES);
	glNormal3d(vec.dx, vec.dy, vec.dz);
	glVertex3d(pt1.x, pt1.y, pt1.z);
	glVertex3d(pt2.x, pt2.y, pt2.z);
	glVertex3d(pt3.x, pt3.y, pt3.z);
	glEnd();
}

void COpenGLDC::DrawTriFrame(CPoint3D A, CPoint3D B, CPoint3D C)
{
	DrawLine(A, B);
	DrawLine(B, C);
	DrawLine(C, A);
}

void COpenGLDC::DrawLine(const CPoint3D& sp, const CPoint3D& ep, int lineStyle/* =LS_SOLID */, int lineWidth/* =1 */)
{
	glLineWidth(lineWidth);


	glEnable(GL_LINE_STIPPLE);

	switch (lineStyle)
	{
	case LS_SOLID:
		glLineStipple(1, 0xFFFF);
		break;

	case LS_DASH:
		glLineStipple(2, 0x0F0F);
		break;

	case LS_CENTER:
		glLineStipple(1, 0xFF18);
		break;
	}

	glBegin(GL_LINES);
	glVertex3f(sp.x, sp.y, sp.z);
	glVertex3f(ep.x, ep.y, ep.z);
	glEnd();

	glDisable(GL_LINE_STIPPLE);

	//SetColor(oldClr);
}

void COpenGLDC::DrawFrag(CSliceFrag fragment)
{
	DrawCylinder(fragment.m_ptBoundary[0], fragment.m_ptBoundary[1]);
	DrawCylinder(fragment.m_ptBoundary[1], fragment.m_ptBoundary[2]);
	DrawCylinder(fragment.m_ptBoundary[2], fragment.m_ptBoundary[3]);
	DrawCylinder(fragment.m_ptBoundary[3], fragment.m_ptBoundary[0]);
}

void COpenGLDC::DrawSTLModel(CSTLModel * model, bool showTri)
{
	int sz = model->m_vecpTris.size();
	if (showTri)
	{
		for (int i = 0; i<sz; i++)
		{
			//model->m_vecpTris[i]->DrawTriangleFace(pDC);  //面片模式     
			DrawTriFace(model->m_vecpTris[i]->Normal, model->m_vecpTris[i]->A, model->m_vecpTris[i]->B, model->m_vecpTris[i]->C);
		}
	}
	else
	{
		for (int i = 0; i < sz; i++)
		{
			DrawTriFrame(model->m_vecpTris[i]->A, model->m_vecpTris[i]->B, model->m_vecpTris[i]->C);  //线框模式
		}
	}
}

void COpenGLDC::DrawLayer(CSliceLayer * layer, bool showPolygon, double color[])
{
	CPoint3D point;
	int szpolyline = layer->m_vpBoundaries.size();
	for (int j = 0; j < szpolyline; j++)
	{
		int szLine = layer->m_vpBoundaries[j]->m_vpSegments.size();
		glLineWidth(1.5f);

		//绘制平面轮廓
		if (showPolygon)
		{
			glBegin(GL_POLYGON);
			glColor3f(color[0], color[1], color[2]);
			for (int k = 0; k < szLine; k++)
			{
				point = layer->m_vpBoundaries[j]->m_vpSegments[k]->m_ptStart;
				glVertex3f(point.x, point.y, point.z);
				point = layer->m_vpBoundaries[j]->m_vpSegments[k]->m_ptEnd;
				glVertex3f(point.x, point.y, point.z);
			}
			glEnd();
		}

		//绘制线框轮廓
		glColor3f(color[0], color[1], color[2]);
		for (int k = 0; k<szLine; k++)
		{
			if (k % 3 == 0)
				glColor3f(1.0, .0, 0.0);
			else if (k % 3 == 1)
				glColor3f(0.0, 1.0, 0.0);
			else
				glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_LINE_LOOP);
			point = layer->m_vpBoundaries[j]->m_vpSegments[k]->m_ptStart;
			glVertex3f(point.x, point.y, point.z);
			point = layer->m_vpBoundaries[j]->m_vpSegments[k]->m_ptEnd;
			glVertex3f(point.x, point.y, point.z);
			glEnd();
		}
	}
}


void COpenGLDC::DrawSliceModel(CSlice* model, bool showPolygon, int start, int end)
{
	double color[3];
	int szlayer = model->m_vpLayers.size();
	CPoint3D point;
	for (int i = (start - 1); i<end; i++)
	{	
		if (model->m_vpLayers[i]->m_vCoordinate[2].dz == 1.0)
		{
			color[0] = 1.0;
			color[1] = 0.0;
			color[2] = 0.0;
		}
		else
		{
			color[0] = 1.0;
			color[1] = 1.0;
			color[2] = 0.0;
		}
		DrawLayer(model->m_vpLayers[i], showPolygon, color);
		for (int j = 0; j < model->m_vpLayers[i]->m_vpFragments.size(); j++)
		{
			if (j % 3 == 0)
				glColor3f(1.0, .0, 0.0);
			else if (j % 3 == 1)
				glColor3f(0.0, 1.0, 0.0);
			else
				glColor3f(0.0, 0.0, 1.0);
			DrawFrag(*model->m_vpLayers[i]->m_vpFragments[j]);
		}
		/*if (model->m_vpLayers[i]->m_pAddictedLayer != NULL)
		{
			DrawLayer(model->m_vpLayers[i]->m_pAddictedLayer, showPolygon, color);
		}*/
	}
}

void COpenGLDC::DrawSweepModel(CSweep * model, int start, int end)
{
	int szL = model->m_vpSweepLayers.size();
	for (int i = (start - 1); i < end; i++)
	{
		//画轮廓
		int szB = model->m_vpSweepLayers[i]->m_vpBoundaries[0]->m_vpSegments.size();
		//glColor3f(1.0, 0.0, 0.0);
		for (int j = 0; j < szB; j++)
		{
			if (j%3 == 0)
				glColor3f(1.0, .0, 0.0);
			else if(j%3 ==1)
				glColor3f(0.0, 1.0, 0.0);
			else
				glColor3f(0.0, 0.0, 1.0);
			CPoint3D p1 = CPoint3D(*model->m_vpSweepLayers[i]->m_vpRoute[j]);
			CPoint3D p2 = CPoint3D(*model->m_vpSweepLayers[i]->m_vpRoute[j + 1]);
			DrawCuboid(p1, p2, model->m_vpSweepLayers[i]->m_vCoordinate[2]);
		}
		//画路径
		int sz = model->m_vpSweepLayers[i]->m_vpRoute.size();
		for (int j = szB + 1; j < sz - 1; j++)
		{
			if (j % 3 == 0)
				glColor3f(1.0, 0.0, 0.0);
			else if (j % 3 == 1)
				glColor3f(0.0, 1.0, 0.0);
			else
				glColor3f(0.0, 0.0, 1.0);
			CPoint3D p1 = CPoint3D(*model->m_vpSweepLayers[i]->m_vpRoute[j]);
			CPoint3D p2 = CPoint3D(*model->m_vpSweepLayers[i]->m_vpRoute[j + 1]);
			DrawCuboid(p1, p2, model->m_vpSweepLayers[i]->m_vCoordinate[2]);
		}
	}
}

void COpenGLDC::DrawCylinder(CPoint3D p1, CPoint3D p2)
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
	GLdouble radius = 0.2;        // 半径  
	GLdouble slices = 30.0;      //  段数  
	GLdouble stack = 3.0;       // 递归次数  
	gluCylinder(quad_obj, radius, radius, bone_length, slices, stack);
	glPopMatrix();
}

void COpenGLDC::DrawCuboid(CPoint3D pStart, CPoint3D pEnd, CVector3D normal)
{
	CVector3D xAxis = CVector3D(pStart, pEnd);
	xAxis.Normalize();
	CVector3D zAxis = normal;
	zAxis.Normalize();
	CVector3D yAxis = zAxis * xAxis;
	yAxis.Normalize();
	CVector3D coordinate[3];
	coordinate[0] = xAxis;
	coordinate[1] = yAxis;
	coordinate[2] = zAxis;
	CPoint3D startQuad[4];
	PointToQuad(startQuad, pStart, 0.2, coordinate);
	CPoint3D endQuad[4];
	PointToQuad(endQuad, pEnd, 0.2, coordinate);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glBegin(GL_QUADS);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(startQuad[0].x, startQuad[0].y, startQuad[0].z);
	glVertex3f(startQuad[1].x, startQuad[1].y, startQuad[1].z);
	glVertex3f(startQuad[2].x, startQuad[2].y, startQuad[2].z);
	glVertex3f(startQuad[3].x, startQuad[3].y, startQuad[3].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(endQuad[0].x, endQuad[0].y, endQuad[0].z);
	glVertex3f(endQuad[1].x, endQuad[1].y, endQuad[1].z);
	glVertex3f(endQuad[2].x, endQuad[2].y, endQuad[2].z);
	glVertex3f(endQuad[3].x, endQuad[3].y, endQuad[3].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(startQuad[0].x, startQuad[0].y, startQuad[0].z);
	glVertex3f(startQuad[1].x, startQuad[1].y, startQuad[1].z);
	glVertex3f(endQuad[1].x, endQuad[1].y, endQuad[1].z);
	glVertex3f(endQuad[0].x, endQuad[0].y, endQuad[0].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(startQuad[1].x, startQuad[1].y, startQuad[1].z);
	glVertex3f(startQuad[2].x, startQuad[2].y, startQuad[2].z);
	glVertex3f(endQuad[2].x, endQuad[2].y, endQuad[2].z);
	glVertex3f(endQuad[1].x, endQuad[1].y, endQuad[1].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(startQuad[2].x, startQuad[2].y, startQuad[2].z);
	glVertex3f(startQuad[3].x, startQuad[3].y, startQuad[3].z);
	glVertex3f(endQuad[3].x, endQuad[3].y, endQuad[3].z);
	glVertex3f(endQuad[2].x, endQuad[2].y, endQuad[2].z);
	glEnd();
	glBegin(GL_QUADS);
	//glNormal3f(xAxis.dx, xAxis.dy, xAxis.dz);
	glVertex3f(startQuad[3].x, startQuad[3].y, startQuad[3].z);
	glVertex3f(startQuad[0].x, startQuad[0].y, startQuad[0].z);
	glVertex3f(endQuad[0].x, endQuad[0].y, endQuad[0].z);
	glVertex3f(endQuad[3].x, endQuad[3].y, endQuad[3].z);
	glEnd();
}