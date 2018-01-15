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
	m_pSTLModel = pSTLModel;//将其保存在用于获取模型文件名的另一个指针中
}

void CSliceModel::PushToSpecifiedLayer(double zValue, const CLine &line)//将横截面的线存到对应的层片里
{
	int sz = m_layers.size();
	CLayer* pCurLayer = NULL;
	for (int i = sz - 1; i >= 0; i--)
	{
		if (m_layers[i]->GetLayerZValue() == zValue)//如果某个层片的z标识刚好是zValue
		{
			pCurLayer = m_layers[i];//那么当前层就是这个层片
			break;
		}
	}

	if (NULL == pCurLayer)
	{
		pCurLayer = new CLayer(this);
		pCurLayer->SetLayerZValue(zValue);//否则就获得当前层的z值将其存入m_layers
		m_layers.push_back(pCurLayer);    //相当于新建了一个layer
	}

	pCurLayer->m_boundaryLines.push_back(line);//再将line存入当前层的水平面和三角形直接截交得到的截交线段集合
}

void CSliceModel::GntLayerBoundaries()//获取层片轮廓
{
	int sz = m_layers.size();
	for (int i = 0; i<sz; i++)
	{
		m_layers[i]->LinesToBoundaries();
	}
	//AfxMessageBox(_T("LinesToBoundaries finish!"),MB_OK,0);
}

void CSliceModel::Draw(COpenGLDC *pDC)
{/*
 int szLayer=m_layers.size();
 if (0==szLayer)
 {
 return;
 }

 if (m_nDisplayLayer>=0 && m_nDisplayLayer<szLayer)
 {
 int szBoundary=m_layers[m_nDisplayLayer]->m_boundaries.size();
 for (int j=0;j<szBoundary;j++)
 {
 m_layers[m_nDisplayLayer]->m_boundaries[j]->Draw(pDC);
 }
 m_layers[m_nDisplayLayer]->m_scanPath.Draw(pDC);
 }
 else if (m_nDisplayLayer<0)
 {
 for (int i=0;i<szLayer;i=i+1)
 {
 int szBoundary=m_layers[i]->m_boundaries.size();
 for (int j=0;j<szBoundary;j++)
 {
 m_layers[i]->m_boundaries[j]->Draw(pDC);
 }
 m_layers[i]->m_scanPath.Draw(pDC);
 }
 }
 */
 //////////////////////////////////////////////////////////////////////////
 // for STL

 // 	int szLine=m_layers[0]->m_boundaryLines.size();
 // 	for (int j=0;j<szLine;j++)
 // 	{
 // 		m_layers[1]->m_boundaryLines[j].SetColor(255,0,0);
 // 		m_layers[1]->m_boundaryLines[j].Draw(pDC);
 // 	}

 // test
 /*
 for (int i=0;i<1;i++)
 {
 int szLine=m_layers[i]->m_boundaryLines.size();
 for (int j=0;j<szLine;j++)
 {
 m_layers[i]->m_boundaryLines[j].SetColor(255,0,0);
 m_layers[i]->m_boundaryLines[j].Draw(pDC);
 }
 }
 */


 // 	for (int i=0;i<1;i=i+1)
 // 	{
 // 		int szBoundary=m_layers[i]->m_boundaries.size();
 //  		for (int j=0;j<szBoundary;j++)
 //  		{
 // 			m_layers[i]->m_boundaries[0]->Draw(pDC);
 // 		}
 // 	}

}

