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
	bool ReadSTL(CString sFilePath); // ��ȡģ���ж��Ƕ����ƻ���ASCII��ʽ
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
	BOOL IsNumber(TCHAR ch); // �Ƿ�������
	BOOL IsSpace(TCHAR ch);  // �Ƿ��ǿո�
	CLVertex* SearchPtInVertices(const CPoint3D& pt);//�����ڰ�߽ṹ�Ķ���������Ѱ�������

	void RoundFour(CTriangle* pTri);
};



