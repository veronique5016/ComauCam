#include "stdafx.h"
#include "Layer.h"

LPoint::LPoint()
{
	p_prev = NULL;
	p_next = NULL;
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

LSegment::LSegment()
{
	pstart =new LPoint(0, 0, 0);
	pend =new LPoint(0, 0, 0);
	triangle =new LTriangle();
}
LSegment::LSegment(const LSegment& lsegment)
{
	pstart = lsegment.pstart;
	pend = lsegment.pend;
	triangle = lsegment.triangle;
}
LSegment::LSegment(LPoint* startpoint, LPoint* endpoint, LTriangle* tri)
{
	pstart = startpoint;
	pend = endpoint;
	triangle = tri;
}

LSegment::~LSegment()
{
}

PolyLine::PolyLine()
{
}
PolyLine::~PolyLine()
{
	unsigned int szP = m_Linkpoints.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_Linkpoints[i];
		m_Linkpoints[i] = NULL;
	}
	m_Linkpoints.clear();
}

Layer::Layer()
{
	layer_coordinate[0] = CVector3D(1, 0, 0);
	layer_coordinate[1] = CVector3D(0, 1, 0);
	layer_coordinate[2] = CVector3D(0, 0, 1);
}
Layer::~Layer()
{
	unsigned int szP = m_Polylines.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_Polylines[i];
		m_Polylines[i] = NULL;
	}
	m_Polylines.clear();
}