#include "stdafx.h"
#include "Layer.h"


CSegment::CSegment()
{
	m_ptStart = CPoint3D(0, 0, 0);
	m_ptEnd = CPoint3D(0, 0, 0);
	m_pTriangle =new CLTriangle();
	m_vSegmentVec = CVector3D(0, 0, 0);
}
CSegment::CSegment(const CSegment& lsegment)
{
	m_ptStart = lsegment.m_ptStart;
	m_ptEnd = lsegment.m_ptEnd;
	m_pTriangle = lsegment.m_pTriangle;
	m_vSegmentVec = lsegment.m_vSegmentVec;
}
CSegment::CSegment(CPoint3D startpoint, CPoint3D endpoint, CLTriangle* tri)
{
	m_ptStart = startpoint;
	m_ptEnd = endpoint;
	m_pTriangle = tri;
	CPoint3D p1 = CPoint3D(startpoint.x, startpoint.y, startpoint.z);
	CPoint3D p2 = CPoint3D(endpoint.x, endpoint.y, endpoint.z);
	m_vSegmentVec = CVector3D(p1, p2);
	m_vSegmentVec.Normalize();
}

CSegment::~CSegment()
{
}


CBoundary::CBoundary()
{
}
CBoundary::CBoundary(const CBoundary & boundary)
{
	unsigned int szSeg = boundary.m_vecpSegments.size();
	for (unsigned int i = 0; i < szSeg; i++)
	{
		m_vecpSegments.push_back(new CSegment(*boundary.m_vecpSegments[i]));
	}
}
CBoundary::~CBoundary()
{
	unsigned int szP = m_vecpSegments.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_vecpSegments[i];
		m_vecpSegments[i] = NULL;
	}
	m_vecpSegments.clear();
}

CSliceLayer::CSliceLayer()
{
	m_vLayerCoordinate[0] = CVector3D(1, 0, 0);
	m_vLayerCoordinate[1] = CVector3D(0, 1, 0);
	m_vLayerCoordinate[2] = CVector3D(0, 0, 1);
	m_pAddictedLayer = NULL;
}
CSliceLayer::~CSliceLayer()
{
	unsigned int szP = m_vecpBoundaries.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		delete m_vecpBoundaries[i];
		m_vecpBoundaries[i] = NULL;
	}
	m_vecpBoundaries.clear();
}

//取出轮廓中点集的三等分点判断轮廓是否逆时针
bool CSliceLayer::IsBoundaryCCW()
{
	unsigned int sz = m_vecpBoundaries[0]->m_vecpSegments.size();
	double angle = -1.0;
	CPoint3D p1, p2;
	CVector3D vec1, vec2;
	CVector3D vec = CVector3D(0, 0, 0);
	if (sz < 3)
	{
		return false;
	}
	else
	{
		p1 = CPoint3D(m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptStart);
		p2 = CPoint3D(m_vecpBoundaries[0]->m_vecpSegments[sz / 3]->m_ptStart);
		vec1 = CVector3D(p1, p2);
		p1 = CPoint3D(m_vecpBoundaries[0]->m_vecpSegments[sz / 3]->m_ptStart);
		p2 = CPoint3D(m_vecpBoundaries[0]->m_vecpSegments[(sz / 3) * 2]->m_ptStart);
		vec2 = CVector3D(p1, p2);
		vec = vec1 * vec2;
		vec.Normalize();

		angle = GetAngle(vec, m_vLayerCoordinate[2]);
		if (angle == 0.0)
			return true;
		else
			return false;
	}
}

void CSliceLayer::MakeBoundaryCCW()
{
	unsigned int sz = m_vecpBoundaries[0]->m_vecpSegments.size();
	vector<CSegment*> tmp;
	for (unsigned int i = 0; i < sz; i++)
	{
		tmp.push_back(m_vecpBoundaries[0]->m_vecpSegments[sz - 1 - i]);
	}
	m_vecpBoundaries[0]->m_vecpSegments.clear();
	for (unsigned int i = 0; i < sz; i++)
	{
		m_vecpBoundaries[0]->m_vecpSegments.push_back(new CSegment(*tmp[i]));
		m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart = CPoint3D(tmp[i]->m_ptEnd);
		m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd = CPoint3D(tmp[i]->m_ptStart);
		m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec = CVector3D(CPoint3D(m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart), CPoint3D(m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd));
		m_vecpBoundaries[0]->m_vecpSegments[i]->m_vSegmentVec.Normalize();
	}
}

