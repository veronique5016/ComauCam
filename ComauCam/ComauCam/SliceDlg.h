#pragma once


// CSliceDlg �Ի���

class CSliceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSliceDlg)

public:
	CSliceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSliceDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_sliceDistance;
};
