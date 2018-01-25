#pragma once
#include "GeomBase.h"
#include "LVertex.h"
#include <vector>

using namespace std;

//扫描路径与轮廓相交得到的交点
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

//切片平面
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
	vector<CBoundary*> m_vpBoundaries;   //截交得到的轮廓，不经修改 size = 1, 如果轮廓内部有空洞或其他情况，则 size 会增加
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
//输出函数
/////////////////////////

//移动线段
void AFX_API_EXPORT MoveSegment(CSegment* lsegment, CVector3D vec);

//求两线段交点
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CSegment seg1, CSegment seg2);
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CPoint3D pt1, CVector3D line_vec, CPoint3D segSp, CPoint3D segEp);
void AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, CSweepLine line1, CSegment line2);

//以一点为中心，扩充成一个正方形
void PointToQuad(CPoint3D quad[4], CPoint3D point, double offset, CVector3D coordinate[3]);

//偏移轮廓
void Offset(CBoundary* boundary, double offset, CVector3D coordinate[]);//由于某些很小的向量分量，最终得到的偏置轮廓可能与原轮廓不共面

//判断点是否在线段内
bool IsPointInSeg(CPoint3D pt, CPoint3D seg_ptS, CPoint3D seg_ptE);
