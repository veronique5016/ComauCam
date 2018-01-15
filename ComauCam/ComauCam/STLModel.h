#pragma once
#include "GeomBase.h"
#include "Triangle.h"
#include "OpenGL.h"
#include "LVertex.h"

#include <algorithm>
#include <vector>



class CSTLModel :public CObject, public TopologySTL
{
public:
	CSTLModel();
	~CSTLModel();

public:
	TriangleAry m_tris;

public:
	bool ReadSTL(CString sFilePath); // ��ȡģ���ж��Ƕ����ƻ���ASCII��ʽ
	void WriteSTL(CString sFilePath);
	void ReleaseMem();
	void Topologize();
	void FindExtreme(double ext[]);
	virtual void Draw(COpenGLDC* pDC, bool ShowTri);           // ��ͼ

public:
	bool ReadBinarySTL(CFile& file);
	bool ReadAsciiSTL(CStdioFile& file);
	CPoint3D FindCord(CString str);
	CVector3D FindNormal(CString str);
	BOOL IsNumber(TCHAR ch); // �Ƿ�������
	BOOL IsSpace(TCHAR ch);  // �Ƿ��ǿո�
	LVertex* SearchPtInVertices(const CPoint3D& pt);//�����ڰ�߽ṹ�Ķ���������Ѱ�������

	void RoundFour(CTriangle* pTri);
};



