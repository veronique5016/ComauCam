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
	void LoadSTLModel(vector<CSTLModel*>& models);					 //����ģ��
	void Slice();

protected:
	void CalcTrisEigenHeight();										 //��������Ƭ�����߶�
	bool CalcTriXYPlaneInst(Triangle* pTri, double z, CLineEx& line);//������xyƽ��Ĺ�ϵ
public:
	vector<CSTLModel*> m_STLModels;
	vector<CSliceModel*> m_slices;                                   //�������Ƭģ��

	vector<double> m_Zs;
//	vector<double> points;
protected:
	vector<Triangle*> m_tris;
};


