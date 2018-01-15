#pragma once
#include "GeomBase.h"
class CLine
{
public:
	CLine(void);
	~CLine(void);
	CLine(const CLine& line);
	CLine(const CPoint3D& pt1, const CPoint3D& pt2);
	CLine(const CPoint3D& pt1, const CPoint3D& pt2, double length);
	CPoint3D m_pt1;
	CPoint3D m_pt2;
	double   m_length;
	virtual void Rotate(const CMatrix3D& m);
};

