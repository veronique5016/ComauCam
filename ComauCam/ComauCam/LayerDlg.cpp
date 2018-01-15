// LayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ComauCam.h"
#include "LayerDlg.h"
#include "afxdialogex.h"


// LayerDlg 对话框

IMPLEMENT_DYNAMIC(LayerDlg, CDialogEx)

LayerDlg::LayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SHOWLAYERDIALOG, pParent)
	, startLayer(1)
	, endLayer(1)
	, numberofLayers(0)
	, firstTurnLayer(0)
{

}

LayerDlg::~LayerDlg()
{
}

void LayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, startLayer);
	DDX_Text(pDX, IDC_EDIT2, endLayer);
	DDX_Text(pDX, IDC_EDIT3, firstTurnLayer);
	DDX_Text(pDX, IDC_EDIT4, numberofLayers);
}


BEGIN_MESSAGE_MAP(LayerDlg, CDialogEx)
END_MESSAGE_MAP()


// LayerDlg 消息处理程序
