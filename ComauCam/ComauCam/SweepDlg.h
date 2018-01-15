#pragma once


// CSweepDlg 对话框

class CSweepDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSweepDlg)

public:
	CSweepDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSweepDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_sweepDistance;
};
