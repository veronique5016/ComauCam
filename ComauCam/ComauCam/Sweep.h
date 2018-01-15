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
	void offSet(SweepLayer* layer, double offset);	//����ĳЩ��С���������������յõ���ƫ������������ԭ����������
	void yaxisSweep(SweepLayer* layer);
	void drawRoute(int begin, int end);

	void cal_AC_angle(SweepLayer* layer);
	void calFiveAxisValue(SweepLayer* layer);

public:
	vector<SliceLayer*> m_slice_layers;	//����һ����ƽ�棬��Ϊ��ʼ����
	vector<SweepLayer*> m_sweep_layers;

//	vector<SweepLayer*> temp_layers;
	double distance;	//ɨ��·�����
};
