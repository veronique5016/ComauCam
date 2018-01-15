// SliceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ComauCam.h"
#include "SliceDlg.h"
#include "afxdialogex.h"


// CSliceDlg 对话框

IMPLEMENT_DYNAMIC(CSliceDlg, CDialogEx)

CSliceDlg::CSliceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_dSliceDistance(10)
{

}

CSliceDlg::~CSliceDlg()
{
}

void CSliceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dSliceDistance);
}


BEGIN_MESSAGE_MAP(CSliceDlg, CDialogEx)
END_MESSAGE_MAP()


// CSliceDlg 消息处理程序
