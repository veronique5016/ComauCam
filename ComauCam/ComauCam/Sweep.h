#pragma once
#include "Slice.h"
#include <vector>
#include "GeomBase.h"
#include <algorithm>

using namespace std;

#define EXTRUDER_DIAMETER 0.4
#define ANGLE_RESOLUTION 1

struct SweepPoint : public CPoint3D
{
public:
	SweepPoint();
	SweepPoint(double ix, double iy, double iz, bool left);
	~SweepPoint();
	bool isLeft;
};

struct Boundary
{
public:
	Boundary();
	~Boundary();
	double z;
	vector<CPoint3D*> m_Boundary;
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
	void generateYPoint(int ext[], double y, int z);
	void generateXPoint(int ext[], double x, int z);
	SweepPoint* getIntersectY(const CPoint3D* p1, const CPoint3D* p2, double y, double z, bool left);
	SweepPoint* getIntersectX(const CPoint3D* p1, const CPoint3D* p2, double x, double z, bool left);
	void drawRoute();
	SweepPoint* pointToSweeppoint(CPoint3D* p, bool left);
	void writeGCode(CString sFilePath);

public:
	vector<Boundary*> m_Boundaries;		// �����ź����������
	vector<Layer*> m_Sweep_Layers;	//����һ����ƽ�棬��Ϊ��ʼ����
	vector<SweepPoint*> m_Routine;	//����·��

	double distance;	//ɨ��·�����
};
