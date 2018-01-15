#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include <vector>
#include "OpenGL.h"
#include "Layer.h"
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

class CSlice
{
public:
	CSlice(void);
	~CSlice(void);

public:
	void loadSTLModel(CSTLModel* models);	//载入模型，建立拓扑关系
	void slice(CSTLModel* model);  //切片函数
	double getTurnHeight();     // 获取模型转折点
	CVector3D getTurnVec();
	void begin3DSlice(double z_min, double z_max, double& z, double dz);
	void begin5DSlice(double z_min, double z_max, double& z, double dz);
	void getBoundaryPoints(SliceLayer* layer);
	void calIntersectPoint(SliceLayer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point);	// 求轮廓点
	void getInterSectEdge(SliceLayer* layer, LTriangle* pCurFace);   //轮廓求交算法初始化，求出第一条交线，和第一个交点；

public:
	void judgeFaceType(SliceLayer* layer, LTriangle* pCurFace);    //判断与切片平面相交面片的类型
	void judgeOtherLine(SliceLayer* layer, LTriangle* pCurFace);   //判断另一条相交线
	double compareThreeNumber(double v1, double v2, double v3, int type);
	bool isBoundaryCCW(SliceLayer* layer);	//函数存在很大问题，当向量的某个分量的值非常小时，可能会出现一些莫名其妙的问题 
	void makeBoundaryCCW(SliceLayer* layer);
	void deletePoints(SliceLayer* layer);
	void drawLayer(bool showPolygon, int start, int end);
	bool lineNeedSupport();

public:
	vector<LEdge*> m_Slice_edge;     //存储相交边
	vector<SliceLayer*> m_layers;         //存储片层
	vector<LTriangle*> m_tris_slice;   //保存一份三角面片，作为初始数据

	vector<LTriangle*> z_tris;

	double height;

	bool need_support;
};

