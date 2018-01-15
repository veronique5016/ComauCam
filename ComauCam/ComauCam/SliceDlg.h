#pragma once


// CSliceDlg 对话框

class CSliceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSliceDlg)

public:
	CSliceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSliceDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_sliceDistance;
};
