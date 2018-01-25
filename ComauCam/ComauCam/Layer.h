#pragma once
#include "GeomBase.h"
#include "LVertex.h"
#include <vector>

using namespace std;

//ɨ��·���������ཻ�õ��Ľ���
struct CSweepPoint : public CPoint3D
{
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
	CBoundary();
	CBoundary(const CBoundary& boundary);
	~CBoundary();

public:
	vector<CSegment*> m_vpSegments;
};

struct CSliceFrag
{
	CSliceFrag();
	CSliceFrag(const CSliceFrag& fragment);
	~CSliceFrag();

public:
	CVector3D m_vCoordinate[3];
	CPoint3D m_ptBoundary[4];
};

//��Ƭƽ��
struct CSliceLayer
{
	CSliceLayer();
	~CSliceLayer();

public:
	bool IsBoundaryCCW();
	void MakeBoundaryCCW();
	void RearrangeBoundary();
	void DeletePoints();
	void OptimizeBoundary();
	double ZminofLayer();

public:
	CPoint3D m_ptLayerPoint;
	CVector3D m_vCoordinate[3];
	vector<CBoundary*> m_vpBoundaries;   //�ؽ��õ��������������޸� size = 1, ��������ڲ��пն�������������� size ������
	vector<CSliceFrag*> m_vpFragments;

};

struct CSweepLine
{
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
	CSweepLayer();
	CSweepLayer(const CSliceLayer& sliceLayer);
	CSweepLayer(const CSweepLayer& sweepLayer);
	~CSweepLayer();

public:
	vector<CSweepLine*> m_vpSweepLines;
	vector<CBoundary*> m_vpOffsetBoundaries;
	vector<CPoint3D*> m_vpRoute;
	vector<CSweepPoint*> m_vpTurnRoute;
};

/////////////////////////
//�������
/////////////////////////

//�ƶ��߶�
void AFX_API_EXPORT MoveSegment(CSegment* lsegment, CVector3D vec);

//�����߶ν���
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CSegment seg1, CSegment seg2);
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CPoint3D pt1, CVector3D line_vec, CPoint3D segSp, CPoint3D segEp);
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CSweepLine line1, CSegment line2);

//��һ��Ϊ���ģ������һ��������
void PointToQuad(CPoint3D quad[4], CPoint3D point, double offset, CVector3D coordinate[3]);

//ƫ������
void Offset(CBoundary* boundary, double offset, CVector3D coordinate[]);//����ĳЩ��С���������������յõ���ƫ������������ԭ����������

//�жϵ��Ƿ����߶���
bool IsPointInSeg(CPoint3D pt, CPoint3D seg_ptS, CPoint3D seg_ptE);
