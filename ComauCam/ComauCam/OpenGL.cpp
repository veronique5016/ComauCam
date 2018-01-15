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
	//glTranslatef(0.0f, 0.0f, 0.0f);//290.16
}


void COpenGLDC::DrawTriChip(CVector3D vec, CPoint3D pt1, CPoint3D pt2, CPoint3D pt3)
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

void COpenGLDC::DrawPoint(const CPoint3D & pt)
{
	glBegin(GL_POINTS);
		glVertex3f(pt.x, pt.y, pt.z);
	glEnd();
}

void COpenGLDC::DrawSelPoint(const CPoint3D & pt, int ShapeType, COLORREF clr)
{
	double len = 10; // 暂时不清楚作用，后期要进行修改
	switch(ShapeType)
	{
	case ST_RECT:
		{
			CPoint3D pt1(pt.x - len, pt.y - len, pt.z);
			CPoint3D pt2(pt.x + len, pt.y - len, pt.z);
			CPoint3D pt3(pt.x + len, pt.y + len, pt.z);
			CPoint3D pt4(pt.x - len, pt.y + len, pt.z);

			DrawLine(pt1, pt2);
			DrawLine(pt2, pt3);
			DrawLine(pt3, pt4);
			DrawLine(pt4, pt1);
			// 			DrawTriChip(CVector3D(0,0,1),pt1,pt2,pt3);
			// 			DrawTriChip(CVector3D(0,0,1),pt3,pt4,pt1);
			break;
		}
	case ST_CIRCLE:
		{
			double step = 2 * PI / 20;
			double r = 1.2*len;
			CPoint3D ptFrom(pt.x + r, pt.y, 0);
			CPoint3D ptTo;
			for (double theta = step; theta<2 * PI; theta = theta + step)
			{
				ptTo = CPoint3D(pt.x + r*cos(theta), pt.y + r*sin(theta), 0);
				DrawLine(ptFrom, ptTo);
				ptFrom = ptTo;
			}
			ptTo = CPoint3D(pt.x + r, pt.y, 0);
			DrawLine(ptFrom, ptTo);

			break;
		}	
	case ST_TRIANGLE:
		{
			double r = 1.5*len;
			CPoint3D pt1(pt.x - 0.866*r, pt.y - 0.5*r, 0);
			CPoint3D pt2(pt.x + 0.866*r, pt.y - 0.5*r, 0);
			CPoint3D pt3(pt.x, pt.y + r, 0);
			DrawLine(pt1, pt2);
			DrawLine(pt2, pt3);
			DrawLine(pt3, pt1);

			break;
		}

	}
}

CEntity::CEntity()
{
	m_ShowTriangleFace = false;
}

CEntity::~CEntity()
{
}
