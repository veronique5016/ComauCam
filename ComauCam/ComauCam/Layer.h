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

	CLPoint operator+=(const CVector3D& v);                 // ��������ӵ�
public:
	//LPoint *p_prev, *p_next;
};

struct CFPoint : public CPoint3D
{
public:
	CFPoint();
	~CFPoint();
	CFPoint(const CPoint3D& pt, double a, double c);
	CFPoint(const CPoint3D& pt);
	CFPoint(const CFPoint& pt);

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
	~CBoundary();

public:
	vector<CSegment*> m_vecpSegments;
};

//��Ƭƽ��
struct CSliceLayer
{
public:
	CSliceLayer();
	~CSliceLayer();

public:
	CPoint3D m_ptLayerPoint;
	CVector3D m_vLayerCoordinate[3];
	vector<CBoundary*> m_vecpBoundaries;   //�ؽ��õ��������������޸� size = 1, ��������ڲ��пն�������������� size ������
};

//ɨ��·���������ཻ�õ��Ľ���
struct CSweepPoint : public CPoint3D
{
public:
	CSweepPoint();
	CSweepPoint(double ix, double iy, double iz, bool left);
	~CSweepPoint();
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
	vector<CFPoint*> m_vecpTurnRoute;
};


//�������

//�ƶ��߶�
void AFX_API_EXPORT MoveSegment(CSegment* lsegment, CVector3D vec);

//�����߶ν���
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CSegment seg1, CSegment seg2);
