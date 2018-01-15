// DisplayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ComauCam.h"
#include "DisplayDlg.h"
#include "afxdialogex.h"


// DisplayDlg 对话框

IMPLEMENT_DYNAMIC(CDisplayDlg, CDialogEx)

CDisplayDlg::CDisplayDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DISPLAYDIALOG, pParent)
	, m_bTriangleDraw(FALSE)
	, m_bFrameDraw(FALSE)
	, m_bPolygonDraw(FALSE)
	, m_bLinesDraw(FALSE)
	, m_bSTLModelDraw(FALSE)
	, m_bSliceModelDraw(FALSE)
	, m_bSweepModelDraw(FALSE)
{

}

CDisplayDlg::~CDisplayDlg()
{
}

void CDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SHOWTRIANGLE, m_bTriangleDraw);
	DDX_Check(pDX, IDC_SHOWFRAME, m_bFrameDraw);
	DDX_Check(pDX, IDC_SHOWPOLYGON, m_bPolygonDraw);
	DDX_Check(pDX, IDC_SHOWLINES, m_bLinesDraw);
	DDX_Check(pDX, IDC_CANSTLSHOW, m_bSTLModelDraw);
	DDX_Check(pDX, IDC_CANSLICESHOW, m_bSliceModelDraw);
	DDX_Check(pDX, IDC_CANSWEEPSHOW, m_bSweepModelDraw);
}


BEGIN_MESSAGE_MAP(CDisplayDlg, CDialogEx)
END_MESSAGE_MAP()


// DisplayDlg 消息处理程序
