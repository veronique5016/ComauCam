#include "stdafx.h"
#include "Sweep.h"



CSweep::CSweep():distance(10)
{

}
CSweep::~CSweep()
{
	unsigned int szB = m_Boundaries.size();
	for (unsigned int i = 0; i < szB; i++)
	{
		delete m_Boundaries[i];
		m_Boundaries[i] = NULL;
	}
	m_Boundaries.clear();

	unsigned int szL = m_slice_layers.size();
	for (unsigned int i = 0; i < szB; i++)
	{
		delete m_slice_layers[i];
		m_slice_layers[i] = NULL;
	}
	m_slice_layers.clear();

	unsigned int szR = m_Routine.size();
	for (unsigned int i = 0; i < szB; i++)
	{
		delete m_Routine[i];
		m_Routine[i] = NULL;
	}
	m_Routine.clear();

	unsigned int szW = m_sweep_layers.size();
	for (unsigned int i = 0; i < szB; i++)
	{
		delete m_sweep_layers[i];
		m_sweep_layers[i] = NULL;
	}
	m_sweep_layers.clear();
}


void CSweep::sweep()
{
	int ext[4];
	double x_min, x_max, x, dx;
	dx = distance;
	for (unsigned int i = 0; i < m_sweep_layers.size(); i++)
	{
		offSet(m_sweep_layers[i], 1.0);

		//寻找轮廓 x 方向的极值
		unsigned int sz = m_sweep_layers[i]->m_Boundaries[0]->m_segments.size();
		x_min = x_max = m_sweep_layers[i]->m_Boundaries[0]->m_segments[0]->pstart.x;
		for (unsigned int j = 0; j < sz; j++)
		{
			if (x_max <= m_sweep_layers[i]->offsetBoundaries[0]->m_segments[j]->pend.x)
				x_max = m_sweep_layers[i]->offsetBoundaries[0]->m_segments[j]->pend.x;
			if (x_min >= m_sweep_layers[i]->offsetBoundaries[0]->m_segments[j]->pend.x)
				x_min = m_sweep_layers[i]->offsetBoundaries[0]->m_segments[j]->pend.x;
		}

		x = x_min + 0.1;
		while (true)
		{
			SweepLine* tmp_sweep = new SweepLine();
			tmp_sweep->line_point = CPoint3D(x, 0, m_sweep_layers[i]->layerPoint.z);
			tmp_sweep->line_vec = CVector3D(0, 1, 0);
			tmp_sweep->line_normal = m_sweep_layers[i]->layer_coordinate[2];
			m_sweep_layers[i]->m_sweeplines.push_back(tmp_sweep);
			yaxisSweep(m_sweep_layers[i]);

			if (x >= (x_max - dx))
				break;
			x += dx;
		}
	}
}

void CSweep::yaxisSweep(SweepLayer * layer)
{
	unsigned int szL = layer->offsetBoundaries[0]->m_segments.size();
	CPoint3D line1_p1, line1_p2, line2_p1, line2_p2;
	CVector3D vec1, vec2;
	vector<CPoint3D> crosspoint;
	CPoint3D pout;
	line1_p1 = layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_point;
	line1_p2 = line1_p1 + layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_vec;
	for (unsigned int i = 0; i < szL; i++)
	{
		//vec1 = layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_vec;
		vec1 = CVector3D(line1_p1, layer->offsetBoundaries[0]->m_segments[i]->pstart);
		vec2 = CVector3D(line1_p1, layer->offsetBoundaries[0]->m_segments[i]->pend);
		if ((layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_vec*vec1).dz 
			*(layer->m_sweeplines[layer->m_sweeplines.size() - 1]->line_vec*vec2).dz <0)
		{
			line2_p1 = layer->offsetBoundaries[0]->m_segments[i]->pstart;
			line2_p2 = layer->offsetBoundaries[0]->m_segments[i]->pend;
			GetCrossPoint(pout, line1_p1, line1_p2, line2_p1, line2_p2);
			crosspoint.push_back(CPoint3D(pout));
			layer->m_Route.push_back(new CPoint3D(pout));
		}
	}
}

void CSweep::offSet(SweepLayer* layer, double offset)
{
	CPoint3D point_out;
	unsigned int sz = layer->m_Boundaries[0]->m_segments.size();
	layer->offsetBoundaries.push_back(new Boundary());
	for (unsigned int i = 0; i < sz; i++)
	{
		layer->offsetBoundaries[0]->m_segments.push_back(new Segment(*layer->m_Boundaries[0]->m_segments[i]));
	}
	CVector3D tmp_norm;
	CPoint3D tmp_point;
	Segment line1, line2;

	for (unsigned int i = 0; i < sz; i++)
	{
		tmp_norm = layer->layer_coordinate[2] * layer->offsetBoundaries[0]->m_segments[i]->segment_vec;
		tmp_norm.Normalize();
		layer->offsetBoundaries[0]->m_segments[i]->pstart = layer->offsetBoundaries[0]->m_segments[i]->pstart + CVector3D(tmp_norm*offset);
		layer->offsetBoundaries[0]->m_segments[i]->pend = layer->offsetBoundaries[0]->m_segments[i]->pend + CVector3D(tmp_norm*offset);
	}

	for (unsigned int i = 0; i < sz; i++)
	{
		line1.pstart = layer->offsetBoundaries[0]->m_segments[i]->pstart;
		line1.pend = layer->offsetBoundaries[0]->m_segments[i]->pend;

		line2.pstart = layer->offsetBoundaries[0]->m_segments[(i + 1) % sz]->pstart;
		line2.pend = layer->offsetBoundaries[0]->m_segments[(i + 1) % sz]->pend;

		GetCrossPoint(point_out, line1.pstart, line1.pend, line2.pstart, line2.pend);
		layer->offsetBoundaries[0]->m_segments[i]->pend = LPoint(point_out);
		layer->offsetBoundaries[0]->m_segments[(i + 1) % sz]->pstart = LPoint(point_out);
	}
}

