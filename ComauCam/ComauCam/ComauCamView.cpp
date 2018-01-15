
// ComauCamView.cpp : CComauCamView ���ʵ��
//

#include "stdafx.h"
#include "OpenGL.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "ComauCam.h"
#endif

#include "ComauCamDoc.h"
#include "ComauCamView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

vector<CSTLModel*> models;
vector<CSlice*> m_slices;
vector<CSweep*> m_sweeps;
// CComauCamView

IMPLEMENT_DYNCREATE(CComauCamView, CView)

BEGIN_MESSAGE_MAP(CComauCamView, CView)
	// ��׼��ӡ����
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
	ON_COMMAND(ID_Triangle_Frame, &CComauCamView::OnTriangleFrame)
	ON_COMMAND(ID_Triangle_Face, &CComauCamView::OnTriangleFace)
	ON_COMMAND(ID_Start_Slice, &CComauCamView::OnStartSlice)
	ON_COMMAND(ID_Sweep, &CComauCamView::OnSweep)
	ON_COMMAND(ID_WriteGCode, &CComauCamView::OnWritegcode)
END_MESSAGE_MAP()

// CComauCamView ����/����

CComauCamView::CComauCamView()
{
	// TODO: �ڴ˴���ӹ������
	m_bCanSTLDraw = false;
	m_STLModel = NULL;
	m_ShowTriFace = false;
	m_bCanSliceDraw = false;
	m_bCanSweepDraw = false;
	m_STLModel = NULL;
	m_slice = NULL;
	m_sweep = NULL;
}

CComauCamView::~CComauCamView()
{
	if (NULL != m_STLModel)
		delete[]m_STLModel;
	if (NULL != m_slice)
		delete[]m_slice;
	if (NULL != m_sweep)
		delete[]m_sweep;
}

BOOL CComauCamView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	//���ô�������
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	/*WS_CLIPCHILDREN�����ڲ����Ӵ���������л���
	WS_CLIPSIBLINGS�����ֵܴ��ڽ��л���*/

	return CView::PreCreateWindow(cs);
}

// CComauCamView ����

void CComauCamView::OnDraw(CDC* /*pDC*/)
{
	CComauCamDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���

	RenderScene();
}


// CComauCamView ��ӡ


void CComauCamView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CComauCamView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CComauCamView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CComauCamView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


void CComauCamView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CComauCamView ���

#ifdef _DEBUG
void CComauCamView::AssertValid() const
{
	CView::AssertValid();
}

void CComauCamView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CComauCamDoc* CComauCamView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CComauCamDoc)));
	return (CComauCamDoc*)m_pDocument;
}
#endif //_DEBUG


// CComauCamView ��Ϣ�������

void CComauCamView::SetCamPos(int axis, int value, BOOL increment, BOOL apply)
{
	if (increment)
	{
		camPos[axis] += (float)value*camPos[axis] / 100;
	}
	else
	{
		camPos[axis] = (float)value / 2;
	}

	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

	RenderScene();

}

void CComauCamView::SetScenePos(int axis, int value, BOOL increment, BOOL apply)
{
	if (increment)
	{
		scenePos[axis] += (float)value*scenePos[axis] / 100;
	}
	else
	{
		scenePos[axis] = (float)value / 2;
	}

	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	RenderScene();
}

void CComauCamView::SetSceneTrans(int axis, int value, BOOL increment, BOOL apply)
{
	if (increment)
		scenePos[axis] += value;
	else
		scenePos[axis] -= value;

	RenderScene();
}

void CComauCamView::SetSceneRot(int axis, int value, BOOL increment, BOOL apply)
{
	if (increment)
		sceneRot[axis] += (sceneRot[axis] >= 360) ? (-360 + value / 2) : value / 2;
	else
		sceneRot[axis] = (sceneRot[axis] >= 360) ? (-360 + value / 2) : value / 2;

	RenderScene();
}


