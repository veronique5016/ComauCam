
// ComauCamView.cpp : CComauCamView 类的实现
//

#include "stdafx.h"
#include "OpenGL.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "ComauCam.h"
#endif

#include "ComauCamDoc.h"
#include "ComauCamView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CComauCamView

IMPLEMENT_DYNCREATE(CComauCamView, CView)

BEGIN_MESSAGE_MAP(CComauCamView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CComauCamView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_OPEN, &CComauCamView::OnStlopen)
	ON_COMMAND(ID_STLOpen, &CComauCamView::OnStlopen)
	ON_COMMAND(ID_DeleteModel, &CComauCamView::OnDeletemodel)
	ON_COMMAND(ID_FILE_SAVE, &CComauCamView::OnFileSave)
	ON_COMMAND(ID_Start_Slice, &CComauCamView::OnStartSlice)
	ON_COMMAND(ID_Sweep, &CComauCamView::OnSweep)
	ON_COMMAND(ID_WriteGCode, &CComauCamView::OnWriteGCode)
	ON_COMMAND(ID_DISPLAYMODE, &CComauCamView::OnDisplayMode)
	ON_COMMAND(ID_SHOWSELECTEDLAYER, &CComauCamView::OnShowSelectedLayers)
	ON_COMMAND(ID_FIVEAXISGCODE, &CComauCamView::OnFiveAxisGCode)
	ON_COMMAND(ID_moveandRotate, &CComauCamView::OnMoveandRotate)
	ON_COMMAND(ID_ONCENTER, &CComauCamView::OnCenterOn)
END_MESSAGE_MAP()

// CComauCamView 构造/析构

CComauCamView::CComauCamView()
{
	// TODO: 在此处添加构造代码
	m_bSTLDraw = false;   
	m_bSliceDraw = false;
	m_bSweepDraw = false;

	m_bTriFaceDraw = false;
	m_bPolygonDraw = false;
	
	m_nStartLayer = 1;
	m_nEndLayer = 0;
}

CComauCamView::~CComauCamView()
{
}

BOOL CComauCamView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	//设置窗口类型
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	/*WS_CLIPCHILDREN父窗口不对子窗口区域进行绘制
	WS_CLIPSIBLINGS不对兄弟窗口进行绘制*/

	return CView::PreCreateWindow(cs);
}

// CComauCamView 绘制

void CComauCamView::OnDraw(CDC* /*pDC*/)
{
	CComauCamDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码

	RenderScene();
}


// CComauCamView 打印


void CComauCamView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CComauCamView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CComauCamView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CComauCamView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


void CComauCamView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CComauCamView 诊断

#ifdef _DEBUG
void CComauCamView::AssertValid() const
{
	CView::AssertValid();
}

void CComauCamView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CComauCamDoc* CComauCamView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CComauCamDoc)));
	return (CComauCamDoc*)m_pDocument;
}
#endif //_DEBUG


// CComauCamView 消息处理程序

void CComauCamView::SetCamPos(int axis, int value, BOOL increment, BOOL apply)
{
	if (increment)
	{
		m_fCamPos[axis] += (float)value*m_fCamPos[axis] / 100;
	}
	else
	{
		m_fCamPos[axis] = (float)value / 2;
	}

	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

	RenderScene();

}

void CComauCamView::SetScenePos(int axis, int value, BOOL increment, BOOL apply)
{
	if (increment)
	{
		m_fScenePos[axis] += (float)value*m_fScenePos[axis] / 100;
	}
	else
	{
		m_fScenePos[axis] = (float)value / 2;
	}

	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	RenderScene();
}

void CComauCamView::SetSceneTrans(int axis, int value, BOOL increment, BOOL apply)
{
	if (increment)
		m_fScenePos[axis] += value;
	else
		m_fScenePos[axis] -= value;

	RenderScene();
}

void CComauCamView::SetSceneRot(int axis, int value, BOOL increment, BOOL apply)
{
	if (increment)
		m_fSceneRot[axis] += (m_fSceneRot[axis] >= 360) ? (-360 + value / 2) : value / 2;
	else
		m_fSceneRot[axis] = (m_fSceneRot[axis] >= 360) ? (-360 + value / 2) : value / 2;

	RenderScene();
}