/*
void CSweep::generateYPoint(int ext[], double y, int z)
{
	unsigned int sz = m_Boundaries[z]->m_Boundary.size();

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

		unsigned int szR = m_Routine.size();
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
*/
/*
void CSweep::generateXPoint(int ext[], double x, int z)
{
	unsigned int sz = m_Boundaries[z]->m_Boundary.size();

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

		unsigned int szR = m_Routine.size();
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
*/
void CSweep::loadSliceModel(CSlice* slicemodel)
{	
	unsigned int szLay = slicemodel->m_layers.size();
	for (unsigned int i = 0; i < szLay; i++)
	{
		m_slice_layers.push_back(slicemodel->m_layers[i]);		//为CSweep对象拷贝一份层切面数据
		m_sweep_layers.push_back(new SweepLayer(*slicemodel->m_layers[i]));
	}
}

/*
void CSweep::rearrange(int ext[], SweepLayer* layer)
{
	vector<Segment*> tmp_segments;
	for (unsigned int j = 0; j < m_Boundaries[z]->m_Boundary.size(); j++)
	{
		tmp_points.push_back(m_Boundaries[z]->m_Boundary[j]);
	}
	m_Boundaries[z]->m_Boundary.clear();

	unsigned int szTmp = tmp_points.size();
	CPoint3D* minpoint = tmp_points[0];
//	CPoint3D* maxpoint = tmp_points[0];
	ext[0] = 0;
//	ext[1] = 0;
	for (unsigned int j = 1; j < szTmp; j++)
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

	unsigned int num = 0;
	
	Boundary* tmp_boundary = new Boundary();
	tmp_boundary->z = tmp_points[0]->z;
	while (num < szTmp)
	{
		tmp_boundary->m_Boundary.push_back(tmp_points[(ext[0] + num) % szTmp]);
		num++;
	}
	m_Boundaries[z] = tmp_boundary;

	unsigned int sz = m_Boundaries[z]->m_Boundary.size();
	for (unsigned int k = 0; k < sz; k++)
	{
		m_Boundaries[z]->m_Boundary[k]->p_next = m_Boundaries[z]->m_Boundary[(k + 1) % sz];
		m_Boundaries[z]->m_Boundary[k]->p_prev = m_Boundaries[z]->m_Boundary[(sz + k - 1) % sz];
	}
}
*/

/*
void CSweep::findExtreme(int ext[], int z)
{
	CPoint3D* minpoint = m_Boundaries[z]->m_Boundary[0];
	CPoint3D* maxpoint = m_Boundaries[z]->m_Boundary[0];
	ext[0] = 0;
	ext[1] = 0;
	ext[2] = 0;
	ext[3] = 0;
	for (unsigned int j = 1; j < m_Boundaries[z]->m_Boundary.size(); j++)
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

	for (unsigned int j = 1; j < m_Boundaries[z]->m_Boundary.size(); j++)
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
	for (unsigned int j = 1; j < m_Boundaries[z]->m_Boundary.size(); j++)
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

	for (unsigned int j = 1; j < m_Boundaries[z]->m_Boundary.size(); j++)
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
*/
/*
SweepPoint* CSweep::getIntersectY(const CPoint3D* p1, const CPoint3D* p2, double y, double z, bool left)
{
	double x = p1->x + (y - p1->y)*(p2->x - p1->x) / (p2->y - p1->y);
	SweepPoint* tmp = new SweepPoint(x, y, z, left);
	return tmp;
}
*/

SweepPoint* CSweep::getIntersectX(const CPoint3D* p1, const CPoint3D* p2, double x, double z, bool left)
{
	double y = p1->y + (x - p1->x)*(p2->y - p1->y) / (p2->x - p1->x);
	SweepPoint* tmp = new SweepPoint(x, y, z, left);
	return tmp;
}

void CSweep::drawRoute()
{
	unsigned int szL = m_sweep_layers.size();
	for (unsigned int i = 0; i < szL; i++)
	{
		unsigned int sz = m_sweep_layers[i]->m_Route.size();
		for (unsigned int j = 0; j < sz - 1; j++)
		{
			CPoint3D p1 = CPoint3D(*m_sweep_layers[i]->m_Route[j]);
			CPoint3D p2 = CPoint3D(*m_sweep_layers[i]->m_Route[j+1]);
			glLineWidth(1.5f);
			glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glEnd();
		}
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
		unsigned int sz = m_Routine.size();
		double length = 0.0000;
		for (unsigned int i = 1; i < sz; i++)
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


