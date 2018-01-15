#pragma once
#include "Slice.h"
#include <vector>
#include "GeomBase.h"
#include <algorithm>

using namespace std;


#define ANGLE_RESOLUTION 1

struct SweepPoint : public CPoint3D
{
public:
	SweepPoint();
	SweepPoint(double ix, double iy, double iz, bool left);
	~SweepPoint();
	bool isLeft;
};

struct SweepLine
{
public:
	SweepLine();
	~SweepLine();
	vector<SweepPoint*> m_Sweeplines;
};

struct CLine
{
public:
	CLine();
	~CLine();
	CPoint3D line_point1;
	CPoint3D line_point2;
	CVector3D line_vec;
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

	void offSet(Layer* layer, double offset, int z);	//����ĳЩ��С���������������յõ���ƫ������������ԭ����������
	bool isBoundaryCCW(Layer* layer, int z);	//�������ںܴ����⣬��������ĳ��������ֵ�ǳ�Сʱ�����ܻ����һЩĪ�����������
	void makeBoundaryCCW(Layer* layer, int z);

public:
	vector<Boundary*> m_Boundaries;		// �����ź����������
	vector<Boundary*> m_offsetBoundaries;
	vector<Layer*> m_Sweep_Layers;	//����һ����ƽ�棬��Ϊ��ʼ����
	vector<SweepPoint*> m_Routine;	//����·��

	double distance;	//ɨ��·�����
};
