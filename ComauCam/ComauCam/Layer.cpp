#include "stdafx.h"
#include "Layer.h"

LPoint::LPoint()
{
	//p_prev = NULL;
	//p_next = NULL;
}
LPoint::~LPoint()
{
}
LPoint::LPoint(const LPoint& lpoint)
{
	x = lpoint.x;
	y = lpoint.y;
	z = lpoint.z;
}
LPoint::LPoint(const CPoint3D & pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
}
LPoint::LPoint(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

LPoint LPoint::operator+=(const CVector3D & v)
{
	return LPoint(x + v.dx, y + v.dy, z + v.dz);
}

Segment::Segment()
{
	pstart = LPoint(0, 0, 0);
	pend = LPoint(0, 0, 0);
	triangle =new LTriangle();
	segment_vec = CVector3D(0, 0, 0);
}
Segment::Segment(const Segment& lsegment)
{
	pstart = lsegment.pstart;
	pend = lsegment.pend;
	triangle = lsegment.triangle;
	segment_vec = lsegment.segment_vec;
}
Segment::Segment(LPoint startpoint, LPoint endpoint, LTriangle* tri)
{
	pstart = startpoint;
	pend = endpoint;
	triangle = tri;
	CPoint3D p1 = CPoint3D(startpoint.x, startpoint.y, startpoint.z);
	CPoint3D p2 = CPoint3D(endpoint.x, endpoint.y, endpoint.z);
	segment_vec = CVector3D(p1, p2);
	segment_vec.Normalize();
}

Segment::~Segment()
{
}

Boundary::Boundary()
{
}
Boundary::~Boundary()
{
	unsigned int szP = m_segments.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_segments[i];
		m_segments[i] = NULL;
	}
	m_segments.clear();
}

SliceLayer::SliceLayer()
{
	layer_coordinate[0] = CVector3D(1, 0, 0);
	layer_coordinate[1] = CVector3D(0, 1, 0);
	layer_coordinate[2] = CVector3D(0, 0, 1);
}
SliceLayer::~SliceLayer()
{
	unsigned int szP = m_Boundaries.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_Boundaries[i];
		m_Boundaries[i] = NULL;
	}
	m_Boundaries.clear();
}

SweepPoint::SweepPoint()
{
}
SweepPoint::SweepPoint(double ix, double iy, double iz, bool left)
{
	x = ix;
	y = iy;
	z = iz;
	isLeft = left;
}
SweepPoint::~SweepPoint()
{
}

SweepLine::SweepLine()
{
}
SweepLine::~SweepLine()
{
}

/*
CLine::CLine()
{
}
CLine::~CLine()
{
}*/

/*
Boundary::Boundary()
{
}
Boundary::~Boundary()
{
	unsigned int sz = m_Boundary.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		delete m_Boundary[i];
		m_Boundary[i] = NULL;
	}
	m_Boundary.clear();
}
*/

SweepLayer::SweepLayer()
{
}
SweepLayer::SweepLayer(const SliceLayer & sliceLayer)
{
	layerPoint = sliceLayer.layerPoint;
	layer_coordinate[0] = sliceLayer.layer_coordinate[0];
	layer_coordinate[1] = sliceLayer.layer_coordinate[1];
	layer_coordinate[2] = sliceLayer.layer_coordinate[2];
	unsigned int sz = sliceLayer.m_Boundaries.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		m_Boundaries.push_back(sliceLayer.m_Boundaries[i]);
	}
}
SweepLayer::~SweepLayer()
{
	unsigned int szB = offsetBoundaries.size();
	for (unsigned int i = 0; i < szB; i++)
	{
		delete offsetBoundaries[i];
		offsetBoundaries[i] = NULL;
	}
	offsetBoundaries.clear();

	unsigned int szR = m_Route.size();
	for (unsigned int i = 0; i < szR; i++)
	{
		delete m_Route[i];
		m_Route[i] = NULL;
	}
	m_Route.clear();

	unsigned int szL = m_sweeplines.size();
	for (unsigned int i = 0; i < szR; i++)
	{
		delete m_sweeplines[i];
		m_sweeplines[i] = NULL;
	}
	m_sweeplines.clear();
}
