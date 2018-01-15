#include "stdafx.h"
#include "Polyline.h"

#define LARGE            9999999999999999999.9
CPolyline::CPolyline(void)
{
}

CPolyline::~CPolyline(void)
{
}

void CPolyline::AddPt(CPoint3D* pPt)
{
	m_pts.Add(pPt);
}

void CPolyline::AddPt(const CPoint3D& pt)
{
	m_pts.Add(new CPoint3D(pt));
}

void CPolyline::DelColinearPt(double angle_resolution)
{
	CVector3D vec1, vec2;
	double angle = 0.0;

	int sz0 = m_pts.GetSize();

	delete m_pts[sz0 - 1];
	m_pts.RemoveAt(sz0 - 1);

	int prev_i, next_i, i = 0, cnt = 0, pos = 0, sz;
	while (true)
	{
		cnt++;

		sz = m_pts.GetSize();

		if (0 == sz)
		{
			return;
		}

		i = O(i, sz);
		prev_i = O(i - 1, sz);
		next_i = O(i + 1, sz);

		if (*m_pts[prev_i] ^= *m_pts[i])  // 判断两个点是否距离很近，同时可以删除重复点
		{
			delete m_pts[prev_i];
			m_pts.RemoveAt(prev_i);
			continue;
		}

		vec1 = *m_pts[prev_i] - *m_pts[i];
		vec2 = *m_pts[next_i] - *m_pts[i];
		angle = GetAngle(vec1, vec2);
		if (angle < angle_resolution || fabs(angle - PI) < angle_resolution)
		{
			delete m_pts[i];
			m_pts.RemoveAt(i);

			pos = cnt;
		}
		else
		{
			i++;

			if (cnt - pos >= 2 * m_pts.GetSize())
			{
				break;
			}
		}
	}
	AddPt(GetFirstPt());
}

CPoint3D CPolyline::GetFirstPt() const
{
	return *m_pts[0];
}

CPoint3D CPolyline::GetLastPt() const
{
	int sz = m_pts.GetSize();
	return *m_pts[sz - 1];
}

int CPolyline::O(int i, int sz)
{
	if (i<0)
	{
		int n = -i / sz + 1;
		i = i + n*sz;
	}
	return i%sz;
}

CPolyline::CPolyline(int n, CPoint3D pt, ...)
{
	Init();

	CPoint3D* p = &pt;
	int sz = sizeof(CPoint3D);
	for (int i = 0; i<n; i++)
	{
		AddPt(*p);
		p += 1;
	}
}

void CPolyline::Init()
{

}
double CPolyline::O(const double &i, const int &nOffset, const int &sz)
{
	double delta = i - floor(i);
	int j = (int)i;

	if (j >= nOffset && j <= sz - 1)
		return double(j - nOffset) + delta;
	else
		return double(sz - 1 - nOffset + j) + delta;
}

void CPolyline::DelCoincidePt(double dist_resolution /* = POINT_RESOLUTION */)
{
	int i = 0;
	CVector3D vec;
	double dist2;
	while (i<m_pts.GetSize() - 1)
	{
		vec = *m_pts[i] - *m_pts[i + 1];
		dist2 = vec.GetMod2();
		if (dist2 < dist_resolution)
		{
			delete m_pts[i + 1];
			m_pts.RemoveAt(i + 1);
		}
		else
		{
			i++;
		}
	}
}

