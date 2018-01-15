// DisplayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ComauCam.h"
#include "DisplayDlg.h"
#include "afxdialogex.h"


// DisplayDlg 对话框

IMPLEMENT_DYNAMIC(DisplayDlg, CDialogEx)

DisplayDlg::DisplayDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DISPLAYDIALOG, pParent)
	, m_ShowTriangle(FALSE)
	, m_ShowFrame(FALSE)
	, m_ShowPolygon(FALSE)
	, m_ShowLines(FALSE)
	, m_showStlModel(FALSE)
	, m_showSliceModel(FALSE)
	, m_showSweepModel(FALSE)
{

}

DisplayDlg::~DisplayDlg()
{
}

void DisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SHOWTRIANGLE, m_ShowTriangle);
	DDX_Check(pDX, IDC_SHOWFRAME, m_ShowFrame);
	DDX_Check(pDX, IDC_SHOWPOLYGON, m_ShowPolygon);
	DDX_Check(pDX, IDC_SHOWLINES, m_ShowLines);
	DDX_Check(pDX, IDC_CANSTLSHOW, m_showStlModel);
	DDX_Check(pDX, IDC_CANSLICESHOW, m_showSliceModel);
	DDX_Check(pDX, IDC_CANSWEEPSHOW, m_showSweepModel);
}


BEGIN_MESSAGE_MAP(DisplayDlg, CDialogEx)
END_MESSAGE_MAP()


// DisplayDlg 消息处理程序
