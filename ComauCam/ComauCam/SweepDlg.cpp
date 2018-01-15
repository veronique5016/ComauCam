// SweepDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ComauCam.h"
#include "SweepDlg.h"
#include "afxdialogex.h"


// CSweepDlg 对话框

IMPLEMENT_DYNAMIC(CSweepDlg, CDialogEx)

CSweepDlg::CSweepDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, m_sweepDistance(10)
{

}

CSweepDlg::~CSweepDlg()
{
}

void CSweepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sweepDistance);
}


BEGIN_MESSAGE_MAP(CSweepDlg, CDialogEx)
END_MESSAGE_MAP()


// CSweepDlg 消息处理程序