//////////////////////////////////////////////////////////
//						初始化openGL场景
//////////////////////////////////////////////////////////
BOOL CComauCamView::InitializeOpenGL(CDC* pDC)
{
	m_pDC1 = pDC;
	SetPixelFormat();
	//生成绘制描述表
	m_hRC = ::wglCreateContext(m_pDC1->GetSafeHdc());
	//设置当前绘制描述表
	::wglMakeCurrent(m_pDC1->GetSafeHdc(), m_hRC);
	return TRUE;
}



//////////////////////////////////////////////////////////
//						设置像素格式
//////////////////////////////////////////////////////////
BOOL CComauCamView::SetPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd = {

		sizeof(PIXELFORMATDESCRIPTOR),    // pfd结构的大小
		1,                                // 版本号
		PFD_DRAW_TO_WINDOW |              // 支持在窗口中绘图
		PFD_SUPPORT_OPENGL |              // 支持 OpenGL
		PFD_DOUBLEBUFFER,                 // 双缓存模式
		PFD_TYPE_RGBA,                    // RGBA 颜色模式
		24,                               // 24 位颜色深度
		0, 0, 0, 0, 0, 0,                 // 忽略颜色位
		0,                                // 没有非透明度缓存
		0,                                // 忽略移位位
		0,                                // 无累加缓存
		0, 0, 0, 0,                       // 忽略累加位
		32,                               // 32 位深度缓存
		0,                                // 无模板缓存
		0,                                // 无辅助缓存
		PFD_MAIN_PLANE,                   // 主层
		0,                                // 保留
		0, 0, 0                           // 忽略层,可见性和损毁掩模
	};
	int pixelformat;
	pixelformat = ::ChoosePixelFormat(m_pDC1->GetSafeHdc(), &pfd);//选择像素格式
	::SetPixelFormat(m_pDC1->GetSafeHdc(), pixelformat, &pfd);     //设置像素格式
	if (pfd.dwFlags & PFD_NEED_PALETTE)
		SetLogicalPalette(); //设置逻辑调色板
	return TRUE;

}



/////////////////////////////////////////////////////////////////////
//	                  设置逻辑调色板
//////////////////////////////////////////////////////////////////////
void CComauCamView::SetLogicalPalette()
{
	struct
	{
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} logicalPalette = { 0x300, 256 };

	BYTE reds[] = { 0, 36, 72, 109, 145, 182, 218, 255 };
	BYTE greens[] = { 0, 36, 72, 109, 145, 182, 218, 255 };
	BYTE blues[] = { 0, 85, 170, 255 };

	for (int colorNum = 0; colorNum<256; ++colorNum)
	{
		logicalPalette.aEntries[colorNum].peRed =
			reds[colorNum & 0x07];
		logicalPalette.aEntries[colorNum].peGreen =
			greens[(colorNum >> 0x03) & 0x07];
		logicalPalette.aEntries[colorNum].peBlue =
			blues[(colorNum >> 0x06) & 0x03];
		logicalPalette.aEntries[colorNum].peFlags = 0;
	}

	m_hPalette = CreatePalette((LOGPALETTE*)&logicalPalette);
}


//////////////////////////////////////////////////////////
//						场景绘制与渲染
//////////////////////////////////////////////////////////
BOOL CComauCamView::RenderScene()
{

	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//改变背景颜色
	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);


	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

	::glTranslatef(m_fCamPos[0], m_fCamPos[1], m_fCamPos[2]);    //三个参数分别表示了在三个坐标上的位移值
	::glRotatef(m_fCamRot[0], 1.0F, 0.0F, 0.0F);           //物体将绕着(0,0,0)到(x,y,z)的直线以逆时针旋转X
	::glRotatef(m_fCamRot[1], 0.0F, 1.0F, 0.0F);
	::glRotatef(m_fCamRot[2], 0.0F, 0.0F, 1.0F);

	::glPushMatrix();
	::glTranslatef(m_fScenePos[0], m_fScenePos[1], m_fScenePos[2]);
	::glRotatef(m_fSceneRot[0], 1.0F, 0.0F, 0.0F);
	::glRotatef(m_fSceneRot[1], 0.0F, 1.0F, 0.0F);
	::glRotatef(m_fSceneRot[2], 0.0F, 0.0F, 1.0F);

	//////
	m_pDC->DrawAxis();
	if (m_bSTLDraw)
	{
		for (int i = 0; i<m_vecpSTLModels.size(); i++)
		{
			m_pDC->DrawSTLModel(m_vecpSTLModels[i], m_bTriFaceDraw);
		}
	} 
	if (m_bSliceDraw)
	{
		for (unsigned int i = 0; i < m_vecpSlices.size(); i++)
		{
			if (m_nEndLayer == 0)
			{
				m_nEndLayer = m_vecpSlices[0]->m_vecpLayers.size();
			}
			m_pDC->DrawSliceModel(m_vecpSlices[0], m_bPolygonDraw, m_nStartLayer, m_nEndLayer);
		}
	}
	if (m_bSweepDraw)
	{
		for (unsigned int i = 0; i < m_vecpSweeps.size(); i++)
		{
			m_pDC->DrawSweepModel(m_vecpSweeps[0], m_nStartLayer, m_nEndLayer);
		}
	}

	::glPopMatrix();
	::SwapBuffers(m_pDC1->GetSafeHdc());		//交互缓冲区
	return TRUE;
}


