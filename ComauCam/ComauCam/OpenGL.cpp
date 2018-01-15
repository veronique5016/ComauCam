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
	glTranslatef(0.0f, 0.0f, 0.0f);//-290.16

								   //µÿ√Ê		
	glLineWidth(4.0);
	int R = 500;
	for (int j = 0; j < 1000; j += 100)
	{
		glBegin(GL_LINE_LOOP);
		glColor3f(0.5f, 0.5f, 0.5f);
		for (int i = 0; i < 200; i++)
		{
			glVertex3f(R * cos(2 * PI*i / 200), R * sin(2 * PI*i / 200), j);
		}
		glEnd();
		R += 50;
	}
	glBegin(GL_LINES);
	glVertex3f(-500.0, 0.0, 0.0);
	glVertex3f(-950.0, 0.0, 900.0);
	glVertex3f(500.0, 0.0, 0.0);
	glVertex3f(950.0, 0.0, 900.0);
	glVertex3f(0.0, -500.0, 0.0);
	glVertex3f(0.0, -950.0, 900.0);
	glVertex3f(0.0, 500.0, 0.0);
	glVertex3f(0.0, 950.0, 900.0);

	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	/*			for(int i=0;i<=10;i++)
	{
	glVertex3f(-500.0f,-500.0f+100.0f*i,0.0f);
	glVertex3f(500.0f,-500.0f+100.0f*i,0.0f);
	glVertex3f(-500.0f+100.0f*i,-500.0f,0.0f);
	glVertex3f(-500.0f+100.0f*i,500.0f,0.0f);
	} */

	for (int i = 0; i <= 270; i += 10)
	{
		glVertex3f(500 * cos(2 * PI*i / 360), 500 * sin(2 * PI*i / 360), 0.0);
		glVertex3f(-500 * cos(2 * PI*i / 360), 500 * sin(2 * PI*i / 360), 0.0);
		glVertex3f(500 * cos(2 * PI*i / 360), 500 * sin(2 * PI*i / 360), 0.0);
		glVertex3f(500 * cos(2 * PI*i / 360), -500 * sin(2 * PI*i / 360), 0.0);
	}
	// x÷·
	glColor3f(1.0F, 0.0F, 0.0F);
	glVertex3f(-90.0f, 0.0f, 0.0f);
	glVertex3f(90.0f, 0.0f, 0.0f);
	glVertex3f(75.0f, 15.0f, 0.0f);
	glVertex3f(90.0f, 0.0f, 0.0f);
	glVertex3f(75.0f, -15.0f, 0.0f);
	glVertex3f(90.0f, 0.0f, 0.0f);

	// y÷·
	glColor3f(0.0F, 1.0F, 0.0F);
	glVertex3f(0.0f, -90.0f, 0.0f);
	glVertex3f(0.0f, 90.0f, 0.0f);
	glVertex3f(-15.0f, 75.0f, 0.0f);
	glVertex3f(0.0f, 90.0f, 0.0f);
	glVertex3f(15.0f, 75.0f, 0.0f);
	glVertex3f(0.0f, 90.0f, 0.0f);

	// z÷·
	glColor3f(0.0F, 0.0F, 1.0F);
	glVertex3f(0.0f, 0.0f, -90.0f);
	glVertex3f(0.0f, 0.0f, 90.0f);
	glVertex3f(-15.0f, 0.0f, 75.0f);
	glVertex3f(0.0f, 0.0f, 90.0f);
	glVertex3f(15.0f, 0.0f, 75.0f);
	glVertex3f(0.0f, 0.0f, 90.0f);

	glEnd();
	glTranslatef(0.0f, 0.0f, 0.0f);//290.16
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