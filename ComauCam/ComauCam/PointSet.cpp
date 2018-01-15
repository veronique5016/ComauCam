#include "stdafx.h"
#include "PointSet.h"


CPointSet::CPointSet(void)
{
}


CPointSet::~CPointSet(void)
{
	int sz = m_pts.GetSize();
	for (int i = sz - 1; i >= 0; i--)
	{
		delete m_pts[i];
		m_pts[i] = NULL;
	}
	m_pts.RemoveAll();
}

double CPointSet::FindExtreme(int nWhichDir) const
{
	int sz = m_pts.GetSize();
	if (0 == sz)
	{
		AfxMessageBox(_T("There is no point in the point-cloud. Check it again."));
		return -1;
	}

	int i;
	double dExt;

	switch (nWhichDir)
	{
	case MIN_X:
		dExt = m_pts[0]->x;
		for (i = 1; i<sz; i++)
		{
			if (m_pts[i]->x < dExt)
			{
				dExt = m_pts[i]->x;
			}
		}
		break;

	case MAX_X:
		dExt = m_pts[0]->x;
		for (i = 1; i<sz; i++)
		{
			if (m_pts[i]->x > dExt)
			{
				dExt = m_pts[i]->x;
			}
		}
		break;

	case MIN_Y:
		dExt = m_pts[0]->y;
		for (i = 1; i<sz; i++)
		{
			if (m_pts[i]->y < dExt)
			{
				dExt = m_pts[i]->y;
			}
		}
		break;

	case MAX_Y:
		dExt = m_pts[0]->y;
		for (i = 1; i<sz; i++)
		{
			if (m_pts[i]->y > dExt)
			{
				dExt = m_pts[i]->y;
			}
		}
		break;

	case MIN_Z:
		dExt = m_pts[0]->z;
		for (i = 1; i<sz; i++)
		{
			if (m_pts[i]->z < dExt)
			{
				dExt = m_pts[i]->z;
			}
		}
		break;

	case MAX_Z:
		dExt = m_pts[0]->z;
		for (i = 1; i<sz; i++)
		{
			if (m_pts[i]->z > dExt)
			{
				dExt = m_pts[i]->z;
			}
		}
		break;
	}
	return dExt;
}