//////////////////////////////////////////////////////////
//				openGL场景初始化参数
//////////////////////////////////////////////////////////
void CComauCamView::Init()
{
	///////////////////////////


	m_fCamPos[0] = 0.0f;
	m_fCamPos[1] = 0.0f;//-30.0f
	m_fCamPos[2] = -2000.0f;
	m_fCamRot[0] = -75.0f;
	m_fCamRot[1] = 0.0f;
	m_fCamRot[2] = 0.0f;
	m_fScenePos[0] = 0.0f;
	m_fScenePos[1] = 0.0f;
	m_fScenePos[2] = 0.0f;
	m_fSceneRot[0] = 0.0f;
	m_fSceneRot[1] = 0.0f;
	m_fSceneRot[2] = 0.0f;

	m_dWidth = 2000;
	m_dHeight = 2000;


	m_ptMousePrevPoint.x = 0;
	m_ptMousePrevPoint.y = 0;
	m_bIsMouseRightDown = FALSE;
	m_bIsMouseLeftDown = FALSE;



	::glShadeModel(GL_FLAT);

	::glClearColor(0.0F, 0.0F, 0.0F, 0.0F);

	::glClearDepth(1.0F);

	::glEnable(GL_DEPTH_TEST);

	::glEnable(GL_CULL_FACE);

	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat lightPos[] = { 6000.0f,6000.0f,6000.0f, 1.0f };

	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

}

void CComauCamView::OnStlopen()
{
	// TODO: Add your command handler code here
	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle = _T("打开STL模型");
	filedlg.m_ofn.lpstrFilter = _T("STL files(*.stl)\0*.stl\0ALL Files(*.*)\0*.*\0\0");
	if (IDOK == filedlg.DoModal())
	{
		CSTLModel* stlModel = new CSTLModel();
		CString str = _T(" ");
		str = filedlg.GetPathName();
		stlModel->ReadSTL(str);
		m_bSTLDraw = true;
		m_bTriFaceDraw = true;
		m_vecpSTLModels.push_back(stlModel);

		Invalidate(TRUE);
	}
}

int CComauCamView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pDC1 = new CClientDC(this);
	InitializeOpenGL(m_pDC1);   //设置为当前RC...产生绘制环境（RC），并使之成为当前的绘制环境
	Init();
	return 0;
}

void CComauCamView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	glViewport(0, 0, cx, cy);
	//////////////////////////////////
	GLdouble aspect_ratio;
	aspect_ratio = (GLdouble)cx / (GLdouble)cy;
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	gluPerspective(40.0F, aspect_ratio, 1.0F, 10000.0F);
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
}

void CComauCamView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	//删除调色板和渲染上下文、定时器
	::wglMakeCurrent(0, 0);
	::wglDeleteContext(m_hRC);
	if (m_hPalette)
		DeleteObject(m_hPalette);
	if (m_pDC1)
	{
		delete m_pDC1;
	}
}


//////////////////////////////////////////////////////////
//					鼠标响应函数
//////////////////////////////////////////////////////////
void CComauCamView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	m_bIsMouseLeftDown = FALSE;
//	SetSceneRot(0, (point.y - mouseprevpoint.y), TRUE, TRUE);
//	SetSceneRot(2, (point.x - mouseprevpoint.x), TRUE, TRUE);

	CView::OnLButtonUp(nFlags, point);
}

