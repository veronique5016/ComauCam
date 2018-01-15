#include "stdafx.h"
#include "Layer.h"
#include "Global.h"
#include "GeomBase.h"
//#include <vector>
using namespace std;
CLayer::CLayer(CSliceModel* pSliceModel)
{
	m_pSliceModel = pSliceModel;
}


CLayer::~CLayer(void)
{
	int sz = m_boundaries.size();
	for (int i = sz - 1; i >= 0; i--)
	{
		delete m_boundaries[i];
		m_boundaries[i] = NULL;
	}
	m_boundaries.clear();
}

void CLayer::SetLayerZValue(double z)
{
	m_z = z;
}

double CLayer::GetLayerZValue()//��ȡ��Ƭ��zֵ
{
	return m_z;
}

void CLayer::LinesToBoundaries()//������������
{
//	::LinesToPolylines(m_boundaryLines, m_boundaries, true);//�������ɶ����
	IdentifyBoundaryDirections();
}
void CLayer::IdentifyBoundaryDirections()       //�б������ķ���
{
	int sz = m_boundaries.size();
	if (0 == sz)
	{
		return;
	}
	if (1 == sz)
	{
		m_boundaries[0]->MakePolygonCCW();//��ʱ�뷽�����ɶ����
		return;
	}
	vector<CBoundary*> tmp_boundaries;//�ȶ���һ����̬����
	for (int i = 0; i<sz; i++)
	{
		tmp_boundaries.push_back(m_boundaries[i]);//�������������ȥ
	}

	int cnt = -1;
	while (true)
	{
		cnt++;
		sz = tmp_boundaries.size();
		if (sz == 0)
		{
			break;
		}

		vector<int> idx;
		for (int i = 0; i<sz; i++)
		{
			if (IsOutMost(tmp_boundaries[i], tmp_boundaries))
			{
				idx.push_back(i);
			}
		}

		int szIdx = idx.size();
		for (int i = szIdx - 1; i >= 0; i--)
		{
			0 == cnt % 2 ? tmp_boundaries[idx[i]]->MakePolygonCCW() : tmp_boundaries[idx[i]]->MakePolygonCW();
			tmp_boundaries.erase(tmp_boundaries.begin() + idx[i]);
		}
		idx.clear();
	}
}
bool CLayer::IsOutMost(CBoundary* pCurBoundary, vector<CBoundary*>& boundaries)   // �������ж����ɵ������и������������һ��
{
	int sz = boundaries.size();
	//CPoint3D ptTest = pCurBoundary->GetFirstPt();
	for (int i = 0; i<sz; i++)
	{
		if (pCurBoundary == boundaries[i])
		{
			continue;
		}
		else if (IsContain(pCurBoundary, boundaries[i])  /*boundaries[i]->IsPtInPolygon(ptTest)*/)
		{
			return false;
		}
	}

	return true;
}

bool CLayer::IsContain(CBoundary* pElement, CBoundary* pContainer)   // �ж����� pElement �Ƿ����� pContainer ����
{
	int i = 0, j = 0;
	int	szC = pContainer->m_pts.GetSize();
	int szE = pElement->m_pts.GetSize();
	CPoint3D ptTest;
	double d2 = 0;
	while (i<szE)
	{
		ptTest = *pElement->m_pts[i];
		for (j = 0; j <= szC - 2; j++)
		{
			d2 = ::GetDist2PtToSeg(ptTest, *pContainer->m_pts[j], *pContainer->m_pts[j + 1]);
			if (d2 < 1.0e-12)
			{
				break;
			}
		}

		if (j == szC - 1)
		{
			break;
		}
		else
		{
			i++;
		}
	}

	return pContainer->IsPtInPolygon(ptTest);
}