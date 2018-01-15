#pragma once
#include "Slice.h"
#include <vector>
#include "GeomBase.h"
#include <algorithm>
#include "Layer.h"

using namespace std;


#define ANGLE_RESOLUTION 1

class CSweep
{
public:
	CSweep();
	~CSweep();

public:
	void loadSliceModel(CSlice* slicemodel);
	void sweep();
//	void rearrange(int ext[], SweepLayer* layer);
	//void deletePoints(int z);
	void findExtreme(int ext[], int z);
//	void generateYPoint(int ext[], double y, int z);
	void generateXPoint(int ext[], double x, int z);
//	SweepPoint* getIntersectY(const CPoint3D* p1, const CPoint3D* p2, double y, double z, bool left);
	SweepPoint* getIntersectX(const CPoint3D* p1, const CPoint3D* p2, double x, double z, bool left);
	void drawRoute();
	SweepPoint* pointToSweeppoint(CPoint3D* p, bool left);
	void writeGCode(CString sFilePath);

	void offSet(SweepLayer* layer, double offset);	//由于某些很小的向量分量，最终得到的偏置轮廓可能与原轮廓不共面
	void yaxisSweep(SweepLayer* layer);


public:
	vector<Boundary*> m_Boundaries;		// 储存排好序的轮廓点
	vector<Boundary*> m_offsetBoundaries;

	vector<SweepPoint*> m_Routine;	//保存路径

	double distance;	//扫描路径间距

public:
	vector<SliceLayer*> m_slice_layers;	//保存一份切平面，作为初始数据
	vector<SweepLayer*> m_sweep_layers;
};
