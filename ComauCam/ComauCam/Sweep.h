#pragma once
#include "Slice.h"
#include <vector>
#include "GeomBase.h"
#include <algorithm>
#include "Layer.h"

using namespace std;

class CSweep
{
public:
	CSweep();
	~CSweep();

public:
	void loadSliceModel(CSlice* slicemodel);
	void sweep();
	void offSet(SweepLayer* layer, double offset);	//由于某些很小的向量分量，最终得到的偏置轮廓可能与原轮廓不共面
	void yaxisSweep(SweepLayer* layer);
	void drawRoute(int begin, int end);

	void cal_AC_angle(SweepLayer* layer);
	void calFiveAxisValue(SweepLayer* layer);

public:
	vector<SliceLayer*> m_slice_layers;	//保存一份切平面，作为初始数据
	vector<SweepLayer*> m_sweep_layers;

//	vector<SweepLayer*> temp_layers;
	double distance;	//扫描路径间距
};
