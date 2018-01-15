#include "stdafx.h"
#include "Sweep.h"

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
/*
double SweepPoint::getLength(const SweepPoint* p1, const SweepPoint* p2)
{
	double length = 0;
	length = sqrt((p1->x - p2->x)*(p1->x - p2->x)+ (p1->y - p2->y)*(p1->y - p2->y)+ (p1->z - p2->z)*(p1->z - p2->z));
	return length;
}
*/

Boundary::Boundary()
{
}
Boundary::~Boundary()
{
	int sz = m_Boundary.size();
	for (int i = 0; i < sz; i++)
	{
		delete m_Boundary[i];
		m_Boundary[i] = NULL;
	}
	m_Boundary.clear();
}


CSweep::CSweep():distance(1)
{

}
CSweep::~CSweep()
{
	int szB = m_Boundaries.size();
	for (int i = 0; i < szB; i++)
	{
		delete m_Boundaries[i];
		m_Boundaries[i] = NULL;
	}
	m_Boundaries.clear();

	int szL = m_Sweep_Layers.size();
	for (int i = 0; i < szB; i++)
	{
		delete m_Sweep_Layers[i];
		m_Sweep_Layers[i] = NULL;
	}
	m_Sweep_Layers.clear();

	int szR = m_Routine.size();
	for (int i = 0; i < szB; i++)
	{
		delete m_Routine[i];
		m_Routine[i] = NULL;
	}
	m_Routine.clear();
}

void CSweep::sweep()
{
	int ext[4];
	for (int z = 0; z < m_Sweep_Layers.size(); z++)
	{
		//int height = m_Sweep_Layers[z]->z;

		deletePoints(z);

		rearrange(ext, z);

		findExtreme(ext, z);

		double x_min, x_max, y_min, y_max;	// 轮廓的极限尺寸
		double x, y;
		double dx, dy;
		x_min = m_Boundaries[z]->m_Boundary[ext[2]]->x;
		x_max = m_Boundaries[z]->m_Boundary[ext[3]]->x;
		y_min = m_Boundaries[z]->m_Boundary[ext[0]]->y;
		y_max = m_Boundaries[z]->m_Boundary[ext[1]]->y;

		x = x_min;
		y = y_min;		// 第一条扫描线
		dx = distance;
		dy = distance;		//扫描线间距

		int sz = m_Boundaries[z]->m_Boundary.size();
		for (int k = 0; k < sz; k++)
		{
			m_Boundaries[z]->m_Boundary[k]->p_next = m_Boundaries[z]->m_Boundary[(k + 1) % sz];
			m_Boundaries[z]->m_Boundary[k]->p_prev = m_Boundaries[z]->m_Boundary[(sz + k - 1) % sz];
		}

		if ((z % 2) == 0)
		{
			while (true)
			{
				generateYPoint(ext, y, z);
				if (y >= (y_max - dy))
					break;
				y += dy;
			}
		}
		else
		{
			while (true)
			{
				generateXPoint(ext, x, z);
				if (x >= (x_max - dx))
					break;
				x += dx;
			}
		}

		int szb = m_Boundaries[z]->m_Boundary.size();
		for (int i = 0; i < (szb+1); i++)
		{
			SweepPoint* tmp = NULL;
			tmp = pointToSweeppoint(m_Boundaries[z]->m_Boundary[(ext[1]+i)%szb], true);
			m_Routine.push_back(tmp);
		}
	}
}

