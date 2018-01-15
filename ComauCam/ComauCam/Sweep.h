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
	~SweepPoint();
	bool left;
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

class CSweep
{
public:
	CSweep();
	~CSweep();
	void loadSliceModel(CSlice* slicemodel);
	void sweep();
	void rearrange(int ext[], int i);
	void deletePoints(int i);
	void findExtreme(int ext[], int i);
	void generatePoint(double y, int i, int j);
	CPoint3D* getIntersect(const CPoint3D* p1, const CPoint3D* p2, double y, double z);
	void drawRoute();

public:
	vector<CPoint3D*> m_Sweeppoints;		// �����ź����������
	vector<SweepLine*> m_SweepLines;	// ����ɨ����
	vector<Layer*> m_Sweep_Layers;	//����һ����ƽ�棬��Ϊ��ʼ����
	vector<CPoint3D*> m_Routine;	//����·��
};
