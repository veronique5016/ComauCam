#pragma once


// CSweepDlg �Ի���

class CSweepDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSweepDlg)

public:
	CSweepDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSweepDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_sweepDistance;
};
