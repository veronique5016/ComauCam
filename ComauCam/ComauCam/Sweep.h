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
	void LoadSliceModel(CSlice* slicemodel);
	void Sweep();
	void Offset(CSweepLayer* layer, double offset);	//由于某些很小的向量分量，最终得到的偏置轮廓可能与原轮廓不共面
	void YaxisSweep(CSweepLayer* layer);

	void CalACAngle(CSweepLayer* layer);
	void CalFiveAxisValue(CSweepLayer* layer);

public:
	vector<CSliceLayer*> m_vecpSliceLayers;	//保存一份切平面，作为初始数据
	vector<CSweepLayer*> m_vecpSweepLayers;

	double m_dDistance;	//扫描路径间距
};
