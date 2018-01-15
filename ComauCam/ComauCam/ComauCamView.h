
// ComauCamView.h : CComauCamView ��Ľӿ�
//

#pragma once

#include "OpenGL.h"
#include "STLModel.h"
#include "Slice.h"

class CComauCamView : public CView
{
protected: // �������л�����
	CComauCamView();
	DECLARE_DYNCREATE(CComauCamView)

// ����
public:
	CComauCamDoc* GetDocument() const;
	CSTLModel* m_STLModel;
	CSlice* m_slice;
	bool m_bCanSTLDraw;
	bool m_ShowTriFace;
	bool m_bCanSliceDraw;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CComauCamView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	//-----------------------------------------------------
	//���ô������ظ�ʽ��MFC�дOpenGL������

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
	HGLRC  m_hRC;        //��ɫ������
	HPALETTE m_hPalette; //OpenGL��ɫ��
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
	CStringArray m_sIgesLines;
	CTypedPtrArray<CObArray, CSTLModel*> m_entities;

// ���ɵ���Ϣӳ�亯��
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
	afx_msg void OnTriangleFrame();
	afx_msg void OnTriangleFace();
	afx_msg void OnStartSlice();
};

#ifndef _DEBUG  // ComauCamView.cpp �еĵ��԰汾
inline CComauCamDoc* CComauCamView::GetDocument() const
   { return reinterpret_cast<CComauCamDoc*>(m_pDocument); }
#endif

