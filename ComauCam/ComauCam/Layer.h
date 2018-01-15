#pragma once
#include "GeomBase.h"
#include "LVertex.h"
#include <vector>

using namespace std;

//struct SliceLayer;


//ɨ��·���������ཻ�õ��Ľ���
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
	CSegment(CPoint3D startpoint, CPoint3D endpoint, CLTriangle* tri);
	~CSegment();
public:
	CPoint3D m_ptStart, m_ptEnd;
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

//��Ƭƽ��
struct CSliceLayer
{
public:
	CSliceLayer();
	~CSliceLayer();

public:
	bool IsBoundaryCCW();
	void MakeBoundaryCCW();
	void RearrangeBoundary();
	void DeletePoints();
	void OptimizeBoundary();
	double ZminofLayer();
	int FindLowestSegment();

public:
	CPoint3D m_ptLayerPoint;
	CVector3D m_vLayerCoordinate[3];
	vector<CBoundary*> m_vecpBoundaries;   //�ؽ��õ��������������޸� size = 1, ��������ڲ��пն�������������� size ������

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


//�������

//�ƶ��߶�
void AFX_API_EXPORT MoveSegment(CSegment* lsegment, CVector3D vec);

//�����߶ν���
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CSegment seg1, CSegment seg2);
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CSweepLine line1, CSegment line2);

//��һ��Ϊ���ģ������һ��������
void PointToQuad(CPoint3D quad[4], CPoint3D point, double offset, CVector3D coordinate[3]);
