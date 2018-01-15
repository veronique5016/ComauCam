#pragma once


// DisplayDlg �Ի���

class CDisplayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDisplayDlg)

public:
	CDisplayDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDisplayDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bTriangleDraw;
	BOOL m_bFrameDraw;
	BOOL m_bPolygonDraw;
	BOOL m_bLinesDraw;

	BOOL m_bSTLModelDraw;
	BOOL m_bSliceModelDraw;
	BOOL m_bSweepModelDraw;
};
