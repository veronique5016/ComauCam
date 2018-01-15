#include "stdafx.h"
#include "LVertex.h"

CLVertex::CLVertex()
{

}

CLVertex::CLVertex(const CLVertex& vertex)
{
	x = vertex.x;
	y = vertex.y;
	z = vertex.z;
}
CLVertex::CLVertex(const CPoint3D& pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
}

bool CLVertex::operator < (const CLVertex& vertex)  // 左下角 到 右上角
{
	if ((x<vertex.x)
		|| (x == vertex.x && y<vertex.y)
		|| (x == vertex.x && y == vertex.y && z<vertex.z))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CLVertex::operator > (const CLVertex& vertex)
{
	if (*this == vertex)
	{
		return false;
	}

	return !((*this)<vertex);
}

bool CLVertex::IsLower(const CLVertex& vertex)//顶点比较函数
{
	if (z < vertex.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CLVertex::IsLower(CLVertex* pVertex)
{
	return IsLower(*pVertex);
}


//////////////////////////////////////////////////////////////////////////

CLEdge::CLEdge()
{
	v1 = v2 = NULL;
	t = NULL;
	e_prev = e_next = e_adja = NULL;
}

CLEdge::CLEdge(CLVertex* _v1, CLVertex* _v2)
{
	v1 = _v1;
	v2 = _v2;

	e_adja = NULL;
}
//////////////////////////////////////////////////////////////////////////
CLTriangle::CLTriangle()
{
	v1 = v2 = v3 = NULL;
	e1 = e2 = e3 = NULL;
	n = NULL;
	m_bUse = false;     //面片是否被使用
	m_nFaceType = 0;  //面片类型
	m_pIntersectLine1 = NULL;     //上一面片已经求交的相交线，用于寻找下一相交线
	m_pIntersectLine2= NULL;      //另一条相交线
}

CLTriangle::CLTriangle(CLVertex* v1, CLVertex* v2, CLVertex* v3)
{
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;

	e1 = e2 = e3 = NULL;
	n = NULL;
}

CVector3D* CLTriangle::GntNormal()
{
	CVector3D vec1 = *v2 - *v1;
	CVector3D vec2 = *v3 - *v2;
	CVector3D vec = vec1*vec2;
	vec.Normalize();
	n = new CVector3D(vec);
	return n;  // 返回n放在外面数组里以便统一管理
}

CLTriangle* CLTriangle::GetNbTri1() const//寻找附近面片
{
	return e1->e_adja ? e1->e_adja->t : NULL;
}

CLTriangle* CLTriangle::GetNbTri2() const
{
	return e2->e_adja ? e2->e_adja->t : NULL;
}

CLTriangle* CLTriangle::GetNbTri3() const
{
	return e3->e_adja ? e3->e_adja->t : NULL;
}

void CLTriangle::DrawLTriangle(COpenGLDC* pDC)
{
	pDC->DrawTriChip(*n, *v1, *v2, *v3);

}

//////////////////////////////////////////////////////////////////////////

bool CLEdgeHull::IsOpposite(const CLEdgeHull& edgeHull)//判断两条线是否方向相反
{
	if (m_pEdge->v1 == edgeHull.m_pEdge->v2 && m_pEdge->v2 == edgeHull.m_pEdge->v1)//即二者的起点终点刚好相反
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CLEdgeHull::operator < (const CLEdgeHull& edgeHull)
{
	CLVertex v1 = *m_pEdge->v1;  // v1 < v2
	CLVertex v2 = *m_pEdge->v2;
	if (v2 < v1)
	{
		swap(v1, v2);
	}

	CLVertex v1_ = *edgeHull.m_pEdge->v1;
	CLVertex v2_ = *edgeHull.m_pEdge->v2;
	if (v2_ < v1_)
	{
		swap(v1_, v2_);
	}

	double& a1 = v1.x;
	double& b1 = v1.y;
	double& c1 = v1.z;
	double& d1 = v2.x;
	double& e1 = v2.y;
	double& f1 = v2.z;

	double& a2 = v1_.x;
	double& b2 = v1_.y;
	double& c2 = v1_.z;
	double& d2 = v2_.x;
	double& e2 = v2_.y;
	double& f2 = v2_.z;

	if ((a1<a2)
		|| (a1 == a2 && b1<b2)
		|| (a1 == a2 && b1 == b2 && c1<c2)
		|| (a1 == a2 && b1 == b2 && c1 == c2 && d1<d2)
		|| (a1 == a2 && b1 == b2 && c1 == c2 && d1 == d2 && e1<e2)
		|| (a1 == a2 && b1 == b2 && c1 == c2 && d1 == d2 && e1 == e2 && f1<f2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
CTopologySTL::CTopologySTL()
{

}

CTopologySTL::~CTopologySTL()
{
	unsigned int sz = m_vecpVertices.size();
	for (int i = sz - 1; i >= 0; i--)
	{
		if (NULL != m_vecpVertices[i])
		{
			delete m_vecpVertices[i];
			m_vecpVertices[i] = NULL;
		}
	}
	m_vecpVertices.clear();

	sz = m_vecpLTris.size();
	for (int i = sz - 1; i >= 0; i--)
	{
		if (NULL != m_vecpLTris[i])
		{
			delete m_vecpLTris[i];
			m_vecpLTris[i] = NULL;
		}
	}
	m_vecpLTris.clear();

	sz = m_vecpEdges.size();
	for (int i = sz - 1; i >= 0; i--)
	{
		if (NULL != m_vecpEdges[i])
		{
			delete m_vecpEdges[i];
			m_vecpEdges[i] = NULL;
		}
	}
	m_vecpEdges.clear();

	sz = m_vecpNormals.size();
	for (int i = sz - 1; i >= 0; i--)
	{
		if (NULL != m_vecpNormals[i])
		{
			delete m_vecpNormals[i];
			m_vecpNormals[i] = NULL;
		}
	}
}

