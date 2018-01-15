#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include <vector>
#include "OpenGL.h"
using namespace std;


#define EXTRUDER_DIAMETER 0.4

//面片类型 
#define NO_POINT_ON_SURFACE 1
#define ONE_POINT_ON_SURFACE 2 
#define EDGE_ON_SURFACE 3
#define ONLY_ONE_POINT_ON_SURFACE 4

#define Z_MIN 1
#define Z_MID 2
#define Z_MAX 3
struct Layer;

struct LPoint : public CPoint3D
{
public:
	LPoint();
	~LPoint();
	LPoint(const LPoint& lpoint);
	LPoint(const CPoint3D& pt);

public:
	LPoint *p_prev, *p_next;
	LEdge* edge;
	Layer* layer;
};

struct PolyLine
{
public:
	PolyLine();
	~PolyLine();
	vector<LPoint*> m_Linkpoints;
};

struct Layer
{
public:
	Layer();
	~Layer();
	CPoint3D layerPoint;
	CVector3D layer_gravity;
	vector<PolyLine*> m_Polylines;   //截交得到的轮廓，不经修改 size = 1
};

class CSlice
{
public:
	CSlice(void);
	~CSlice(void);
	void LoadSTLModel(CSTLModel* models);	//载入模型，建立拓扑关系
	void slice(CSTLModel* model);  //切片函数
	void getpolylinePoints(Layer* layer);
	void drawpolyline(bool showPolygon);
	double getTurnHeight();     // 获取模型转折点
	CVector3D getTurnVec();

public:
	void getInterSectEdge(Layer* layer, LTriangle* pCurFace);   //轮廓求交算法初始化，求出第一条交线，和第一个交点；
	void CalIntersectPoint(Layer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point);	// 求轮廓点
	void JudgeFaceType(Layer* layer, LTriangle* pCurFace);    //判断与切片平面相交面片的类型
	void JudgeOtherLine(Layer* layer, LTriangle* pCurFace);   //判断另一条相交线
//	double ReturnZmin(const LTriangle* Ltri);   //返回一个面片的最小Z坐标值
//	double ReturnZmax(const LTriangle* Ltri);
//	double ReturnZmid(const LTriangle* Ltri);
	double ReturnZtype(double v1, double v2, double v3, int ztype);

public:
	void begin3DSlice(double z_min, double z_max, double& z, double dz);
	void begin5DSlice(double z_min, double z_max, double& z, double dz);
	bool isBoundaryCCW(Layer* layer);	//函数存在很大问题，当向量的某个分量的值非常小时，可能会出现一些莫名其妙的问题 
	void makeBoundaryCCW(Layer* layer);
//	void mergeTwoBoundary(Layer* layer1, Layer* layer2);
	void releaseMem();

	bool lineNeedSupport();

public:
	vector<LEdge*> m_Slice_edge;     //存储相交边
	vector<Layer*> m_layers;         //存储片层
	vector<LTriangle*> m_tris_slice;   //保存一份三角面片，作为初始数据
	vector<LTriangle*> z_tris;

	double height;
	CVector3D gravity;
	bool need_support;
};