void CSliceLayer::RearrangeBoundary()
{
	unsigned int szP = m_vecpBoundaries[0]->m_vecpSegments.size();
	double position = m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptStart.x + m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptStart.y;
	int index = 0;
	for (unsigned int i = 0; i < szP; i++)
	{
		CPoint3D tmp_point = m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart;
		double tmp_position = tmp_point.x + tmp_point.y;
		if (tmp_position < position)
		{
			position = tmp_position;
			index = i;
		}
	}
	vector<CSegment*> tmp_segments;
	for (unsigned int i = 0; i < szP; i++)
	{
		tmp_segments.push_back(m_vecpBoundaries[0]->m_vecpSegments[(i + index) % szP]);
	}
	m_vecpBoundaries[0]->m_vecpSegments.clear();
	for (unsigned int i = 0; i < szP; i++)
	{
		m_vecpBoundaries[0]->m_vecpSegments.push_back(tmp_segments[i]);
	}
	tmp_segments.clear();
}

void CSliceLayer::DeletePoints()
{
	vector<CSegment*> tmp_segments;
	unsigned int szP = m_vecpBoundaries[0]->m_vecpSegments.size();

	// 取出单个层切面轮廓中的所有线段
	for (unsigned int j = 0; j < szP; j++)
	{
		tmp_segments.push_back(m_vecpBoundaries[0]->m_vecpSegments[j]);
	}
	m_vecpBoundaries[0]->m_vecpSegments.clear();

	unsigned int szTemp = tmp_segments.size();
	for (unsigned int i = 0; i < szTemp; i++)
	{
		if (fabs(tmp_segments[i]->m_ptStart.x) <= 0.0000001)
			tmp_segments[i]->m_ptStart.x = 0;
		if (fabs(tmp_segments[i]->m_ptStart.y) <= 0.0000001)
			tmp_segments[i]->m_ptStart.y = 0;
		if (fabs(tmp_segments[i]->m_ptStart.z) <= 0.0000001)
			tmp_segments[i]->m_ptStart.z = 0;
		if (fabs(tmp_segments[i]->m_ptEnd.x) <= 0.0000001)
			tmp_segments[i]->m_ptEnd.x = 0;
		if (fabs(tmp_segments[i]->m_ptEnd.y) <= 0.0000001)
			tmp_segments[i]->m_ptEnd.y = 0;
		if (fabs(tmp_segments[i]->m_ptEnd.z) <= 0.0000001)
			tmp_segments[i]->m_ptEnd.z = 0;
	}

	CPoint3D pCur, pNext, pNextNext;
	while (szTemp >= 3)
	{
		for (unsigned int j = 0; j < szTemp; j++)
		{
			pCur = tmp_segments[j%szTemp]->m_ptStart;
			pNext = tmp_segments[j%szTemp]->m_ptEnd;
			pNextNext = tmp_segments[(j + 1) % szTemp]->m_ptEnd;
			double distance = CalPointtoLine(pNext, pCur, pNextNext);
			if (distance <= 0.0001)
			{
				tmp_segments[j%szTemp]->m_ptEnd = tmp_segments[(j + 1) % szTemp]->m_ptEnd;
				CPoint3D p1 = CPoint3D(tmp_segments[j%szTemp]->m_ptStart);
				CPoint3D p2 = CPoint3D(tmp_segments[j%szTemp]->m_ptEnd);
				tmp_segments[j%szTemp]->m_vSegmentVec = CVector3D(p1, p2);
				tmp_segments[j%szTemp]->m_vSegmentVec.Normalize();
				tmp_segments.erase(tmp_segments.begin() + (j + 1) % szTemp);
				szTemp = tmp_segments.size();
				j = -1;
			}
		}
		break;
	}

	//将删除共线点之后的线段存回 m_vecpSegments 中
	for (unsigned int i = 0; i < tmp_segments.size(); i++)
	{
		m_vecpBoundaries[0]->m_vecpSegments.push_back(tmp_segments[i]);
	}
	tmp_segments.clear();
}