void CSweep::generateYPoint(int ext[], double y, int z)
{
	int sz = m_Boundaries[z]->m_Boundary.size();

	SweepPoint* pCurL = NULL;
	SweepPoint* pCurR = NULL;
	CPoint3D* pBeginL = NULL;
	CPoint3D* pEndL = NULL;
	CPoint3D* pBeginR = NULL;
	CPoint3D* pEndR = NULL;

	pBeginL = m_Boundaries[z]->m_Boundary[ext[0]];
	pBeginR = m_Boundaries[z]->m_Boundary[ext[0]];
	
	pEndL = m_Boundaries[z]->m_Boundary[ext[0]]->p_prev;
	pEndR = m_Boundaries[z]->m_Boundary[ext[0]]->p_next;
	if(pEndL != pEndR)
	{
		while (pBeginL->y <y && pEndL->y<y)
		{
			pBeginL = pBeginL->p_prev;
			pEndL = pEndL->p_prev;
		}
		while (pBeginR->y<y && pEndR->y<y)
		{
			pBeginR = pBeginR->p_next;
			pEndR = pEndR->p_next;
		}

		pCurL = getIntersectY(pBeginL, pEndL, y, m_Boundaries[z]->m_Boundary[0]->z, true);
		pCurR = getIntersectY(pBeginR, pEndR, y, m_Boundaries[z]->m_Boundary[0]->z, false);

		int szR = m_Routine.size();
		if ((szR == 0) || (m_Routine[szR - 1]->z == pCurL->z) || (m_Routine[szR - 1]->z == pCurR->z))
		{
			if ((szR == 0) || (m_Routine[szR - 1]->isLeft == true))
			{
				m_Routine.push_back(pCurL);
				m_Routine.push_back(pCurR);
			}
			else
			{
				m_Routine.push_back(pCurR);
				m_Routine.push_back(pCurL);
			}
		}
		else
		{
			m_Routine.push_back(pCurL);
			m_Routine.push_back(pCurR);
		}
	}
}
void CSweep::generateXPoint(int ext[], double x, int z)
{
	int sz = m_Boundaries[z]->m_Boundary.size();

	SweepPoint* pCurL = NULL;
	SweepPoint* pCurR = NULL;
	CPoint3D* pBeginL = NULL;
	CPoint3D* pEndL = NULL;
	CPoint3D* pBeginR = NULL;
	CPoint3D* pEndR = NULL;

	pBeginL = m_Boundaries[z]->m_Boundary[ext[2]];
	pBeginR = m_Boundaries[z]->m_Boundary[ext[2]];

	pEndL = m_Boundaries[z]->m_Boundary[ext[2]]->p_prev;
	pEndR = m_Boundaries[z]->m_Boundary[ext[2]]->p_next;

	if(pEndL != pEndR)
	{
		while (pBeginL->x <x && pEndL->x<x)
		{
			pBeginL = pBeginL->p_prev;
			pEndL = pEndL->p_prev;
		}
		while (pBeginR->x<x && pEndR->x<x)
		{
			pBeginR = pBeginR->p_next;
			pEndR = pEndR->p_next;
		}

		pCurL = getIntersectX(pBeginL, pEndL, x, m_Boundaries[z]->m_Boundary[0]->z, true);
		pCurR = getIntersectX(pBeginR, pEndR, x, m_Boundaries[z]->m_Boundary[0]->z, false);

		int szR = m_Routine.size();
		if ((szR == 0) || (m_Routine[szR - 1]->z == pCurL->z) || (m_Routine[szR - 1]->z == pCurR->z))
		{
			if ((szR == 0) || (m_Routine[szR - 1]->isLeft == true))
			{
				m_Routine.push_back(pCurL);
				m_Routine.push_back(pCurR);
			}
			else
			{
				m_Routine.push_back(pCurR);
				m_Routine.push_back(pCurL);
			}
		}
		else
		{
			m_Routine.push_back(pCurL);
			m_Routine.push_back(pCurR);
		}
	}
}

void CSweep::loadSliceModel(CSlice* slicemodel)
{
	
	int szLay = slicemodel->m_layers.size();
	for (int i = 0; i < szLay; i++)
	{
		m_Sweep_Layers.push_back(slicemodel->m_layers[i]);		//为CSweep对象拷贝一份层切面数据
	}
//	int szLayer = m_Sweep_Layers.size();
	
	for (int i = 0; i < szLay; i++)
	{
		vector<CPoint3D*> tmp_points;
		int szPolyline = m_Sweep_Layers[i]->m_Polylines.size();
		
		for (int j = 0; j < szPolyline; j++)
		{
			int szLinkpoint = m_Sweep_Layers[i]->m_Polylines[j]->m_Linkpoints.size();
			for (int k = 0; k < szLinkpoint - 1; k++)
			{
				tmp_points.push_back(m_Sweep_Layers[i]->m_Polylines[j]->m_Linkpoints[k]);
			}
		}
		Boundary* tmp_boundary = new Boundary();
		tmp_boundary->z = tmp_points[0]->z;
		int szTemp = tmp_points.size();
		for (int j = 0; j < szTemp; j++)
		{
			tmp_boundary->m_Boundary.push_back(tmp_points[j]);
		}
		m_Boundaries.push_back(tmp_boundary);
	}


}