//////////////////////////////////////////////////////////
//						��ʼ��openGL����
//////////////////////////////////////////////////////////
BOOL CComauCamView::InitializeOpenGL(CDC* pDC)
{
	m_pDC1 = pDC;
	SetPixelFormat();
	//���ɻ���������
	m_hRC = ::wglCreateContext(m_pDC1->GetSafeHdc());
	//���õ�ǰ����������
	::wglMakeCurrent(m_pDC1->GetSafeHdc(), m_hRC);
	return TRUE;
}



//////////////////////////////////////////////////////////
//						�������ظ�ʽ
//////////////////////////////////////////////////////////
BOOL CComauCamView::SetPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd = {

		sizeof(PIXELFORMATDESCRIPTOR),    // pfd�ṹ�Ĵ�С
		1,                                // �汾��
		PFD_DRAW_TO_WINDOW |              // ֧���ڴ����л�ͼ
		PFD_SUPPORT_OPENGL |              // ֧�� OpenGL
		PFD_DOUBLEBUFFER,                 // ˫����ģʽ
		PFD_TYPE_RGBA,                    // RGBA ��ɫģʽ
		24,                               // 24 λ��ɫ���
		0, 0, 0, 0, 0, 0,                 // ������ɫλ
		0,                                // û�з�͸���Ȼ���
		0,                                // ������λλ
		0,                                // ���ۼӻ���
		0, 0, 0, 0,                       // �����ۼ�λ
		32,                               // 32 λ��Ȼ���
		0,                                // ��ģ�建��
		0,                                // �޸�������
		PFD_MAIN_PLANE,                   // ����
		0,                                // ����
		0, 0, 0                           // ���Բ�,�ɼ��Ժ������ģ
	};
	int pixelformat;
	pixelformat = ::ChoosePixelFormat(m_pDC1->GetSafeHdc(), &pfd);//ѡ�����ظ�ʽ
	::SetPixelFormat(m_pDC1->GetSafeHdc(), pixelformat, &pfd);     //�������ظ�ʽ
	if (pfd.dwFlags & PFD_NEED_PALETTE)
		SetLogicalPalette(); //�����߼���ɫ��
	return TRUE;

}



/////////////////////////////////////////////////////////////////////
//	                  �����߼���ɫ��
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
//						������������Ⱦ
//////////////////////////////////////////////////////////
BOOL CComauCamView::RenderScene()
{

	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//�ı䱳����ɫ
	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);


	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

	::glTranslatef(camPos[0], camPos[1], camPos[2]);    //���������ֱ��ʾ�������������ϵ�λ��ֵ
	::glRotatef(camRot[0], 1.0F, 0.0F, 0.0F);           //���彫����(0,0,0)��(x,y,z)��ֱ������ʱ����תX
	::glRotatef(camRot[1], 0.0F, 1.0F, 0.0F);
	::glRotatef(camRot[2], 0.0F, 0.0F, 1.0F);

	::glPushMatrix();
	::glTranslatef(scenePos[0], scenePos[1], scenePos[2]);
	::glRotatef(sceneRot[0], 1.0F, 0.0F, 0.0F);
	::glRotatef(sceneRot[1], 0.0F, 1.0F, 0.0F);
	::glRotatef(sceneRot[2], 0.0F, 0.0F, 1.0F);


	//////
	m_pDC->DrawAxis();
	if (m_bCanSTLDraw)
	{
		for (int i = 0; i<m_entities.GetSize(); i++)
		{
			m_entities.GetAt(i)->Draw(m_pDC, m_ShowTriFace);
		}

	} 
	if (m_bCanSliceDraw)
	{ 
		double green[3];
		green[0] = 0.0;
		green[1] = 1.0;
		green[2] = 0.0;

		double red[3];
		red[0] = 1.0;
		red[1] = 0.0;
		red[2] = 0.0;

		m_slices[0]->drawpolyline(green);

	}
	if (m_bCanSweepDraw)
	{
		m_sweeps[0]->drawRoute();
	}
	// ondeletemodel �ͷ� DC 
	::glPopMatrix();
	::SwapBuffers(m_pDC1->GetSafeHdc());		//����������
	return TRUE;
}


