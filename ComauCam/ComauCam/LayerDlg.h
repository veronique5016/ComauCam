#pragma once


// LayerDlg �Ի���

class LayerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LayerDlg)

public:
	LayerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LayerDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOWLAYERDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int startLayer;
	int endLayer;
	int numberofLayers;
	int firstTurnLayer;
};
