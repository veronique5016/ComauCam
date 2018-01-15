#include "stdafx.h"
#include "Line.h"


CLine::CLine(void)
{
}


CLine::~CLine(void)
{
}
void CLine::Rotate(const CMatrix3D& m)
{
	m_pt1 *= m;
	m_pt2 *= m;
}
CLine::CLine(const CLine& line)
{
	m_pt1 = line.m_pt1;
	m_pt2 = line.m_pt2;
	m_length = line.m_length;
}

CLine::CLine(const CPoint3D& pt1, const CPoint3D& pt2)
{
	m_pt1 = pt1;
	m_pt2 = pt2;
}

CLine::CLine(const CPoint3D& pt1, const CPoint3D& pt2, double length)
{
	m_pt1 = pt1;
	m_pt2 = pt2;
	m_length = length;
}