void CSliceLayer::OptimizeBoundary()
{
	bool isCCW = IsBoundaryCCW();
	if (!isCCW)
		MakeBoundaryCCW();
	DeletePoints();
}

double CSliceLayer::ZminofLayer()
{
	double zmin;
	zmin = m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptStart.z;
	unsigned int sz = m_vecpBoundaries[0]->m_vecpSegments.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		if (zmin > m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd.z)
			zmin = m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd.z;
	}
	return zmin;
}

int CSliceLayer::FindLowestSegment()
{
	int index = -1;
	if (m_vLayerCoordinate[2].dz == 1.0)
	{
		return index;
	}
	else
	{
		int szP = m_vecpBoundaries[0]->m_vecpSegments.size();
		double height = m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptStart.z + m_vecpBoundaries[0]->m_vecpSegments[0]->m_ptEnd.z;
		for (unsigned int i = 0; i < szP; i++)
		{
			double tmp_height = m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptStart.z + m_vecpBoundaries[0]->m_vecpSegments[i]->m_ptEnd.z;
			if (height >= tmp_height)
			{
				height = tmp_height;
				index = i;
			}
		}
		return index;
	}
}

CSweepLine::CSweepLine()
{
}
CSweepLine::~CSweepLine()
{
}

CSweepLayer::CSweepLayer()
{
}
CSweepLayer::CSweepLayer(const CSliceLayer & sliceLayer)
{
	m_ptLayerPoint = sliceLayer.m_ptLayerPoint;
	m_vLayerCoordinate[0] = sliceLayer.m_vLayerCoordinate[0];
	m_vLayerCoordinate[1] = sliceLayer.m_vLayerCoordinate[1];
	m_vLayerCoordinate[2] = sliceLayer.m_vLayerCoordinate[2];
	unsigned int sz = sliceLayer.m_vecpBoundaries.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		m_vecpBoundaries.push_back(sliceLayer.m_vecpBoundaries[i]);
	}
}
CSweepLayer::CSweepLayer(const CSweepLayer & sweepLayer)
{
	m_ptLayerPoint = sweepLayer.m_ptLayerPoint;
	m_vLayerCoordinate[0] = sweepLayer.m_vLayerCoordinate[0];
	m_vLayerCoordinate[1] = sweepLayer.m_vLayerCoordinate[1];
	m_vLayerCoordinate[2] = sweepLayer.m_vLayerCoordinate[2];
	unsigned int sz = sweepLayer.m_vecpBoundaries.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		m_vecpBoundaries.push_back(sweepLayer.m_vecpBoundaries[i]);
	}

	unsigned int szL = sweepLayer.m_vecpSweepLines.size();
	for (unsigned int i = 0; i < szL; i++)
	{
		m_vecpSweepLines.push_back(sweepLayer.m_vecpSweepLines[i]);
	}

	unsigned int szP = sweepLayer.m_vecpRoute.size();
	for (unsigned int i = 0; i < szP; i++)
	{
		m_vecpRoute.push_back(new CPoint3D(*sweepLayer.m_vecpRoute[i]));
	}

	unsigned int szT = sweepLayer.m_vecpTurnRoute.size();
	for (unsigned int i = 0; i < szT; i++)
	{
		m_vecpTurnRoute.push_back(new CSweepPoint(*sweepLayer.m_vecpTurnRoute[i]));
	}
}
CSweepLayer::~CSweepLayer()
{
	unsigned int szB = m_vecpOffsetBoundaries.size();
	for (unsigned int i = 0; i < szB; i++)
	{
		delete m_vecpOffsetBoundaries[i];
		m_vecpOffsetBoundaries[i] = NULL;
	}
	m_vecpOffsetBoundaries.clear();

	unsigned int szR = m_vecpRoute.size();
	for (unsigned int i = 0; i < szR; i++)
	{
		delete m_vecpRoute[i];
		m_vecpRoute[i] = NULL;
	}
	m_vecpRoute.clear();

	unsigned int szL = m_vecpSweepLines.size();
	for (unsigned int i = 0; i < szR; i++)
	{
		delete m_vecpSweepLines[i];
		m_vecpSweepLines[i] = NULL;
	}
	m_vecpSweepLines.clear();

	unsigned int szT = m_vecpTurnRoute.size();
	for (unsigned int i = 0; i < szT; i++)
	{
		delete m_vecpTurnRoute[i];
		m_vecpTurnRoute[i] = NULL;
	}
	m_vecpTurnRoute.clear();
}