void CPolyline::MakePolygonCCW()
{
	if (!IsPolygonCCW())
	{
		int sz = m_pts.GetSize();
		CPoint3D* pt = new CPoint3D[sz];
		for (int i = 0; i<sz; i++)
		{
			pt[i] = *m_pts[sz - 1 - i];
		}
		for (int i = 0; i<sz; i++)
		{
			*m_pts[i] = pt[i];
		}
		delete[]pt;
	}
}
BOOL CPolyline::IsPolygonCCW()
{
	int sz = m_pts.GetSize();

	if (sz <= 3)      // 一条线的时候（3个点）认为是顺时针的
	{
		return FALSE;
	}

	int ibtm = 0;
	double yMin = m_pts[0]->y;
	for (int j = 0; j<sz; j++)
	{
		if (m_pts[j]->y<yMin)
		{
			ibtm = j;
			yMin = m_pts[j]->y;
		}
	}

	CVector3D vec1;
	CVector3D vec2;
	double r = 0;
	while (TRUE)
	{
		if (sz - 1 == ibtm || 0 == ibtm)
		{
			vec1 = *m_pts[0] - *m_pts[sz - 2];
			vec2 = *m_pts[1] - *m_pts[0];
		}
		else
		{
			vec1 = *m_pts[ibtm] - *m_pts[ibtm - 1];
			vec2 = *m_pts[ibtm + 1] - *m_pts[ibtm];
		}

		r = (vec1*vec2).dz;
		if (0 == r)
		{
			ibtm++;
			if (sz == ibtm)
			{
				ibtm = 1;
			}
			continue;
		}
		else
		{
			if (r>0)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
}
void CPolyline::MakePolygonCW()
{
	if (IsPolygonCCW())
	{
		int sz = m_pts.GetSize();
		CPoint3D* pt = new CPoint3D[sz];
		for (int i = 0; i<sz; i++)
		{
			pt[i] = *m_pts[sz - 1 - i];
		}
		for (int i = 0; i<sz; i++)
		{
			*m_pts[i] = pt[i];
		}
		delete[]pt;
	}
}

bool CPolyline::IsPtInPolygon(const CPoint3D& pt) const
{
	if (IsPtOnPolygon(pt))
	{
		return false;
	}

	CPoint3D ptFaraway(LARGE, pt.y, pt.z);
	int countCrossPoint = 0;

	int sz = m_pts.GetSize();
	for (int i = 0; i <= sz - 2; i++)
	{
		if ((m_pts[i]->y - pt.y)*(m_pts[i + 1]->y - pt.y)<0)
		{
			if (Is2SegsCross(pt, ptFaraway, *m_pts[i], *m_pts[i + 1]))
			{
				countCrossPoint++;
			}
		}
	}

	double prev_y, next_y;
	for (int i = 0; i <= sz - 2; i++)
	{
		if (m_pts[i]->y == pt.y && m_pts[i]->x > pt.x)
		{
			if (0 == i)
			{
				prev_y = m_pts[sz - 2]->y;
				next_y = m_pts[1]->y;
			}
			else
			{
				prev_y = m_pts[i - 1]->y;
				next_y = m_pts[i + 1]->y;
			}

			if ((prev_y - pt.y)*(next_y - pt.y)<0)
			{
				countCrossPoint++;
			}
		}
	}

	if (1 == countCrossPoint % 2)
		return true;
	else
		return false;
}
bool CPolyline::IsPtOnPolygon(const CPoint3D& pt) const
{
	CVector3D v1, v2;
	CVector3D zero_vector(0, 0, 0);
	int sz = m_pts.GetSize();
	for (int i = 0; i<sz - 1; i++)
	{
		v1 = *m_pts[i] - pt;
		v2 = *m_pts[i + 1] - pt;

		if (v1*v2 == zero_vector)
		{
			return true;
		}
	}
	return false;
}
void CPolyline::Rotate(const CMatrix3D& m)
{
	int sz = m_pts.GetSize();
	for (int i = 0; i<sz; i++)
	{
		(*m_pts[i]) *= m;
	}
}

void CPolyline::Draw(COpenGLDC* pDC, bool ShowTri)
{
	int sz = m_pts.GetSize();
/*
	COLORREF oldClr;
	if (GetIsSelected())
		oldClr = pDC->SetMaterialColor(CLR_SELECTED);
	else
		oldClr = pDC->SetMaterialColor(m_clr);
*/
	if (1 == sz)
	{
		pDC->DrawPoint(*m_pts[0]);
		return;
	}
	for (int i = 0; i<sz - 1; i++)
	{
		pDC->DrawLine(*m_pts[i], *m_pts[i + 1], 0, 1);    // 最后一个参数先暂定 1

		if (m_bIsDrawPt)
			pDC->DrawSelPoint(*m_pts[i]);
	}

	if (m_bIsDrawPt)
	{
		pDC->DrawSelPoint(GetFirstPt(), ST_CIRCLE);
		pDC->DrawSelPoint(*m_pts[1], ST_TRIANGLE);
	}
//	pDC->SetMaterialColor(oldClr);
}