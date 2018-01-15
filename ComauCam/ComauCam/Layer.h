#pragma once
#include "GeomBase.h"
#include "LVertex.h"
#include <vector>

using namespace std;

//struct SliceLayer;

struct LPoint : public CPoint3D
{
public:
	LPoint();
	~LPoint();
	LPoint(const LPoint& lpoint);
	LPoint(const CPoint3D& pt);
	LPoint(double x, double y, double z);

	LPoint operator+=(const CVector3D& v);                 // ��������ӵ�
public:
	//LPoint *p_prev, *p_next;
};

struct FPoint : public CPoint3D
{
public:
	FPoint();
	~FPoint();
	FPoint(const CPoint3D& pt, double a, double c);
	FPoint(const CPoint3D& pt);
	FPoint(const FPoint& pt);

public:
	double A;
	double C;
};

struct Segment
{
public:
	Segment();
	Segment(const Segment& lsegment);
	Segment(LPoint startpoint, LPoint endpoint, LTriangle* tri);
	~Segment();

public:
	LPoint pstart, pend;
	LTriangle* triangle;
	CVector3D segment_vec;
};

struct Boundary
{
public:
	Boundary();
	~Boundary();

public:
//	vector<LPoint*> m_Linkpoints;
	vector<Segment*> m_segments;
};

//��Ƭƽ��
struct SliceLayer
{
public:
	SliceLayer();
	~SliceLayer();

public:
	CPoint3D layerPoint;
	CVector3D layer_coordinate[3];
	vector<Boundary*> m_Boundaries;   //�ؽ��õ��������������޸� size = 1, ��������ڲ��пն�������������� size ������
};

//ɨ��·���������ཻ�õ��Ľ���
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

public:
	CPoint3D line_point;
	CVector3D line_vec;
	CVector3D line_normal;
};

struct SweepLayer : public SliceLayer
{
public:
	SweepLayer();
	SweepLayer(const SliceLayer& sliceLayer);
	SweepLayer(const SweepLayer& sweepLayer);
	~SweepLayer();

public:
	vector<SweepLine*> m_sweeplines;
	vector<Boundary*> offsetBoundaries;
	vector<CPoint3D*> m_Route;
	vector<FPoint*> m_turnRoute;
};

/*
struct Boundary
{
public:
Boundary();
~Boundary();
vector<PolyLine*> m_Boundary;
};
*/
//��������ƫ��
/*
struct CLine
{
public:
CLine();
~CLine();
CPoint3D line_point1;
CPoint3D line_point2;
CVector3D line_vec;
};*/