//////////////////////////////////////////////////////////
//				openGL������ʼ������
//////////////////////////////////////////////////////////
void CComauCamView::Init()
{
	///////////////////////////


	camPos[0] = 0.0f;
	camPos[1] = 0.0f;//-30.0f
	camPos[2] = -2000.0f;
	camRot[0] = -75.0f;
	camRot[1] = 0.0f;
	camRot[2] = 0.0f;
	scenePos[0] = 0.0f;
	scenePos[1] = 0.0f;
	scenePos[2] = 0.0f;
	sceneRot[0] = 0.0f;
	sceneRot[1] = 0.0f;
	sceneRot[2] = 0.0f;

	m_width = 2000;
	m_height = 2000;


	mouseprevpoint.x = 0;
	mouseprevpoint.y = 0;
	mouserightdown = FALSE;
	mouseleftdown = FALSE;



	::glShadeModel(GL_FLAT);

	::glClearColor(0.0F, 0.0F, 0.0F, 0.0F);

	::glClearDepth(1.0F);

	::glEnable(GL_DEPTH_TEST);

	::glEnable(GL_CULL_FACE);

	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat lightPos[] = { 6000.0f,6000.0f,6000.0f, 1.0f };

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
	filedlg.m_ofn.lpstrTitle = _T("��STLģ��");
	filedlg.m_ofn.lpstrFilter = _T("STL files(*.stl)\0*.stl\0ALL Files(*.*)\0*.*\0\0");
	/*filedlg.m_ofn.lpstrDefExt=_T("txt");*/
	if (IDOK == filedlg.DoModal())
	{

		//vector<CSTLModel*> models;
		//CString str=filedlg.GetFileName();
		CString str = _T(" ");
		str = filedlg.GetPathName();
		m_STLModel->ReadSTL(str);
		m_bCanSTLDraw = true;
		m_ShowTriFace = true;
		models.push_back(m_STLModel);
		m_entities.Add(m_STLModel);   //��m_STLmodelָ�����m_entities
									  //models.push_back(m_STLmodel);
		Invalidate(TRUE);
	}
}


int CComauCamView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_STLModel = new CSTLModel();


	m_pDC1 = new CClientDC(this);
	InitializeOpenGL(m_pDC1);   //����Ϊ��ǰRC...�������ƻ�����RC������ʹ֮��Ϊ��ǰ�Ļ��ƻ���
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
	//ɾ����ɫ�����Ⱦ�����ġ���ʱ��
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
//					�����Ӧ����
//////////////////////////////////////////////////////////
void CComauCamView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	mouseleftdown = FALSE;
//	SetSceneRot(0, (point.y - mouseprevpoint.y), TRUE, TRUE);
//	SetSceneRot(2, (point.x - mouseprevpoint.x), TRUE, TRUE);

	CView::OnLButtonUp(nFlags, point);
}


void CComauCamView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	SetCapture();
	mouseleftdown = TRUE;
//	mouseprevpoint.x = point.x;
//	mouseprevpoint.y = point.y;

	CView::OnLButtonDown(nFlags, point);
}


void CComauCamView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (mouserightdown)
	{
		//SetCamPos(1, -(point.y - mouseprevpoint.y), TRUE, TRUE);
		SetSceneTrans(2, (point.y - mouseprevpoint.y), FALSE, TRUE);
		SetSceneTrans(0, (point.x - mouseprevpoint.x), TRUE, TRUE);

	}
	else if (mouseleftdown)
	{
		SetSceneRot(0, (point.y - mouseprevpoint.y), TRUE, TRUE);
		SetSceneRot(2, (point.x - mouseprevpoint.x), TRUE, TRUE);
	}
	CView::OnMouseMove(nFlags, point);

	mouseprevpoint.x = point.x;
	mouseprevpoint.y = point.y;

}