void CComauCamView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetCapture();
	m_bIsMouseLeftDown = TRUE;
//	mouseprevpoint.x = point.x;
//	mouseprevpoint.y = point.y;

	CView::OnLButtonDown(nFlags, point);
}

void CComauCamView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bIsMouseRightDown)
	{
		//SetCamPos(1, -(point.y - mouseprevpoint.y), TRUE, TRUE);
		SetSceneTrans(2, (point.y - m_ptMousePrevPoint.y), FALSE, TRUE);
		SetSceneTrans(0, (point.x - m_ptMousePrevPoint.x), TRUE, TRUE);

	}
	else if (m_bIsMouseLeftDown)
	{
		SetSceneRot(0, (point.y - m_ptMousePrevPoint.y), TRUE, TRUE);
		SetSceneRot(2, (point.x - m_ptMousePrevPoint.x), TRUE, TRUE);
	}
	CView::OnMouseMove(nFlags, point);

	m_ptMousePrevPoint.x = point.x;
	m_ptMousePrevPoint.y = point.y;

}

BOOL CComauCamView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (zDelta>0)
	{
		m_fCamPos[2] *= 0.8;
	}
	else
	{
		m_fCamPos[2] *= 1.25;
	}
	Invalidate(TRUE);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CComauCamView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetCapture();
	m_bIsMouseRightDown = TRUE;
	m_ptMousePrevPoint.x = point.x;
	m_ptMousePrevPoint.y = point.y;

	CView::OnRButtonDown(nFlags, point);
}

void CComauCamView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ReleaseCapture();
	m_bIsMouseRightDown = FALSE;
	SetCamPos(1, (point.y - m_ptMousePrevPoint.y), TRUE, TRUE);
	CView::OnRButtonUp(nFlags, point);
}

void CComauCamView::OnDeletemodel()
{
	// TODO: Add your command handler code here
	m_vecpSTLModels.clear();
	m_vecpSlices.clear();
	m_vecpSweeps.clear();

	m_bSTLDraw = false;
	m_bSliceDraw = false;
	m_bSweepDraw = false;

	m_bTriFaceDraw = false;
	m_bPolygonDraw = false;

	m_nStartLayer = 1;
	m_nEndLayer = 0;

	RenderScene();
}

