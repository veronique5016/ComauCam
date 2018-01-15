#pragma once
#include "Slice.h"
#include <vector>
#include "GeomBase.h"
#include <algorithm>

using namespace std;

#define EXTRUDER_DIAMETER 0.4

struct SweepPoint : public CPoint3D
{
public:
	SweepPoint();
	SweepPoint(double ix, double iy, double iz, bool left);
	~SweepPoint();
	bool isLeft;
	double getLength(const SweepPoint* p1, const SweepPoint* p2);
};

struct SweepLine
{
public:
	SweepLine();
	~SweepLine();
	double y;
	vector<SweepPoint*> m_IntersectPoints;
	bool isLeft(SweepPoint* p1, SweepPoint* p2);
};

struct Boundary
{
public:
	Boundary();
	~Boundary();
	double z;
	vector<CPoint3D*> m_boundary;
};

class CSweep
{
public:
	CSweep();
	~CSweep();
	void loadSliceModel(CSlice* slicemodel);
	void sweep();
	void rearrange(int ext[], int z);
	void deletePoints(int z);
	void findExtreme(int ext[], int z);
	void generatePoint(double y, int i, int j, int z);
	SweepPoint* getIntersect(const CPoint3D* p1, const CPoint3D* p2, double y, double z, bool left);
	void drawRoute();
	SweepPoint* pointToSweeppoint(CPoint3D* p, bool left);
	void writeGCode(CString sFilePath);

public:
	vector<Boundary*> m_Boundaries;		// 储存排好序的轮廓点
	vector<SweepLine*> m_SweepLines;	// 储存扫描线
	vector<Layer*> m_Sweep_Layers;	//保存一份切平面，作为初始数据
	vector<SweepPoint*> m_Routine;	//保存路径
};
