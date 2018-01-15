#pragma once
#include "GeomBase.h"
#include "LVertex.h"
#include <vector>

using namespace std;

//struct SliceLayer;

struct CLPoint : public CPoint3D
{
public:
	CLPoint();
	~CLPoint();
	CLPoint(const CLPoint& lpoint);
	CLPoint(const CPoint3D& pt);
	CLPoint(double x, double y, double z);

	CLPoint operator+=(const CVector3D& v);                 // 点和向量加等
public:
	//LPoint *p_prev, *p_next;
};

//扫描路径与轮廓相交得到的交点
struct CSweepPoint : public CPoint3D
{
public:
	CSweepPoint();
	~CSweepPoint();
	CSweepPoint(const CPoint3D& pt, double a, double c);
	CSweepPoint(const CSweepPoint& pt);

public:
	double A;
	double C;
};

struct CSegment
{
public:
	CSegment();
	CSegment(const CSegment& lsegment);
	CSegment(CLPoint startpoint, CLPoint endpoint, CLTriangle* tri);
	~CSegment();
public:
	CLPoint m_ptStart, m_ptEnd;
	CLTriangle* m_pTriangle;
	CVector3D m_vSegmentVec;
};

struct CBoundary
{
public:
	CBoundary();
	CBoundary(const CBoundary& boundary);
	~CBoundary();

public:
	vector<CSegment*> m_vecpSegments;
};

//切片平面
struct CSliceLayer
{
public:
	CSliceLayer();
	~CSliceLayer();

public:
	CPoint3D m_ptLayerPoint;
	CVector3D m_vLayerCoordinate[3];
	vector<CBoundary*> m_vecpBoundaries;   //截交得到的轮廓，不经修改 size = 1, 如果轮廓内部有空洞或其他情况，则 size 会增加

public:
	CSliceLayer* m_pAddictedLayer;
};

struct CSweepLine
{
public:
	CSweepLine();
	~CSweepLine();

public:
	CPoint3D m_ptLinePoint;
	CVector3D m_vLineVec;
	CVector3D m_vLineNormal;

public:
	vector<CSweepPoint> m_ptCrossPoint;
};

struct CSweepLayer : public CSliceLayer
{
public:
	CSweepLayer();
	CSweepLayer(const CSliceLayer& sliceLayer);
	CSweepLayer(const CSweepLayer& sweepLayer);
	~CSweepLayer();

public:
	vector<CSweepLine*> m_vecpSweepLines;
	vector<CBoundary*> m_vecpOffsetBoundaries;
	vector<CPoint3D*> m_vecpRoute;
	vector<CSweepPoint*> m_vecpTurnRoute;
};


//输出函数

//移动线段
void AFX_API_EXPORT MoveSegment(CSegment* lsegment, CVector3D vec);

//求两线段交点
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CSegment seg1, CSegment seg2);
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CSweepLine line1, CSegment line2);

//以一点为中心，扩充成一个正方形
void PointToQuad(CPoint3D quad[4], CPoint3D point, double offset, CVector3D coordinate[3]);