void CSweep::deletePoints(int z)
{
	vector<CPoint3D*> tmp_points;
	int szB = m_Boundaries[z]->m_Boundary.size();

	// 取出单个层切面轮廓中的所有交点
	for (int j = 0; j < szB; j++)
	{
		tmp_points.push_back(m_Boundaries[z]->m_Boundary[j]);
	}
	m_Boundaries[z]->m_Boundary.clear();

	int szTemp = tmp_points.size();
	for (int i = 0; i < szTemp; i++)
	{
		if (tmp_points[i]->x <= 0.0000001)
			tmp_points[i]->x = 0;
		if (tmp_points[i]->y <= 0.0000001)
			tmp_points[i]->y = 0;
		if (tmp_points[i]->z <= 0.0000001)
			tmp_points[i]->z = 0;
	}
	CPoint3D* pCur = NULL;
	CPoint3D* pNext = NULL;
	CPoint3D* pNextNext = NULL;
	CVector3D vec1, vec2; 
	while (szTemp >= 3)
	{
		for (int j = 0; j < szTemp; j++)
		{
			pCur = tmp_points[j%szTemp];
			pNext = tmp_points[(j+1)%szTemp];
			pNextNext = tmp_points[(j + 2)%szTemp];
			//CVector3D* vec1 = new CVector3D(pCur, pNext);
			vec1 = *pNext - *pCur;
			vec2 = *pNextNext - *pCur;

			double angle = 0.0;
			angle = GetAngle(vec1, vec2);
			if(::IsParallel(vec1, vec2))
			//if(angle <ANGLE_RESOLUTION || fabs(angle-PI)<ANGLE_RESOLUTION)
			{
				tmp_points.erase(tmp_points.begin() +(j+1)%szTemp);
				szTemp = tmp_points.size();
				j = -1;
				continue;
			}
		}
		break;
	}
	int num = 0;

	Boundary* tmp_boundary = new Boundary();
	tmp_boundary->z = tmp_points[0]->z;
	for (int i = 0; i < szTemp; i++)
	{
		tmp_boundary->m_Boundary.push_back(tmp_points[i]);
	}
	m_Boundaries[z] = tmp_boundary;
}

void CSweep::rearrange(int ext[], int z)
{
	vector<CPoint3D*> tmp_points;
	for (int j = 0; j < m_Boundaries[z]->m_Boundary.size(); j++)
	{
		tmp_points.push_back(m_Boundaries[z]->m_Boundary[j]);
	}
	m_Boundaries[z]->m_Boundary.clear();

	int szTmp = tmp_points.size();
	CPoint3D* minpoint = tmp_points[0];
	CPoint3D* maxpoint = tmp_points[0];
	ext[0] = 0;
	ext[1] = 0;
	for (int j = 1; j < szTmp; j++)
	{
		if (minpoint->y > tmp_points[j]->y)
		{
			minpoint = tmp_points[j];
			ext[0] = j;
		}
		else if(minpoint->y == tmp_points[j]->y)
		{
			if (minpoint->x > tmp_points[j]->x)
			{
				minpoint = tmp_points[j];
				ext[0] = j;
			}
		}
		else {}
	}
	for (int j = 1; j < szTmp; j++)
	{
		if (maxpoint->y < tmp_points[j]->y)
		{
			maxpoint = tmp_points[j];
			ext[1] = j;
		}
		else if (maxpoint->y == tmp_points[j]->y)
		{
			if (maxpoint->x < tmp_points[j]->x)
			{
				maxpoint = tmp_points[j];
				ext[1] = j;
			}
		}
		else {}
	}

	int num = 0;
	
	Boundary* tmp_boundary = new Boundary();
	tmp_boundary->z = tmp_points[0]->z;
	while (num < szTmp)
	{
		tmp_boundary->m_Boundary.push_back(tmp_points[(ext[0] + num) % szTmp]);
		num++;
	}
	m_Boundaries[z] = tmp_boundary;
}

