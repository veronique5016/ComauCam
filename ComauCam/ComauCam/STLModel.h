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
	bool ReadSTL(CString sFilePath); // 读取模型判断是二进制还是ASCII格式
	bool ReadBinarySTL(CFile& file);
	bool ReadAsciiSTL(CStdioFile& file);
	CPoint3D FindCord(CString str);
	CVector3D FindNormal(CString str);
	BOOL IsNumber(TCHAR ch); // 是否是数字
	BOOL IsSpace(TCHAR ch);  // 是否是空格

	virtual void Draw(COpenGLDC* pDC);           // 绘图

};



