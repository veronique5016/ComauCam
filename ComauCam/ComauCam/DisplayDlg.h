#pragma once


// DisplayDlg 对话框

class DisplayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DisplayDlg)

public:
	DisplayDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DisplayDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
