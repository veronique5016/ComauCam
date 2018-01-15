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

#define MIN 1
#define MID 2
#define MAX 3
struct Layer;

struct LPoint : public CPoint3D
{
public:
	LPoint();
	~LPoint();
	LPoint(const LPoint& lpoint);
	LPoint(const CPoint3D& pt);
	LPoint(double x, double y, double z);

public:
	LPoint *p_prev, *p_next;
};

struct LLine
{
public:
	LLine();
	LLine(const LPoint* startpoint, const LPoint* endpoint);
	~LLine();

public:
	LPoint pstart, pend;
	Layer* layer;
};

struct PolyLine
{
public:
	PolyLine();
	~PolyLine();

public:
	vector<LPoint*> m_Linkpoints;
	vector<LLine*> m_Linklines;
};

struct Layer
{
public:
	Layer();
	~Layer();

public:
	CPoint3D layerPoint;
	CVector3D layer_coordinate[3];
	vector<PolyLine*> m_Polylines;   //截交得到的轮廓，不经修改 size = 1
};

class CSlice
{
public:
	CSlice(void);
	~CSlice(void);
	void loadSTLModel(CSTLModel* models);	//载入模型，建立拓扑关系
	void slice(CSTLModel* model);  //切片函数
	void getPolylinePoints(Layer* layer);
	void drawLayer(bool showPolygon, int start, int end);
	double getTurnHeight();     // 获取模型转折点
	CVector3D getTurnVec();

public:
	void getInterSectEdge(Layer* layer, LTriangle* pCurFace);   //轮廓求交算法初始化，求出第一条交线，和第一个交点；
	void calIntersectPoint(Layer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point);	// 求轮廓点
	void judgeFaceType(Layer* layer, LTriangle* pCurFace);    //判断与切片平面相交面片的类型
	void judgeOtherLine(Layer* layer, LTriangle* pCurFace);   //判断另一条相交线
	double compareThreeNumber(double v1, double v2, double v3, int type);

public:
	void begin3DSlice(double z_min, double z_max, double& z, double dz);
	void begin5DSlice(double z_min, double z_max, double& z, double dz);
	bool isBoundaryCCW(Layer* layer);	//函数存在很大问题，当向量的某个分量的值非常小时，可能会出现一些莫名其妙的问题 
	void makeBoundaryCCW(Layer* layer);
	void deletePoints(Layer* layer);
	bool lineNeedSupport();

public:
	vector<LEdge*> m_Slice_edge;     //存储相交边
	vector<Layer*> m_layers;         //存储片层
	vector<LTriangle*> m_tris_slice;   //保存一份三角面片，作为初始数据
	vector<LTriangle*> z_tris;

	double height;

	bool need_support;
};

