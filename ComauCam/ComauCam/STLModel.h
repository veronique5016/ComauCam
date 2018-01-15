#pragma once
#include "GemoBase.h"
#include "Triangle.h"
#include "OpenGL.h"



class CSTLModel :public CObject
{
public:
	CSTLModel(void);
	~CSTLModel(void);

public:
	TriangleAry m_tris;

public:
	bool ReadSTL(CString sFilePath); // ��ȡģ���ж��Ƕ����ƻ���ASCII��ʽ
	bool ReadBinarySTL(CFile& file);
	bool ReadAsciiSTL(CStdioFile& file);
	CPoint3D FindCord(CString str);
	CVector3D FindNormal(CString str);
	BOOL IsNumber(TCHAR ch); // �Ƿ�������
	BOOL IsSpace(TCHAR ch);  // �Ƿ��ǿո�

	virtual void Draw(COpenGLDC* pDC);           // ��ͼ

};



