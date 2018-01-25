
// ComauCamView.h : CComauCamView 类的接口
//

#pragma once

#include "OpenGL.h"
#include "STLModel.h"
#include "Slice.h"
#include "Sweep.h"
#include "SliceDlg.h"
#include "SweepDlg.h"
#include "DisplayDlg.h"
#include "LayerDlg.h"
#include "GCode.h"
#include "TransferDlg.h"



class CComauCamView : public CView
{
protected: // 仅从序列化创建
	CComauCamView();
	DECLARE_DYNCREATE(CComauCamView)

// 特性
public:
	CComauCamDoc* GetDocument() const;
	vector<CSTLModel*> m_vpSTLModels;
	vector<CSlice*> m_vpSlices;
	vector<CSweep*> m_vpSweeps;
	CGCode* m_pGCode;

	bool m_bSTLDraw;
	bool m_bSliceDraw;
	bool m_bSweepDraw;

	bool m_bTriFaceDraw;
	bool m_bPolygonDraw;

	int m_nStartLayer, m_nEndLayer;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CComauCamView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	//-----------------------------------------------------
	//设置窗口像素格式（MFC中搭建OpenGL环境）

	float		m_fCamRot[3];
	float		m_fCamPos[3];
	float		m_fSceneRot[3];
	float		m_fScenePos[3];
	double  m_dWidth, m_dHeight;
	BOOL m_bIsMouseRightDown;
	BOOL m_bIsMouseLeftDown;
	CPoint m_ptMousePrevPoint;
	COpenGLDC* m_pDC;
	CDC* m_pDC1;
	HGLRC  m_hRC;        //着色描述表
	HPALETTE m_hPalette; //OpenGL调色板
	void SetLogicalPalette();
	BOOL SetPixelFormat();
	BOOL RenderScene();
	void Init();
	BOOL InitializeOpenGL(CDC* pDC1);

	void SetSceneTrans(int axis, int value, BOOL increment, BOOL apply);
	void SetSceneRot(int axis, int value, BOOL increment, BOOL apply);
	void SetCamPos(int axis, int value, BOOL increment, BOOL apply);
	void SetScenePos(int axis, int value, BOOL increment, BOOL apply);

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnStlopen();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDeletemodel();
	afx_msg void OnFileSave();

	afx_msg void OnStartSlice();
	afx_msg void OnSweep();
	afx_msg void OnWriteGCode();
	afx_msg void OnDisplayMode();
	afx_msg void OnShowSelectedLayers();
	afx_msg void OnFiveAxisGCode();
	afx_msg void OnMoveandRotate();
	afx_msg void OnCenterOn();
};

#ifndef _DEBUG  // ComauCamView.cpp 中的调试版本
inline CComauCamDoc* CComauCamView::GetDocument() const
   { return reinterpret_cast<CComauCamDoc*>(m_pDocument); }
#endif

