#pragma once


// LayerDlg �Ի���

class CLayerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLayerDlg)

public:
	CLayerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLayerDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOWLAYERDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nStartLayer;
	int m_nEndLayer;
	int m_nNumberofLayers;
	int m_nFirstTurnLayer;
};