BOOL CComauCamView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (zDelta>0)
	{
		camPos[2] *= 0.8;
	}
	else
	{
		camPos[2] *= 1.25;
	}
	Invalidate(TRUE);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}



void CComauCamView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	SetCapture();
	mouserightdown = TRUE;
	mouseprevpoint.x = point.x;
	mouseprevpoint.y = point.y;

	CView::OnRButtonDown(nFlags, point);
}


void CComauCamView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ReleaseCapture();
	mouserightdown = FALSE;
	SetCamPos(1, (point.y - mouseprevpoint.y), TRUE, TRUE);
	CView::OnRButtonUp(nFlags, point);
}


void CComauCamView::OnDeletemodel()
{
	// TODO: Add your command handler code here
	m_STLModel->ReleaseMem();
	RenderScene();
}


void CComauCamView::OnFileSave()
{
	// TODO: Add your command handler code here
	TCHAR szFilter[] = _T("�ı��ļ�(*.txt) | *.txt | �����ļ�(*.*) | *.* || ");
	CFileDialog fileDlg(FALSE, _T("doc"), _T("test"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString strFilePath;

	if (IDOK == fileDlg.DoModal())
	{
		strFilePath = fileDlg.GetPathName();
		m_STLModel->WriteSTL(strFilePath);
		SetDlgItemText(ID_FILE_SAVE, strFilePath);
	}
}

void CComauCamView::OnTriangleFrame()
{
	// TODO: Add your command handler code here
	m_ShowTriFace = false;
	RenderScene();
}


void CComauCamView::OnTriangleFace()
{
	// TODO: Add your command handler code here
	m_ShowTriFace = true;
	RenderScene();
}


void CComauCamView::OnStartSlice()
{
	// TODO: �ڴ���������������
	CSliceDlg dialog;
	if (dialog.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		unsigned int szModel = models.size();
		for (unsigned int i = 0; i < szModel; i++)
		{
			CSlice* pSlice = new CSlice();
			pSlice->height = dialog.m_sliceDistance;
			pSlice->LoadSTLModel(models[i]);
			pSlice->slice(models[i]);
			m_slices.push_back(pSlice);
			m_bCanSliceDraw = true;
			//m_bCanSTLDraw = false;
			Invalidate(TRUE);
		}
	}
}


void CComauCamView::OnSweep()
{
	// TODO: �ڴ���������������
	CSweepDlg dialog;
	if (dialog.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		unsigned int szSlice = m_slices.size();
		for (unsigned int i = 0; i < szSlice; i++)
		{
			CSweep* pSweep = new CSweep();
			pSweep->distance = dialog.m_sweepDistance;
			pSweep->loadSliceModel(m_slices[i]);
			pSweep->sweep();
			m_sweeps.push_back(pSweep);
			m_bCanSweepDraw = true;
			m_bCanSTLDraw = false;
			m_bCanSliceDraw = false;
			Invalidate(TRUE);
		}
	}
}


void CComauCamView::OnWritegcode()
{
	// TODO: �ڴ���������������
	unsigned int sz = m_sweeps.size();
	TCHAR szFilter[] = _T("�ı��ļ�(*.gcode) | *.gcode | �����ļ�(*.*) | *.* || ");
	CFileDialog fileDlg(FALSE, _T(" "), _T("test"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString strFilePath;

	if (IDOK == fileDlg.DoModal())
	{
		strFilePath = fileDlg.GetPathName();
		for (unsigned int i = 0; i < sz; i++)
		{
			m_sweeps[i]->writeGCode(strFilePath);
		}
		SetDlgItemText(ID_WriteGCode, strFilePath);
	}

}