void CComauCamView::OnFileSave()
{
	// TODO: Add your command handler code here
	TCHAR szFilter[] = _T("文本文件(*.txt) | *.txt | 所有文件(*.*) | *.* || ");
	CFileDialog fileDlg(FALSE, _T("doc"), _T("test"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString strFilePath;

	if (IDOK == fileDlg.DoModal())
	{
		strFilePath = fileDlg.GetPathName();
		m_vecpSTLModels[0]->WriteSTL(strFilePath);
		SetDlgItemText(ID_FILE_SAVE, strFilePath);
	}
}

void CComauCamView::OnStartSlice()
{
	// TODO: 在此添加命令处理程序代码
	CSliceDlg dialog;
	if (dialog.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		unsigned int szModel = m_vecpSTLModels.size();
		for (unsigned int i = 0; i < szModel; i++)
		{
			CSlice* pSlice = new CSlice();
			pSlice->m_dHeight = dialog.m_dSliceDistance;
			pSlice->LoadSTLModel(m_vecpSTLModels[i]);
			pSlice->Slice(m_vecpSTLModels[i]);
			m_vecpSlices.push_back(pSlice);
			m_bSliceDraw = true;
			m_bSTLDraw = false;
			Invalidate(TRUE);
		}
	}
}

void CComauCamView::OnSweep()
{
	// TODO: 在此添加命令处理程序代码
	CSweepDlg dialog;
	if (dialog.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		unsigned int szSlice = m_vecpSlices.size();
		for (unsigned int i = 0; i < szSlice; i++)
		{
			CSweep* pSweep = new CSweep();
			pSweep->m_dDistance = dialog.m_dSweepDistance;
			pSweep->LoadSliceModel(m_vecpSlices[i]);
			pSweep->Sweep();
			m_vecpSweeps.push_back(pSweep);
			m_bSweepDraw = true;
			m_bSTLDraw = false;
			m_bSliceDraw = false;
			Invalidate(TRUE);
		}
	}
}

void CComauCamView::OnWriteGCode()
{
	// TODO: 在此添加命令处理程序代码
	unsigned int sz = m_vecpSweeps.size();
	TCHAR szFilter[] = _T("文本文件(*.gcode) | *.gcode | 所有文件(*.*) | *.* || ");
	CFileDialog fileDlg(FALSE, _T(" "), _T("test"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString strFilePath;
	m_pGCode = new CGCode();
	if (IDOK == fileDlg.DoModal())
	{
		strFilePath = fileDlg.GetPathName();
		for (unsigned int i = 0; i < sz; i++)
		{
			m_pGCode->LoadSweepModel(m_vecpSweeps[i]);
			m_pGCode->Write(strFilePath);
		}
		SetDlgItemText(ID_WriteGCode, strFilePath);
	}
}

void CComauCamView::OnFiveAxisGCode()
{
	// TODO: 在此添加命令处理程序代码
	unsigned int sz = m_vecpSweeps.size();
	TCHAR szFilter[] = _T("文本文件(*.gcode) | *.gcode | 所有文件(*.*) | *.* || ");
	CFileDialog fileDlg(FALSE, _T(" "), _T("test"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString strFilePath;	
	m_pGCode = new CGCode();
	if (IDOK == fileDlg.DoModal())
	{
		strFilePath = fileDlg.GetPathName();
		for (unsigned int i = 0; i < sz; i++)
		{
			//m_sweeps[i]->writeFiveAxisGCode(strFilePath);
			
			m_pGCode->LoadSweepModel(m_vecpSweeps[i]);
			m_pGCode->WriteFiveAxis(strFilePath);
		}
		SetDlgItemText(ID_WriteGCode, strFilePath);
	}
}

void CComauCamView::OnDisplayMode()
{
	// TODO: 在此添加命令处理程序代码
	CDisplayDlg dialog;
	if (dialog.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		m_bTriFaceDraw = dialog.m_bTriangleDraw;
		m_bPolygonDraw = dialog.m_bPolygonDraw;
		m_bSTLDraw = dialog.m_bSTLModelDraw;
		m_bSliceDraw = dialog.m_bSliceModelDraw;
		m_bSweepDraw = dialog.m_bSweepModelDraw;
		Invalidate(TRUE);
	}
}

void CComauCamView::OnShowSelectedLayers()
{
	// TODO: 在此添加命令处理程序代码
	CLayerDlg dialog;
	UpdateData(FALSE);
	for (unsigned int i = 0; i < m_vecpSlices[0]->m_vecpLayers.size(); i++)
	{
		if (m_vecpSlices[0]->m_vecpLayers[i]->m_vLayerCoordinate[2].dz != 1.0)
		{
			dialog.m_nFirstTurnLayer = i;
			break;
		}
	}
	dialog.m_nNumberofLayers = m_vecpSlices[0]->m_vecpLayers.size();
	if (dialog.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		m_nStartLayer = dialog.m_nStartLayer;
		m_nEndLayer = dialog.m_nEndLayer;
		Invalidate(TRUE);
	}
}

void CComauCamView::OnMoveandRotate()
{
	// TODO: 在此添加命令处理程序代码
	TransferDlg dialog;
	if (dialog.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		double xmove = dialog.x_move;
		double ymove = dialog.y_move;
		double zmove = dialog.z_move;
		double angle = dialog.angle*PI / 180.0;
		double xaxis = dialog.x_axis;
		double yaxis = dialog.y_axis;
		double zaxis = dialog.z_axis;
		for (unsigned int i = 0; i < m_vecpSTLModels.size(); i++)
		{
			m_vecpSTLModels[i]->MoveModel(CVector3D(xmove, ymove, zmove));
			m_vecpSTLModels[i]->RotateModel(angle, CVector3D(xaxis, yaxis, zaxis));
		}
		Invalidate(TRUE);
	}
}

void CComauCamView::OnCenterOn()
{
	// TODO: 在此添加命令处理程序代码
	CVector3D offset_vec;
	for (unsigned int i = 0; i < m_vecpSTLModels.size(); i++)
	{
		offset_vec = m_vecpSTLModels[i]->OnCenter();
		m_vecpSTLModels[i]->MoveModel(offset_vec);
		RenderScene();
	}
}
