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
#include "STLModel.h"
#include "Slice.h"
#include "Sweep.h"
#include "Layer.h"

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
	void DrawTriFace(CVector3D vec, CPoint3D pt1, CPoint3D pt2, CPoint3D pt3);
	void DrawTriFrame(CPoint3D A, CPoint3D B, CPoint3D C);
	void DrawLine(const CPoint3D& sp, const CPoint3D& ep, int lineStyle = LS_SOLID, int lineWidth = 1);
	void DrawCylinder(CPoint3D p1, CPoint3D p2);  //路径用圆柱表示
	void DrawCuboid(CPoint3D pStart, CPoint3D pEnd, CVector3D normal);  //路径用四棱柱表示
	void DrawSTLModel(CSTLModel* model, bool showTri);
	void DrawLayer(CSliceLayer* layer, bool showPolygon, double color[]);
	void DrawSliceModel(CSlice* model, bool showPolygon, int start, int end);
	void DrawSweepModel(CSweep* model, int start, int end);
};
