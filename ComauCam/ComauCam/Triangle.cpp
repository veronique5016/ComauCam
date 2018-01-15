#include "StdAfx.h"
#include "Triangle.h"

CTriangle::CTriangle(void)
{
}

CTriangle::~CTriangle(void)
{
}

CTriangle::CTriangle(const CTriangle& triangleFace)
{
	A = triangleFace.A;
	B = triangleFace.B;
	C = triangleFace.C;
	Normal = triangleFace.Normal;
}

CTriangle::CTriangle(CPoint3D pt_A, CPoint3D pt_B, CPoint3D pt_C)
{
	A = pt_A;
	B = pt_B;
	C = pt_C;
}


const CTriangle& CTriangle::operator = (const CTriangle& triangleFace)
{
	A = triangleFace.A;
	B = triangleFace.B;
	C = triangleFace.C;
	Normal = triangleFace.Normal;

	return *this;
}

CTriangle::CTriangle(CVector3D vecNormal, CPoint3D ptA, CPoint3D ptB, CPoint3D ptC)
{
	A = ptA;
	B = ptB;
	C = ptC;
	Normal = vecNormal;
}

void CTriangle::GenerateNormal()
{
	Normal = (B - A)*(C - B);
	Normal.Normalize();
}

//void CTriangle::DrawTriangleFace(COpenGLDC* pDC)
//{
//	pDC->DrawTriChip(Normal,A,B,C);
//}
//
//void CTriangle::DrawTriangleFrame(COpenGLDC* pDC)
//{
//	pDC->DrawTriFrame(A,B,C);
//}

void CTriangle::Rotate(double angle, CVector3D axis)
{
	CMatrix3D matrix_roate = CMatrix3D::CreateRotateMatrix(angle, axis);

	A *= matrix_roate;
	B *= matrix_roate;
	C *= matrix_roate;

	Normal = (B - A)*(C - B);
	Normal.Normalize();
}

void CTriangle::Move(CVector3D vec)
{
	CMatrix3D matric = CMatrix3D::CreateTransferMatrix(vec);
	A *= matric;
	B *= matric;
	C *= matric;
}



void CTriangle::DrawTriangleFace(COpenGLDC* pDC)
{
	pDC->DrawTriChip(Normal, A, B, C);
}

void CTriangle::DrawTriangleFrame(COpenGLDC* pDC)
{
	pDC->DrawTriFrame(A, B, C);
}