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
	void getBoundaryPoints(SliceLayer* layer);
	void calIntersectPoint(SliceLayer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point);	// 求切片平面与某条边相交得到的轮廓点
	void getInterSectEdge(SliceLayer* layer, LTriangle* pCurFace);   //轮廓求交算法初始化，求出第一条交线，和第一个交点；

public:
	void judgeFaceType(SliceLayer* layer, LTriangle* pCurFace);    //判断与切片平面相交面片的类型
	void judgeOtherLine(SliceLayer* layer, LTriangle* pCurFace);   //判断另一条相交线
	double compareThreeNumber(double v1, double v2, double v3, int type);
	double zminofLayer(SliceLayer* layer);
	void optimizeBoundary(SliceLayer* layer);
	bool isBoundaryCCW(SliceLayer* layer);	//函数存在很大问题，当向量的某个分量的值非常小时，可能会出现一些莫名其妙的问题 
	void makeBoundaryCCW(SliceLayer* layer);
	void deletePoints(SliceLayer* layer);
	void drawLayer(bool showPolygon, int start, int end);

public:
	vector<LEdge*> m_Slice_edge;     //存储相交边
	vector<SliceLayer*> m_layers;     //存储片层
	vector<LTriangle*> m_tris_slice;   //保存一份三角面片，作为初始数据
	vector<int> index;	//保存所有法向量为 z 轴正方向的面片序号
	double height; // 层切面间距
};

