#include "stdafx.h"
#include "Sweep.h"

SweepPoint::SweepPoint()
{
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
bool SweepLine::isLeft(SweepPoint* p1, SweepPoint* p2)
{
	if (p1->x <= p2->x)
		return true;
	return false;
}

CSweep::CSweep()
{

}
CSweep::~CSweep()
{

}

void CSweep::sweep()
{
	int ext[2];
	rearrange(ext, 0);
	deletePoints(0);
	findExtreme(ext, 0);

	double y_min, y_max;	// 轮廓的极限尺寸
	double y;
	double dy;
	y_min = m_Sweeppoints[ext[0]]->y;	
	y_max = m_Sweeppoints[ext[1]]->y;

	int i = 0;
	int j = 0;

	y = y_min + 0.5;		// 第一条扫描线
	dy = 3;		//扫描线间距


	while (true)
	{
		generatePoint(y, i, j);
		if (y >= (y_max - dy))
			break;
		y += dy;
	}
	int szRoute = m_Routine.size();

}

void CSweep::loadSliceModel(CSlice* slicemodel)
{
	int szLay = slicemodel->m_layers.size();
	for (int i = 0; i < szLay; i++)
	{
		m_Sweep_Layers.push_back(slicemodel->m_layers[i]);		//为CSweep对象拷贝一份层切面数据
	}
}

void CSweep::deletePoints(int i)
{
	vector<CPoint3D*> tmp_points;
	for (int j = 0; j < m_Sweeppoints.size(); j++)
	{
		tmp_points.push_back(m_Sweeppoints[j]);
	}
	m_Sweeppoints.clear();

	while (tmp_points.size() >= 3)
	{
		for (int j = 0; j < tmp_points.size()-1; j++)
		{
			CPoint3D* pCur = tmp_points[j];
			CPoint3D* pNext = tmp_points[j+1];
			CPoint3D* pNextNext = tmp_points[(j + 2)%tmp_points.size()];
			CVector3D* vec1 = new CVector3D(pCur, pNext);
			CVector3D* vec2 = new CVector3D(pCur, pNextNext);
			if (::IsParallel(vec1, vec2))
			{
				tmp_points.erase(tmp_points.begin() + j + 1);
				continue;
			}
		}
		break;
	}
	for (int j = 0; j < tmp_points.size(); j++)
	{
		m_Sweeppoints.push_back(tmp_points[j]);
	}
}

void CSweep::rearrange(int ext[], int i)
{
	vector<CPoint3D*> tmp_points;
	int szPolyline = m_Sweep_Layers[i]->m_Polylines.size();

	// 取出单个层切面轮廓中的所有交点
	for (int j = 0; j < szPolyline; j++)
	{
		int szLinkpoint = m_Sweep_Layers[i]->m_Polylines[j]->m_Linkpoints.size();
		for (int k = 0; k < szLinkpoint - 1; k++)
		{
			tmp_points.push_back(m_Sweep_Layers[i]->m_Polylines[j]->m_Linkpoints[k]);
		}
	}

	int szSweeppoints = tmp_points.size();
	CPoint3D* minpoint = tmp_points[0];
	CPoint3D* maxpoint = tmp_points[0];
	ext[0] = 0;
	ext[1] = 0;
	for (int j = 1; j < szSweeppoints; j++)
	{
		if (minpoint->x <= tmp_points[j]->x &&minpoint->y <= tmp_points[j]->y)
			continue;
		else
		{
			minpoint = tmp_points[j];
			ext[0] = j;
		}
	}
	for (int j = 1; j < szSweeppoints; j++)
	{
		if (maxpoint->x >= tmp_points[j]->x &&maxpoint->y >= tmp_points[j]->y)
			continue;
		else
		{
			maxpoint = tmp_points[j];
			ext[1] = j;
		}
	}

	int num = 0;
	while (num < tmp_points.size())
	{
		m_Sweeppoints.push_back(tmp_points[(ext[0] + num)%tmp_points.size()]);
		num++;
	}

}

void CSweep::findExtreme(int ext[], int i)
{
	CPoint3D* minpoint = m_Sweeppoints[0];
	CPoint3D* maxpoint = m_Sweeppoints[0];
	ext[0] = 0;
	ext[1] = 0;
	for (int j = 1; j < m_Sweeppoints.size(); j++)
	{
		if (minpoint->x <= m_Sweeppoints[j]->x &&minpoint->y <= m_Sweeppoints[j]->y)
			continue;
		else
		{
			minpoint = m_Sweeppoints[j];
			ext[0] = j;
		}
	}
	for (int j = 1; j < m_Sweeppoints.size(); j++)
	{
		if (maxpoint->x >= m_Sweeppoints[j]->x &&maxpoint->y >= m_Sweeppoints[j]->y)
			continue;
		else
		{
			maxpoint = m_Sweeppoints[j];
			ext[1] = j;
		}
	}
}

void CSweep::generatePoint(double y, int i, int j)
{
	int sz = m_Sweeppoints.size();

	CPoint3D* pCur = NULL;
	CPoint3D* pBeginL = NULL;
	CPoint3D* pEndL = NULL;
	CPoint3D* pBeginR = NULL;
	CPoint3D* pEndR = NULL;
	while((j+1)!=(sz-i-1))
	{
		pBeginL = m_Sweeppoints[i];
		pEndL = m_Sweeppoints[sz - i - 1];

		pBeginR = m_Sweeppoints[j];
		pEndR = m_Sweeppoints[j+1];
		if (pBeginL->y <y && pEndL->y<y)
		{
			pBeginL = m_Sweeppoints[sz - i - 1];
			pEndL = m_Sweeppoints[sz - i - 2];
		}
		if (pBeginR->y<y && pEndR->y<y)
		{
			pBeginR = m_Sweeppoints[j+1];
			pEndR = m_Sweeppoints[j+2];
		}

		pCur = getIntersect(pBeginL, pEndL, y, m_Sweeppoints[0]->z);
		m_Routine.push_back(pCur);
		pCur = getIntersect(pBeginR, pEndR, y, m_Sweeppoints[0]->z);
		m_Routine.push_back(pCur);

		if (pEndR->y <= y)
			j++;
		if (pEndL->y <= y)
			i++;
		break;
	}
}

CPoint3D* CSweep::getIntersect(const CPoint3D* p1, const CPoint3D* p2, double y, double z)
{
	double x = p1->x + (y - p1->y)*(p2->x - p1->x) / (p2->y - p1->y);
	CPoint3D* tmp = new CPoint3D(x, y, z);
	return tmp;
}

void CSweep::drawRoute()
{
	int sz = m_Routine.size();
	for (int i = 0; i < sz-1; i++)
	{
		CPoint3D* p1 = new CPoint3D(*m_Routine[i]);
		CPoint3D* p2 = new CPoint3D(*m_Routine[i+1]);
		glLineWidth(1.5f);
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(p1->x, p1->y, p1->z);
		glVertex3f(p2->x, p2->y, p2->z);
		glEnd();
	}
}