#pragma once
#include "afx.h"
#include "GeomBase.h"

class CTriangle :
	public CObject
{
public:
	CTriangle(void);
	~CTriangle(void);
	CTriangle(const CTriangle& triangleFace);
	CTriangle(CPoint3D pt_A, CPoint3D pt_B, CPoint3D pt_C);
	CTriangle(CVector3D vecNormal, CPoint3D ptA, CPoint3D ptB, CPoint3D ptC);

public:
	const CTriangle& operator=(const CTriangle& triangleFace);

public:
	void GenerateNormal();  // 生成法向量
	void Rotate(double angle, CVector3D axis);
	void Move(CVector3D vec);

public:
	CPoint3D A, B, C;
	CVector3D Normal;
};
