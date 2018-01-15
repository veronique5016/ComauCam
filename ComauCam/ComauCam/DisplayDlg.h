#pragma once


// DisplayDlg �Ի���

class DisplayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DisplayDlg)

public:
	DisplayDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DisplayDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_ShowTriangle;
	BOOL m_ShowFrame;
	BOOL m_ShowPolygon;
	BOOL m_ShowLines;

	BOOL m_showStlModel;
	BOOL m_showSliceModel;
	BOOL m_showSweepModel;
};