void CSweep::findExtreme(int ext[], int z)
{
	CPoint3D* minpoint = m_Boundaries[z]->m_Boundary[0];
	CPoint3D* maxpoint = m_Boundaries[z]->m_Boundary[0];
	ext[0] = 0;
	ext[1] = 0;
	ext[2] = 0;
	ext[3] = 0;
	for (int j = 1; j < m_Boundaries[z]->m_Boundary.size(); j++)
	{
		if (minpoint->y > m_Boundaries[z]->m_Boundary[j]->y)
		{
			minpoint = m_Boundaries[z]->m_Boundary[j];
			ext[0] = j;
		}
		else if(minpoint->y == m_Boundaries[z]->m_Boundary[j]->y)
		{
			if (minpoint->x > m_Boundaries[z]->m_Boundary[j]->x)
			{
				minpoint = m_Boundaries[z]->m_Boundary[j];
				ext[0] = j;
			}
		}
		else {}
	}

	for (int j = 1; j < m_Boundaries[z]->m_Boundary.size(); j++)
	{
		if (maxpoint->y < m_Boundaries[z]->m_Boundary[j]->y)
		{
			maxpoint = m_Boundaries[z]->m_Boundary[j];
			ext[1] = j;
		}
		else if (maxpoint->y == m_Boundaries[z]->m_Boundary[j]->y)
		{
			if (maxpoint->x < m_Boundaries[z]->m_Boundary[j]->x)
			{
				maxpoint = m_Boundaries[z]->m_Boundary[j];
				ext[1] = j;
			}
		}
		else {}
	}

	minpoint = m_Boundaries[z]->m_Boundary[0];
	maxpoint = m_Boundaries[z]->m_Boundary[0];
	for (int j = 1; j < m_Boundaries[z]->m_Boundary.size(); j++)
	{
		if (minpoint->x > m_Boundaries[z]->m_Boundary[j]->x)
		{
			minpoint = m_Boundaries[z]->m_Boundary[j];
			ext[2] = j;
		}
		else if (minpoint->x == m_Boundaries[z]->m_Boundary[j]->x)
		{
			if (minpoint->y > m_Boundaries[z]->m_Boundary[j]->y)
			{
				minpoint = m_Boundaries[z]->m_Boundary[j];
				ext[2] = j;
			}
		}
		else {}
	}

	for (int j = 1; j < m_Boundaries[z]->m_Boundary.size(); j++)
	{
		if (maxpoint->x < m_Boundaries[z]->m_Boundary[j]->x)
		{
			maxpoint = m_Boundaries[z]->m_Boundary[j];
			ext[3] = j;
		}
		else if (maxpoint->x == m_Boundaries[z]->m_Boundary[j]->x)
		{
			if (maxpoint->y < m_Boundaries[z]->m_Boundary[j]->y)
			{
				maxpoint = m_Boundaries[z]->m_Boundary[j];
				ext[3] = j;
			}
		}
		else {}
	}
}

SweepPoint* CSweep::getIntersectY(const CPoint3D* p1, const CPoint3D* p2, double y, double z, bool left)
{
	double x = p1->x + (y - p1->y)*(p2->x - p1->x) / (p2->y - p1->y);
	SweepPoint* tmp = new SweepPoint(x, y, z, left);
	return tmp;
}
SweepPoint* CSweep::getIntersectX(const CPoint3D* p1, const CPoint3D* p2, double x, double z, bool left)
{
	double y = p1->y + (x - p1->x)*(p2->y - p1->y) / (p2->x - p1->x);
	SweepPoint* tmp = new SweepPoint(x, y, z, left);
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


SweepPoint* CSweep::pointToSweeppoint(CPoint3D* p, bool left)
{
	SweepPoint* pCur = new SweepPoint();
	pCur->x = p->x;
	pCur->y = p->y;
	pCur->z = p->z;
	pCur->isLeft = left;
	return pCur;
}

void CSweep::writeGCode(CString sFilePath)
{
	CStdioFile fileout;
	if (fileout.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		fileout.WriteString(_T("G28 ;\n"));
		fileout.WriteString(_T("G1 Z15 F2000 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("M82 ;\n"));
		fileout.WriteString(_T("M190 S50 ;\n"));
		fileout.WriteString(_T("M104 T0 S210 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M109 T0 S210 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		CString str;
		str.Format(_T("G0 X%.3f Y%.3f z%.3f\n"),
			m_Routine[0]->x, m_Routine[0]->y, m_Routine[0]->z);
		fileout.WriteString(str);
		int sz = m_Routine.size();
		double length = 0.0000;
		for (int i = 1; i < sz; i++)
		{
			if (m_Routine[i]->z == m_Routine[i - 1]->z)
			{
				length += sqrt((m_Routine[i]->x - m_Routine[i - 1]->x)*(m_Routine[i]->x - m_Routine[i - 1]->x)
					+ (m_Routine[i]->y - m_Routine[i -1]->y)*(m_Routine[i]->y - m_Routine[i - 1]->y) 
					+ (m_Routine[i]->z - m_Routine[i - 1]->z)*(m_Routine[i]->z - m_Routine[i - 1]->z));
				str.Format(_T("G1 X%.3f Y%.3f Z%.3f E%.5f\n"),
					       m_Routine[i]->x, m_Routine[i]->y, m_Routine[i]->z, (length/10));
				fileout.WriteString(str);
			}
			else
			{
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f \n"),
					m_Routine[i]->x, m_Routine[i]->y, m_Routine[i]->z);
				fileout.WriteString(str);
			}
		}
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G91 ;\n"));
		fileout.WriteString(_T("T0 ;\n"));
		fileout.WriteString(_T("G1 E-1 ;\n"));
		fileout.WriteString(_T("M104 T0 S0 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M140 S0 ;\n"));
		fileout.WriteString(_T("M84 ;\n"));
		fileout.Close();
	}
}