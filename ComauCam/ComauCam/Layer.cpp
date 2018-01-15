#include "stdafx.h"
#include "Layer.h"

CLPoint::CLPoint()
{
}
CLPoint::~CLPoint()
{
}
CLPoint::CLPoint(const CLPoint& lpoint)
{
	x = lpoint.x;
	y = lpoint.y;
	z = lpoint.z;
}
CLPoint::CLPoint(const CPoint3D & pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
}
CLPoint::CLPoint(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

CLPoint CLPoint::operator+=(const CVector3D & v)
{
	return CLPoint(x + v.dx, y + v.dy, z + v.dz);
}

CSegment::CSegment()
{
	m_ptStart = CLPoint(0, 0, 0);
	m_ptEnd = CLPoint(0, 0, 0);
	m_pTriangle =new CLTriangle();
	m_vSegmentVec = CVector3D(0, 0, 0);
}
CSegment::CSegment(const CSegment& lsegment)
{
	m_ptStart = lsegment.m_ptStart;
	m_ptEnd = lsegment.m_ptEnd;
	m_pTriangle = lsegment.m_pTriangle;
	m_vSegmentVec = lsegment.m_vSegmentVec;
}
CSegment::CSegment(CLPoint startpoint, CLPoint endpoint, CLTriangle* tri)
{
	m_ptStart = startpoint;
	m_ptEnd = endpoint;
	m_pTriangle = tri;
	CPoint3D p1 = CPoint3D(startpoint.x, startpoint.y, startpoint.z);
	CPoint3D p2 = CPoint3D(endpoint.x, endpoint.y, endpoint.z);
	m_vSegmentVec = CVector3D(p1, p2);
	m_vSegmentVec.Normalize();
}

CSegment::~CSegment()
{
}

CBoundary::CBoundary()
{
}
CBoundary::~CBoundary()
{
	unsigned int szP = m_vecpSegments.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_vecpSegments[i];
		m_vecpSegments[i] = NULL;
	}
	m_vecpSegments.clear();
}

CSliceLayer::CSliceLayer()
{
	m_vLayerCoordinate[0] = CVector3D(1, 0, 0);
	m_vLayerCoordinate[1] = CVector3D(0, 1, 0);
	m_vLayerCoordinate[2] = CVector3D(0, 0, 1);
}
CSliceLayer::~CSliceLayer()
{
	unsigned int szP = m_vecpBoundaries.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_vecpBoundaries[i];
		m_vecpBoundaries[i] = NULL;
	}
	m_vecpBoundaries.clear();
}

CSweepPoint::CSweepPoint()
{
}
CSweepPoint::CSweepPoint(double ix, double iy, double iz, bool left)
{
	x = ix;
	y = iy;
	z = iz;
}
CSweepPoint::~CSweepPoint()
{
}

CSweepLine::CSweepLine()
{
}
CSweepLine::~CSweepLine()
{
}

CSweepLayer::CSweepLayer()
{
}
CSweepLayer::CSweepLayer(const CSliceLayer & sliceLayer)
{
	m_ptLayerPoint = sliceLayer.m_ptLayerPoint;
	m_vLayerCoordinate[0] = sliceLayer.m_vLayerCoordinate[0];
	m_vLayerCoordinate[1] = sliceLayer.m_vLayerCoordinate[1];
	m_vLayerCoordinate[2] = sliceLayer.m_vLayerCoordinate[2];
	unsigned int sz = sliceLayer.m_vecpBoundaries.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		m_vecpBoundaries.push_back(sliceLayer.m_vecpBoundaries[i]);
	}
}
CSweepLayer::CSweepLayer(const CSweepLayer & sweepLayer)
{
	m_ptLayerPoint = sweepLayer.m_ptLayerPoint;
	m_vLayerCoordinate[0] = sweepLayer.m_vLayerCoordinate[0];
	m_vLayerCoordinate[1] = sweepLayer.m_vLayerCoordinate[1];
	m_vLayerCoordinate[2] = sweepLayer.m_vLayerCoordinate[2];
	unsigned int sz = sweepLayer.m_vecpBoundaries.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		m_vecpBoundaries.push_back(sweepLayer.m_vecpBoundaries[i]);
	}

	unsigned int szL = sweepLayer.m_vecpSweepLines.size();
	for (unsigned int i = 0; i < szL; i++)
	{
		m_vecpSweepLines.push_back(sweepLayer.m_vecpSweepLines[i]);
	}

	unsigned int szP = sweepLayer.m_vecpRoute.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		m_vecpRoute.push_back(new CPoint3D(*sweepLayer.m_vecpRoute[i]));
	}

	unsigned int szT = sweepLayer.m_vecpTurnRoute.size();
	for (unsigned int i = 0; i < szT; i++)
	{
		m_vecpTurnRoute.push_back(new CFPoint(*sweepLayer.m_vecpTurnRoute[i]));
	}
}
CSweepLayer::~CSweepLayer()
{
	unsigned int szB = m_vecpOffsetBoundaries.size();
	for (unsigned int i = 0; i < szB; i++)
	{
		delete m_vecpOffsetBoundaries[i];
		m_vecpOffsetBoundaries[i] = NULL;
	}
	m_vecpOffsetBoundaries.clear();

	unsigned int szR = m_vecpRoute.size();
	for (unsigned int i = 0; i < szR; i++)
	{
		delete m_vecpRoute[i];
		m_vecpRoute[i] = NULL;
	}
	m_vecpRoute.clear();

	unsigned int szL = m_vecpSweepLines.size();
	for (unsigned int i = 0; i < szR; i++)
	{
		delete m_vecpSweepLines[i];
		m_vecpSweepLines[i] = NULL;
	}
	m_vecpSweepLines.clear();

	unsigned int szT = m_vecpTurnRoute.size();
	for (unsigned int i = 0; i < szT; i++)
	{
		delete m_vecpTurnRoute[i];
		m_vecpTurnRoute[i] = NULL;
	}
	m_vecpTurnRoute.clear();
}

CFPoint::CFPoint()
{
}

CFPoint::~CFPoint()
{
}

CFPoint::CFPoint(const CPoint3D & pt, double a, double c)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
	A = a;
	C = c;
}

CFPoint::CFPoint(const CPoint3D & pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
	A = 0;
	C = 0;
}

CFPoint::CFPoint(const CFPoint & pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
	A = pt.A;
	C = pt.C;
}
