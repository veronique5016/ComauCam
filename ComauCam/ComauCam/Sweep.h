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
	void YaxisSweep(CSweepLayer* layer);

	void CalACAngle(CSweepLayer* layer);
	void CalFiveAxisValue(CSweepLayer* layer);

public:
	vector<CSliceLayer*> m_vpSliceLayers;	//保存一份切平面，作为初始数据
	vector<CSweepLayer*> m_vpSweepLayers;

	double m_dDistance;	//扫描路径间距
};
