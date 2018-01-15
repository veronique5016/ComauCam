
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

class CComauCamView : public CView
{
protected: // 仅从序列化创建
	CComauCamView();
	DECLARE_DYNCREATE(CComauCamView)

// 特性
public:
	CComauCamDoc* GetDocument() const;
	vector<CSTLModel*> m_models;
	vector<CSlice*> m_slices;
	vector<CSweep*> m_sweeps;
	bool m_bCanSTLDraw;
	bool m_bCanSliceDraw;
	bool m_bCanSweepDraw;

	bool m_ShowTriFace;
	bool m_ShowPolygon;

	int startLayer, endLayer;

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

	float		camRot[3];
	float		camPos[3];
	float		sceneRot[3];
	float		scenePos[3];
	double  m_width, m_height;
	BOOL mouserightdown;
	BOOL mouseleftdown;
	CPoint mouseprevpoint;
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



private:
//	CStringArray m_sIgesLines;
	CTypedPtrArray<CObArray, CSTLModel*> m_entities;

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
	afx_msg void OnWritegcode();
	afx_msg void OnDisplaymode();
	afx_msg void OnShowselectedlayer();
	afx_msg void OnFiveaxisgcode();
};

#ifndef _DEBUG  // ComauCamView.cpp 中的调试版本
inline CComauCamDoc* CComauCamView::GetDocument() const
   { return reinterpret_cast<CComauCamDoc*>(m_pDocument); }
#endif

