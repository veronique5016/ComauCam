// SliceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ComauCam.h"
#include "SliceDlg.h"
#include "afxdialogex.h"


// CSliceDlg �Ի���

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


// CSliceDlg ��Ϣ�������
