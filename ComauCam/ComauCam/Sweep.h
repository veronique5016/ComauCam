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
	void Offset(CSweepLayer* layer, double offset);	//����ĳЩ��С���������������յõ���ƫ������������ԭ����������
	void YaxisSweep(CSweepLayer* layer);

	void CalACAngle(CSweepLayer* layer);
	void CalFiveAxisValue(CSweepLayer* layer);

public:
	vector<CSliceLayer*> m_vecpSliceLayers;	//����һ����ƽ�棬��Ϊ��ʼ����
	vector<CSweepLayer*> m_vecpSweepLayers;

	double m_dDistance;	//ɨ��·�����
};
