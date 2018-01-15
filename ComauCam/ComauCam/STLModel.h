#pragma once

#include "GeomBase.h"
#include "Triangle.h"
#include "LVertex.h"
#include <algorithm>
#include <vector>


class CSTLModel :public CObject, public CTopologySTL
{
public:
	CSTLModel();
	~CSTLModel();

public:
	vector<CTriangle*> m_vecpTris;

public:
	bool ReadSTL(CString sFilePath); // 读取模型判断是二进制还是ASCII格式
	void WriteSTL(CString sFilePath);
	void ReleaseMem();
	void Topologize();
	void FindExtreme(double ext[]);
	void MoveModel(CVector3D vec);
	void RotateModel(double angle, CVector3D vec);
	CVector3D OnCenter();
	double ReturnZmax(const CTriangle* Ltri);
	double ReturnZmin(const CTriangle* Ltri);


public:
	bool ReadBinarySTL(CFile& file);
	bool ReadAsciiSTL(CStdioFile& file);
	CPoint3D FindCord(CString str);
	CVector3D FindNormal(CString str);
	BOOL IsNumber(TCHAR ch); // 是否是数字
	BOOL IsSpace(TCHAR ch);  // 是否是空格
	CLVertex* SearchPtInVertices(const CPoint3D& pt);//还是在半边结构的顶点数组中寻找这个点

	void RoundFour(CTriangle* pTri);
};



