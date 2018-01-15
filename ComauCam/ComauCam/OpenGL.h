#pragma once

#include <afxwin.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <glut.h>
#include <glaux.h>
#pragma comment(lib,"OPENGL32.LIB") // 基本库，gl打头的函数都在这个库里gl
#pragma comment(lib,"glu32.lib")    // 实用库
#pragma comment(lib,"glut32.lib")   // 民间做的实用库
#pragma comment(lib,"glut.lib")     // 和上面一样的,包含一个就可以了
#pragma comment(lib,"GlAux.Lib")    // 官方发行的辅助库

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
