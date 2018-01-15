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
double SweepPoint::getLength(const SweepPoint* p1, const SweepPoint* p2)
{
	double length = 0;
	length = sqrt((p1->x - p2->x)*(p1->x - p2->x)+ (p1->y - p2->y)*(p1->y - p2->y)+ (p1->z - p2->z)*(p1->z - p2->z));
	return length;
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


Boundary::Boundary()
{
}
Boundary::~Boundary()
{
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
	for (int z = 0; z < m_Sweep_Layers.size(); z++)
	{
		//int height = m_Sweep_Layers[z]->z;
		rearrange(ext, z);
		deletePoints(z);
		findExtreme(ext, z);

		double y_min, y_max;	// 轮廓的极限尺寸
		double y;
		double dy;
		y_min = m_Boundaries[z]->m_boundary[ext[0]]->y;
		y_max = m_Boundaries[z]->m_boundary[ext[1]]->y;

		int i = 0;
		int j = 0;

		y = y_min + 0.5;		// 第一条扫描线
		dy = 1;		//扫描线间距


		while (true)
		{
			generatePoint(y, i, j, z);
			if (y >= (y_max - dy))
				break;
			y += dy;
		}
		int szb = m_Boundaries[z]->m_boundary.size();
		for (int i = 0; i < (szb+1); i++)
		{
			SweepPoint* tmp = NULL;
			tmp = pointToSweeppoint(m_Boundaries[z]->m_boundary[(ext[1]+i)%szb], true);
			m_Routine.push_back(tmp);
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
}

void CSweep::deletePoints(int z)
{
	vector<CPoint3D*> tmp_points;
	for (int j = 0; j < m_Boundaries[z]->m_boundary.size(); j++)
	{
		tmp_points.push_back(m_Boundaries[z]->m_boundary[j]);
	}
	m_Boundaries[z]->m_boundary.clear();

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
		m_Boundaries[z]->m_boundary.push_back(tmp_points[j]);
	}
}

void CSweep::rearrange(int ext[], int z)
{
	vector<CPoint3D*> tmp_points;
	int szPolyline = m_Sweep_Layers[z]->m_Polylines.size();

	// 取出单个层切面轮廓中的所有交点
	for (int j = 0; j < szPolyline; j++)
	{
		int szLinkpoint = m_Sweep_Layers[z]->m_Polylines[j]->m_Linkpoints.size();
		for (int k = 0; k < szLinkpoint - 1; k++)
		{
			tmp_points.push_back(m_Sweep_Layers[z]->m_Polylines[j]->m_Linkpoints[k]);
		}
	}

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
	
	Boundary* tmp_boundary = new Boundary();
	tmp_boundary->z = z;
	while (num < tmp_points.size())
	{
		tmp_boundary->m_boundary.push_back(tmp_points[(ext[0] + num) % tmp_points.size()]);		
		//m_Boundaries[z]->m_boundary.push_back(tmp_points[(ext[0] + num)%tmp_points.size()]);
		num++;
	}
	m_Boundaries.push_back(tmp_boundary);
}

void CSweep::findExtreme(int ext[], int z)
{
	CPoint3D* minpoint = m_Boundaries[z]->m_boundary[0];
	CPoint3D* maxpoint = m_Boundaries[z]->m_boundary[0];
	ext[0] = 0;
	ext[1] = 0;
	for (int j = 1; j < m_Boundaries[z]->m_boundary.size(); j++)
	{
		if (minpoint->x <= m_Boundaries[z]->m_boundary[j]->x &&minpoint->y <= m_Boundaries[z]->m_boundary[j]->y)
			continue;
		else
		{
			minpoint = m_Boundaries[z]->m_boundary[j];
			ext[0] = j;
		}
	}
	for (int j = 1; j < m_Boundaries[z]->m_boundary.size(); j++)
	{
		if (maxpoint->x >= m_Boundaries[z]->m_boundary[j]->x &&maxpoint->y >= m_Boundaries[z]->m_boundary[j]->y)
			continue;
		else
		{
			maxpoint = m_Boundaries[z]->m_boundary[j];
			ext[1] = j;
		}
	}
}

void CSweep::generatePoint(double y, int i, int j, int z)
{
	int sz = m_Boundaries[z]->m_boundary.size();

	SweepPoint* pCurL = NULL;
	SweepPoint* pCurR = NULL;
	CPoint3D* pBeginL = NULL;
	CPoint3D* pEndL = NULL;
	CPoint3D* pBeginR = NULL;
	CPoint3D* pEndR = NULL;
	while((j+1)!=(sz-i-1))
	{
		pBeginL = m_Boundaries[z]->m_boundary[i];
		pEndL = m_Boundaries[z]->m_boundary[sz - i - 1];

		pBeginR = m_Boundaries[z]->m_boundary[j];
		pEndR = m_Boundaries[z]->m_boundary[j+1];
		if (pBeginL->y <y && pEndL->y<y)
		{
			pBeginL = m_Boundaries[z]->m_boundary[sz - i - 1];
			pEndL = m_Boundaries[z]->m_boundary[sz - i - 2];
		}
		if (pBeginR->y<y && pEndR->y<y)
		{
			pBeginR = m_Boundaries[z]->m_boundary[j+1];
			pEndR = m_Boundaries[z]->m_boundary[j+2];
		}

		pCurL = getIntersect(pBeginL, pEndL, y, m_Boundaries[z]->m_boundary[0]->z, true);
		
		pCurR = getIntersect(pBeginR, pEndR, y, m_Boundaries[z]->m_boundary[0]->z, false);
		
		int szR = m_Routine.size();
		if ((szR==0) || (m_Routine[szR - 1]->z == pCurL->z) || (m_Routine[szR - 1]->z == pCurR->z))
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
		if (pEndR->y <= y)
			j++;
		if (pEndL->y <= y)
			i++;
		break;
		
	}
}

SweepPoint* CSweep::getIntersect(const CPoint3D* p1, const CPoint3D* p2, double y, double z, bool left)
{
	double x = p1->x + (y - p1->y)*(p2->x - p1->x) / (p2->y - p1->y);
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