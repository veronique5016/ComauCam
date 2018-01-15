// TransferDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ComauCam.h"
#include "TransferDlg.h"
#include "afxdialogex.h"


// TransferDlg 对话框

IMPLEMENT_DYNAMIC(TransferDlg, CDialogEx)

TransferDlg::TransferDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRANSFERDIALOG, pParent)
	, x_move(0)
	, y_move(0)
	, z_move(0)
	, angle(0)
	, x_axis(0)
	, y_axis(0)
	, z_axis(0)
{

}

TransferDlg::~TransferDlg()
{
}

void TransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_XMOVE, x_move);
	DDX_Text(pDX, IDC_YMOVE, y_move);
	DDX_Text(pDX, IDC_EDIT5, z_move);
	DDX_Text(pDX, IDC_ANGLE, angle);
	DDX_Text(pDX, IDC_XAXIS, x_axis);
	DDX_Text(pDX, IDC_YAXIS, y_axis);
	DDX_Text(pDX, IDC_ZAXIS, z_axis);
}


BEGIN_MESSAGE_MAP(TransferDlg, CDialogEx)
END_MESSAGE_MAP()


// TransferDlg 消息处理程序
