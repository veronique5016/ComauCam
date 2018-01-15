#include "stdafx.h"
#include "SliceModel.h"
#include <vector>
using namespace std;

CSliceModel::CSliceModel(void)
{
	m_pSTLModel = NULL;
}


CSliceModel::~CSliceModel(void)
{
	int sz = m_layers.size();
	for (int i = sz - 1; i >= 0; i--)
	{
		delete m_layers[i];
		m_layers[i] = NULL;
	}
	m_layers.clear();
}

void CSliceModel::SetSTLModel(CSTLModel* pSTLModel)
{
	m_pSTLModel = pSTLModel;//���䱣�������ڻ�ȡģ���ļ�������һ��ָ����
}

void CSliceModel::PushToSpecifiedLayer(double zValue, const CLine &line)//���������ߴ浽��Ӧ�Ĳ�Ƭ��
{
	int sz = m_layers.size();
	CLayer* pCurLayer = NULL;
	for (int i = sz - 1; i >= 0; i--)
	{
		if (m_layers[i]->GetLayerZValue() == zValue)//���ĳ����Ƭ��z��ʶ�պ���zValue
		{
			pCurLayer = m_layers[i];//��ô��ǰ����������Ƭ
			break;
		}
	}

	if (NULL == pCurLayer)
	{
		pCurLayer = new CLayer(this);
		pCurLayer->SetLayerZValue(zValue);//����ͻ�õ�ǰ���zֵ�������m_layers
		m_layers.push_back(pCurLayer);    //�൱���½���һ��layer
	}

	pCurLayer->m_boundaryLines.push_back(line);//�ٽ�line���뵱ǰ���ˮƽ���������ֱ�ӽؽ��õ��Ľؽ��߶μ���
}

void CSliceModel::GntLayerBoundaries()//��ȡ��Ƭ����
{
	int sz = m_layers.size();
	for (int i = 0; i<sz; i++)
	{
		m_layers[i]->LinesToBoundaries();
	}
	//AfxMessageBox(_T("LinesToBoundaries finish!"),MB_OK,0);
}

void CSliceModel::Draw(COpenGLDC *pDC, bool ShowTri)
{
/*	
	 int szLayer=m_layers.size();
	 if (0==szLayer)
	 {
		return;
	 }

 //if (m_nDisplayLayer>=0 && m_nDisplayLayer<szLayer)
 //{
	int szBoundary=m_layers[0]->m_boundaryLines.size();
	glBegin(GL_LINE_LOOP);
	CPoint3D point;
	glVertex3d(100.0, 100.0, m_layers[0]->m_z);
	for (int j = 0; j<szBoundary; j++)
	{
		point.x = m_layers[0]->m_boundaryLines[j].m_pt1.x;
		point.y = m_layers[0]->m_boundaryLines[j].m_pt1.y;
		point.z = m_layers[0]->m_z;
		glVertex3d(point.x, point.y, m_layers[0]->m_z);
	}
	glVertex3d(100.0, 100.0, m_layers[0]->m_z);
	glEnd();
*/
	glBegin(GL_LINE);
	glColor3f(0.9, 0.2, 0.1);
	glVertex3d(100.0, 100.0, 0.0);
	glVertex3d(10.0, 10.0, 0.0);
	glEnd();
}

