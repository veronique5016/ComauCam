#pragma once

#include <afxwin.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <glut.h>
#include <glaux.h>
#pragma comment(lib,"OPENGL32.LIB") // �����⣬gl��ͷ�ĺ��������������gl
#pragma comment(lib,"glu32.lib")    // ʵ�ÿ�
#pragma comment(lib,"glut32.lib")   // �������ʵ�ÿ�
#pragma comment(lib,"glut.lib")     // ������һ����,����һ���Ϳ�����
#pragma comment(lib,"GlAux.Lib")    // �ٷ����еĸ�����

#include "GeomBase.h"

#define LS_SOLID        0   // line style
#define LS_DASH         1
#define LS_CENTER       2


class AFX_CLASS_EXPORT COpenGLDC
{
public:
	COpenGLDC();
	virtual ~COpenGLDC();

public:
	void DrawAxis();
	void DrawTriChip(CVector3D vec, CPoint3D pt1, CPoint3D pt2, CPoint3D pt3);
	void DrawTriFrame(CPoint3D A, CPoint3D B, CPoint3D C);
	void DrawLine(const CPoint3D& sp, const CPoint3D& ep, int lineStyle = LS_SOLID, int lineWidth = 1);
};
