#pragma once


// TransferDlg 对话框

class TransferDlg : public CDialogEx
{
	DECLARE_DYNAMIC(TransferDlg)

public:
	TransferDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~TransferDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRANSFERDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double x_move;
	double y_move;
	double z_move;
	double angle;
	double x_axis;
	double y_axis;
	double z_axis;
};
