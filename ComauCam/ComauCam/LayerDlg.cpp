// LayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ComauCam.h"
#include "LayerDlg.h"
#include "afxdialogex.h"


// LayerDlg 对话框

IMPLEMENT_DYNAMIC(CLayerDlg, CDialogEx)

CLayerDlg::CLayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SHOWLAYERDIALOG, pParent)
	, m_nStartLayer(1)
	, m_nEndLayer(1)
	, m_nNumberofLayers(0)
	, m_nFirstTurnLayer(0)
{

}

CLayerDlg::~CLayerDlg()
{
}

void CLayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nStartLayer);
	DDX_Text(pDX, IDC_EDIT2, m_nEndLayer);
	DDX_Text(pDX, IDC_EDIT3, m_nFirstTurnLayer);
	DDX_Text(pDX, IDC_EDIT4, m_nNumberofLayers);
}


BEGIN_MESSAGE_MAP(CLayerDlg, CDialogEx)
END_MESSAGE_MAP()


// LayerDlg 消息处理程序
