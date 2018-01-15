#pragma once


// LayerDlg 对话框

class LayerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LayerDlg)

public:
	LayerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~LayerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOWLAYERDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int startLayer;
	int endLayer;
	int numberofLayers;
	int firstTurnLayer;
};
