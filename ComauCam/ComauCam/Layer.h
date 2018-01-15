#pragma once
#include "GeomBase.h"
#include "LVertex.h"
#include <vector>

using namespace std;

struct Layer;

struct LPoint : public CPoint3D
{
public:
	LPoint();
	~LPoint();
	LPoint(const LPoint& lpoint);
	LPoint(const CPoint3D& pt);
	LPoint(double x, double y, double z);

public:
	LPoint *p_prev, *p_next;
};

struct LSegment
{
public:
	LSegment();
	LSegment(const LSegment& lsegment);
	LSegment(LPoint* startpoint, LPoint* endpoint, LTriangle* tri);
	~LSegment();

public:
	LPoint *pstart, *pend;
	LTriangle* triangle;
};

struct PolyLine
{
public:
	PolyLine();
	~PolyLine();

public:
	vector<LPoint*> m_Linkpoints;
	vector<LSegment*> m_Linklines;
};

struct Layer
{
public:
	Layer();
	~Layer();

public:
	CPoint3D layerPoint;
	CVector3D layer_coordinate[3];
	vector<PolyLine*> m_Polylines;   //截交得到的轮廓，不经修改 size = 1
};