CSweepPoint::CSweepPoint()
{
}

CSweepPoint::~CSweepPoint()
{
}

CSweepPoint::CSweepPoint(const CPoint3D & pt, double a, double c)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
	A = a;
	C = c;
}

CSweepPoint::CSweepPoint(const CSweepPoint & pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
	A = pt.A;
	C = pt.C;
}

void MoveSegment(CSegment* lsegment, CVector3D vec)
{
	lsegment->m_ptStart = lsegment->m_ptStart + vec;
	lsegment->m_ptEnd = lsegment->m_ptEnd + vec;
}

void GetCrossPoint(CPoint3D& pt_out, CSegment seg1, CSegment seg2)
{
	GetCrossPoint(pt_out, seg1.m_ptStart, seg1.m_ptEnd, seg2.m_ptStart, seg2.m_ptEnd);
}

void GetCrossPoint(CPoint3D & pt_out, CSweepLine line1, CSegment line2)
{
	CPoint3D pStart = line1.m_ptLinePoint;
	CPoint3D pEnd = pStart + line1.m_vLineVec * 10;
	GetCrossPoint(pt_out, pStart, pEnd, line2.m_ptStart, line2.m_ptEnd);
}

void PointToQuad(CPoint3D quad[4], CPoint3D point, double offset, CVector3D coordinate[3])
{
	CPoint3D tmp = point;
	CVector3D offset_vec = coordinate[1] + coordinate[2];
	offset_vec.Normalize();
	tmp = point + offset_vec*offset;
	quad[0] = tmp;

	offset_vec = coordinate[2] - coordinate[1];
	offset_vec.Normalize();
	tmp = point + offset_vec*offset;
	quad[1] = tmp;

	offset_vec = CVector3D(0, 0, 0) - coordinate[2] - coordinate[1];
	offset_vec.Normalize();
	tmp = point + offset_vec*offset;
	quad[2] = tmp;

	offset_vec = coordinate[1] - coordinate[2];
	offset_vec.Normalize();
	tmp = point + offset_vec*offset;
	quad[3] = tmp;
}

void Offset(CBoundary* boundary, double offset, CVector3D coordinate[])
{
	CPoint3D point_out;
	unsigned int sz = boundary->m_vecpSegments.size();

	CVector3D offset_vec;
	CPoint3D tmp_point;
	CSegment line1, line2;

	//将轮廓中所有的线段往内偏移一个距离
	for (unsigned int i = 0; i < sz; i++)
	{
		offset_vec = coordinate[2] * boundary->m_vecpSegments[i]->m_vSegmentVec;
		offset_vec.Normalize();
		MoveSegment(boundary->m_vecpSegments[i], CVector3D(offset_vec*offset));
	}

	//对偏移后的线段求交点，形成偏移后的轮廓
	for (unsigned int i = 0; i < sz; i++)
	{
		line1 = *boundary->m_vecpSegments[i];
		line2 = *boundary->m_vecpSegments[(i + 1) % sz];

		GetCrossPoint(point_out, line1, line2);
		boundary->m_vecpSegments[i]->m_ptEnd = CPoint3D(point_out);
		boundary->m_vecpSegments[(i + 1) % sz]->m_ptStart = CPoint3D(point_out);
	}
}
