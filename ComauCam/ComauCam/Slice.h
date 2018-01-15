#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include <vector>
#include "Layer.h"
using namespace std;

#define EXTRUDER_DIAMETER 1

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
	void LoadSTLModel(CSTLModel* models);	//载入模型，建立拓扑关系
	void Slice(CSTLModel* model);  //切片函数

public:
	void GetBoundaryPoints(CSliceLayer* layer);
	void CalIntersectPoint(CSliceLayer* layer, CLEdge * edge, CLTriangle*pCurFace, CLPoint* point);	// 求切片平面与某条边相交得到的轮廓点
	void GetInterSectEdge(CSliceLayer* layer, CLTriangle* pCurFace);   //轮廓求交算法初始化，求出第一条交线，和第一个交点；
	void JudgeFaceType(CSliceLayer* layer, CLTriangle* pCurFace);    //判断与切片平面相交面片的类型
	void JudgeOtherLine(CSliceLayer* layer, CLTriangle* pCurFace);   //判断另一条相交线
	double ZminofLayer(CSliceLayer* layer);
	int FindLowestSegment(CSliceLayer* layer);

public:
	void GetAddedLayerBoundary(CSliceLayer* layer, CSegment turnlayer_seg, CSegment layer_seg);

public:
	double CompareThreeNumber(double v1, double v2, double v3, int type);
	void ModifyTurnLayer(CSliceLayer* layer);	//对变法向切平面轮廓进行修改
	void OptimizeBoundary(CSliceLayer* layer);
	bool IsBoundaryCCW(CSliceLayer* layer);		//判断轮廓是否逆时针
	void MakeBoundaryCCW(CSliceLayer* layer);
	void RearrangeBoundary(CSliceLayer* layer);
	void DeletePoints(CSliceLayer* layer);		//删除冗余点

public:
	vector<CLEdge*> m_vecpSliceEdge;     //存储相交边
	vector<CSliceLayer*> m_vecpLayers;     //存储片层
	vector<CLTriangle*> m_vecpSliceTris;   //保存一份三角面片，作为初始数据
	double m_dHeight; // 层切面间距
};

