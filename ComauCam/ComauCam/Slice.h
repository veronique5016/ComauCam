#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include "SliceModel.h"
#include <vector>

using namespace std;

class CSlice
{
public:
	struct Triangle;
	struct CLineEx;
	//struct Segment;
	//struct Pt_Seg;

	CSlice(void);
	~CSlice(void);
	void LoadSTLModel(vector<CSTLModel*>& models);					 //载入模型
	void Slice();

protected:
	void CalcTrisEigenHeight();										 //计算三角片特征高度
	bool CalcTriXYPlaneInst(Triangle* pTri, double z, CLineEx& line);//计算与xy平面的关系
public:
	vector<CSTLModel*> m_STLModels;
	vector<CSliceModel*> m_slices;                                   //输出的切片模型

	vector<double> m_Zs;
//	vector<double> points;
protected:
	vector<Triangle*> m_tris;
};


