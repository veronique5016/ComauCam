#pragma once


// LayerDlg 对话框

class CLayerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLayerDlg)

public:
	CLayerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLayerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOWLAYERDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nStartLayer;
	int m_nEndLayer;
	int m_nNumberofLayers;
	int m_nFirstTurnLayer;
};
