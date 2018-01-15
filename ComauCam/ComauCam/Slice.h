#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include <vector>
#include "OpenGL.h"
using namespace std;


//面片类型 
#define NO_POINT_ON_SURFACE 1
#define ONE_POINT_ON_SURFACE 2 
#define EDGE_ON_SURFACE 3
#define ONLY_ONE_POINT_ON_SURFACE 4

struct PolyLine
{
public:
	PolyLine();
	~PolyLine();
	vector<CPoint3D*> m_Linkpoints;
};

struct Layer
{
public:
	Layer();
	~Layer();
	double z;
	vector<PolyLine*> m_Polylines;
};

class CSlice
{
public:
	CSlice(void);
	~CSlice(void);
	void LoadSTLModel(CSTLModel* models);	//载入模型，建立拓扑关系
	void slice(CSTLModel* model);  //切片函数
	void getpolylinePoints(double z);
	void drawpolyline();

protected:
	void InterSect(double z, LTriangle* pCurFace);   //轮廓求交算法初始化，求出第一条交线，和第一个交点；
	void CalInerSectPoint(double z, LEdge * edge, LTriangle*pCurFace, CPoint3D* point);
	void JudgeFaceType(double z, LTriangle* pCurFace);    //判断面片的类型
	void JudgeOtherLine(double z, LTriangle* pCurFace);   //判断另一条相交线
	double ReturnZmin(const LTriangle* Ltri);   //返回一个面片的最小Z坐标值
	double ReturnZmax(const LTriangle* Ltri);
	double ReturnZmid(const LTriangle* Ltri);

public:
	vector<LEdge*> m_Slice_edge;     //存储相交边
	vector<Layer*> m_layers;         //存储片层
	vector<LTriangle*> m_tris_slice;   //保存一份三角面片，作为初始数据
};

