// CadBase.cpp: implementation of the CadBase class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GeomBase.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////
// 点
//////////////////////////////////////////////////////////////////////

CPoint3D::CPoint3D()
{
	x = y = z = 0;
	p_prev = p_next = NULL;
}

CPoint3D::CPoint3D(const double& ix, const double& iy, const double& iz)
{
	x = ix;
	y = iy;
	z = iz;
}

CPoint3D::CPoint3D(const double* p)
{
	x = p[0];
	y = p[1];
	z = p[2];
}

CPoint3D::CPoint3D(const CPoint3D& pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
}

CPoint3D::~CPoint3D()
{
}

const CPoint3D& CPoint3D::operator=(const CPoint3D& pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;

	return *this;
}

CPoint3D CPoint3D::operator+(const CVector3D& v) const
{
	return CPoint3D(x + v.dx, y + v.dy, z + v.dz);
}

CPoint3D CPoint3D::operator+(const CPoint3D& pt) const
{
	return CPoint3D(x + pt.x, y + pt.y, z + pt.z);
}

CPoint3D CPoint3D::operator *(const double& dFactor) const
{
	return CPoint3D(x*dFactor, y*dFactor, z*dFactor);
}

void CPoint3D::operator+=(const CVector3D& v)
{
	x += v.dx;
	y += v.dy;
	z += v.dz;
}

CPoint3D CPoint3D::operator-(const CVector3D& v) const
{
	return CPoint3D(x - v.dx, y - v.dy, z - v.dz);
}

void CPoint3D::operator-=(const CVector3D& v)                 // point and vector operation
{
	x -= v.dx;
	y -= v.dy;
	z -= v.dz;
}

CVector3D CPoint3D::operator-(const CPoint3D& pt) const       // point minus point to get vector
{
	return CVector3D(x - pt.x, y - pt.y, z - pt.z);
}

BOOL CPoint3D::operator==(const CPoint3D& pt) const  // Complete the same 
{
	if (x != pt.x) return FALSE;
	if (y != pt.y) return FALSE;
	if (z != pt.z) return FALSE;
	return TRUE;
}

BOOL CPoint3D::operator^=(const CPoint3D& pt) const  // More or less the same
{
	CVector3D vec = CVector3D(x - pt.x, y - pt.y, z - pt.z);
	double len = vec.GetLengthSquare();
	if (len <= POINT_EQUAL_ERROR)
		return TRUE;
	else
		return FALSE;
}

BOOL CPoint3D::operator!=(const CPoint3D& pt) const
{
	return !(*this == pt);
}

CPoint3D CPoint3D::operator*(const CMatrix3D& matrix) const
{
	double rx, ry, rz, sc;
	rx = x*matrix.A[0][0] + y*matrix.A[1][0] + z*matrix.A[2][0] + matrix.A[3][0];
	ry = x*matrix.A[0][1] + y*matrix.A[1][1] + z*matrix.A[2][1] + matrix.A[3][1];
	rz = x*matrix.A[0][2] + y*matrix.A[1][2] + z*matrix.A[2][2] + matrix.A[3][2];
	sc = x*matrix.A[0][3] + y*matrix.A[1][3] + z*matrix.A[2][3] + matrix.A[3][3];
	rx /= sc;
	ry /= sc;
	rz /= sc;
	return CPoint3D(rx, ry, rz);
}

void CPoint3D::operator*=(const CMatrix3D& matrix)            // point by matrix to get point   
{
	(*this) = (*this)*matrix;
}



//////////////////////////////////////////////////////////////////////
// 向量
//////////////////////////////////////////////////////////////////////

CVector3D::CVector3D()
{
	dx = dy = dz = 0;
}

CVector3D::~CVector3D()
{

}

CVector3D::CVector3D(const double& ix, const double& iy, const double& iz)
{
	dx = ix;
	dy = iy;
	dz = iz;
}

CVector3D::CVector3D(const double* p)
{
	dx = p[0];
	dy = p[1];
	dz = p[2];
}

CVector3D::CVector3D(const CVector3D& v)
{
	dx = v.dx;
	dy = v.dy;
	dz = v.dz;
}
CVector3D::CVector3D(const CPoint3D p1, const CPoint3D p2)
{
	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	dz = p2.z - p1.z;
}
const CVector3D& CVector3D::operator=(const CVector3D& v)
{
	dx = v.dx;
	dy = v.dy;
	dz = v.dz;
	return *this;
}

BOOL CVector3D::operator==(const CVector3D& v) const
{
	if (dx == v.dx && dy == v.dy && dz == v.dz)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

CVector3D CVector3D::operator+(const CVector3D& v) const
{
	return CVector3D(dx + v.dx, dy + v.dy, dz + v.dz);
}

void CVector3D::operator+=(const CVector3D& v)
{
	dx += v.dx;
	dy += v.dy;
	dz += v.dz;
}

CVector3D CVector3D::operator-(const CVector3D& v) const
{
	return CVector3D(dx - v.dx, dy - v.dy, dz - v.dz);
}

void CVector3D::operator-=(const CVector3D& v)
{
	dx -= v.dx;
	dy -= v.dy;
	dz -= v.dz;
}

CVector3D CVector3D::operator -()
{
	return CVector3D(-dx, -dy, -dz);
}

CVector3D CVector3D::operator*(const double& d) const
{
	return CVector3D(dx*d, dy*d, dz*d);
}

void CVector3D::operator*=(const double& d)
{
	dx = dx*d;
	dy = dy*d;
	dz = dz*d;
}

CVector3D CVector3D::operator/(const double& d) const
{
	return CVector3D(dx / d, dy / d, dz / d);
}

void CVector3D::operator/=(const double& d)
{
	dx = dx / d;
	dy = dy / d;
	dz = dz / d;
}

CVector3D CVector3D::operator*(const CVector3D& v) const    //叉乘
{
	return CVector3D(dy*v.dz - dz*v.dy, dz*v.dx - dx*v.dz, dx*v.dy - dy*v.dx);
}

double CVector3D::operator|(const CVector3D& v) const      //点乘
{
	return dx*v.dx + dy*v.dy + dz*v.dz;
}

CVector3D CVector3D::operator*(const CMatrix3D& matrix) const
{
	double rx, ry, rz, sc;
	rx = dx*matrix.A[0][0] + dy*matrix.A[1][0] + dz*matrix.A[2][0] + matrix.A[3][0];
	ry = dx*matrix.A[0][1] + dy*matrix.A[1][1] + dz*matrix.A[2][1] + matrix.A[3][1];
	rz = dx*matrix.A[0][2] + dy*matrix.A[1][2] + dz*matrix.A[2][2] + matrix.A[3][2];
	sc = dx*matrix.A[0][3] + dy*matrix.A[1][3] + dz*matrix.A[2][3] + matrix.A[3][3];
	rx /= sc;
	ry /= sc;
	rz /= sc;
	return CVector3D(rx, ry, rz);
}

void CVector3D::operator*=(const CMatrix3D& matrix)
{
	(*this) = (*this)*matrix;
}

double CVector3D::GetLength() const
{
	return sqrt(dx*dx + dy*dy + dz*dz);
}

double CVector3D::GetLengthSquare() const
{
	return dx*dx + dy*dy + dz*dz;
}
double CVector3D::GetMod2() const
{
	return dx*dx + dy*dy + dz*dz;
}

double CVector3D::GetLengthXY() const
{
	return sqrt(dx*dx + dy*dy);
}

double CVector3D::GetLengthYZ() const
{
	return sqrt(dy*dy + dz*dz);
}

double CVector3D::GetLengthZX() const
{
	return sqrt(dz*dz + dx*dx);
}

CVector3D CVector3D::GetNormal() const
{
	double length;
	length = this->GetLength();
	return CVector3D(dx / length, dy / length, dz / length);
}

void CVector3D::Normalize()//将向量单位化
{
	double length;
	length = this->GetLength();//得到向量的模
	if (0 != length)
	{
		dx /= length;
		dy /= length;
		dz /= length;
	}
	else
	{
		dx = 0;
		dy = 0;
		dz = 0;
	}
}

double CVector3D::GetAngle(CVector3D v1, CVector3D v2) const
{
	v1.Normalize();
	v2.Normalize();
	double dotProduct = v1 | v2;
	if (dotProduct>1)
		dotProduct = 1;
	if (dotProduct<-1)
		dotProduct = -1;

	return acos(v1 | v2);
}

BOOL CVector3D::IsZeroLength() const
{
	if (0 == this->GetLength())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//////////////////////////////////////////////////////////////////////
// 矩阵
//////////////////////////////////////////////////////////////////////

CMatrix3D::CMatrix3D()
{
	for (int i = 0; i<4; i++)
	{
		for (int j = 0; j<4; j++)
		{
			this->A[i][j] = 0;
		}
	}
	for (int i = 0; i<4; i++)
		this->A[i][i] = 1;
}

CMatrix3D::~CMatrix3D()
{

}

CMatrix3D::CMatrix3D(const CMatrix3D& matrix)
{
	for (int i = 0; i<4; i++)
	{
		for (int j = 0; j<4; j++)
		{
			A[i][j] = matrix.A[i][j];
		}
	}
}

CMatrix3D::CMatrix3D(const double* p)
{
	for (int i = 0; i<4; i++)
	{
		for (int j = 0; i<4; j++)
		{
			A[i][j] = p[i * 4 + j];
		}
	}
}

const CMatrix3D& CMatrix3D::operator=(const CMatrix3D& matrix)
{
	for (int i = 0; i<4; i++)
	{
		for (int j = 0; j<4; j++)
		{
			A[i][j] = matrix.A[i][j];
		}
	}
	return *this;
}

CMatrix3D CMatrix3D::operator*(const CMatrix3D& matrix) const
{
	CMatrix3D matrixTemp;
	for (int i = 0; i<4; i++)
	{
		for (int j = 0; j<4; j++)
		{
			matrixTemp.A[i][j] = A[i][0] * matrix.A[0][j]
				+ A[i][1] * matrix.A[1][j]
				+ A[i][2] * matrix.A[2][j]
				+ A[i][3] * matrix.A[3][j];
		}
	}
	return matrixTemp;
}

void CMatrix3D::operator*=(const CMatrix3D& matrix)
{
	(*this) = (*this)*matrix;
}


void CMatrix3D::IdenticalMatrix()
{
	for (int i = 0; i<4; i++)
	{
		for (int j = 0; j<4; j++)
		{
			this->A[i][j] = 0;
		}
	}
	for (int i = 0; i<4; i++)
		this->A[i][i] = 1;
}

double CMatrix3D::GetValue() const
{
	return A[0][0] * A[1][1] * A[2][2] +
		A[0][1] * A[1][2] * A[2][0] +
		A[0][2] * A[1][0] * A[2][1] +
		A[0][2] * A[1][1] * A[2][0] -
		A[0][1] * A[1][0] * A[2][2] -
		A[0][0] * A[1][2] * A[2][1];
}

double CMatrix3D::GetValue(const double& a00, const double& a01, const double& a02,
	const double& a10, const double& a11, const double& a12,
	const  double& a20, const double& a21, const double& a22)
{
	return  a00*a11*a22 +
		a01*a12*a20 +
		a02*a10*a21 +
		a02*a11*a20 -
		a01*a10*a22 -
		a00*a12*a21;
}

CMatrix3D CMatrix3D::CreateMirrorMatrix(CVector3D planeNorm)
{
	planeNorm.Normalize();
	CVector3D z(0, 0, 1);
	CVector3D b;
	b = planeNorm*z;
	double alphe;
	alphe = acos(planeNorm | z);

	CMatrix3D m1 = CMatrix3D::CreateRotateMatrix(alphe, b);
	CMatrix3D mXOY;
	mXOY.A[2][2] = -1;
	CMatrix3D m2 = CMatrix3D::CreateRotateMatrix(-alphe, b);

	CMatrix3D m;
	m = m1*mXOY*m2;
	return m;
}

CMatrix3D CMatrix3D::CreateRotateMatrix(const double& angle, CVector3D axis)
{
	CMatrix3D m;
	if (0 == angle)
		return m;

	if (0 == axis.GetLength())
		return m;

	axis.Normalize();
	double cosa = cos(angle);
	double sina = sin(angle);

	if (CVector3D(1, 0, 0) == axis)
	{
		m.A[1][1] = cosa;
		m.A[1][2] = sina;
		m.A[2][1] = -sina;
		m.A[2][2] = cosa;  //x
		return m;
	}

	if (CVector3D(0, 1, 0) == axis)
	{
		m.A[0][0] = cosa;
		m.A[0][2] = -sina;
		m.A[2][0] = sina;
		m.A[2][2] = cosa;  //y
		return m;
	}

	if (CVector3D(0, 0, 1) == axis)
	{
		m.A[0][0] = cosa;
		m.A[0][1] = sina;
		m.A[1][0] = -sina;
		m.A[1][1] = cosa;  //z
		return m;
	}

	double minuscosa = 1 - cosa;
	m.A[0][0] = cosa + minuscosa*axis.dx*axis.dx;
	m.A[1][0] = minuscosa*axis.dx*axis.dy - axis.dz*sina;
	m.A[2][0] = minuscosa*axis.dx*axis.dz + axis.dy*sina;
	m.A[0][1] = minuscosa*axis.dx*axis.dy + axis.dz*sina;
	m.A[1][1] = minuscosa*axis.dy*axis.dy + cosa;
	m.A[2][1] = minuscosa*axis.dy*axis.dz - axis.dx*sina;
	m.A[0][2] = minuscosa*axis.dx*axis.dz - axis.dy*sina;
	m.A[1][2] = minuscosa*axis.dy*axis.dz + axis.dx*sina;
	m.A[2][2] = minuscosa*axis.dz*axis.dz + cosa;

	return m;
}
CMatrix3D CMatrix3D::CreateRotateMatrix(const double &angle, int nAxis)
{
	CMatrix3D m;
	double cosa = cos(angle);
	double sina = sin(angle);

	if (AXIS_X == nAxis)
	{
		m.A[1][1] = cosa;
		m.A[1][2] = sina;
		m.A[2][1] = -sina;
		m.A[2][2] = cosa;  //x
		return m;
	}
	else if (AXIS_Y == nAxis)
	{
		m.A[0][0] = cosa;
		m.A[0][2] = -sina;
		m.A[2][0] = sina;
		m.A[2][2] = cosa;  //y
		return m;
	}
	else if (AXIS_Z == nAxis)
	{
		m.A[0][0] = cosa;
		m.A[0][1] = sina;
		m.A[1][0] = -sina;
		m.A[1][1] = cosa;  //z
		return m;
	}
	else
	{
		return m;
	}
}

CMatrix3D CMatrix3D::CreateScaleMatrix(const double& xyzAxis)
{
	CMatrix3D m;
	m.A[0][0] = m.A[1][1] = m.A[2][2] = xyzAxis;
	return m;
}

CMatrix3D CMatrix3D::CreateScaleMatrix(const double& xAxis, const double& yAxis, const double& zAxis)
{
	CMatrix3D m;
	m.A[0][0] = xAxis;
	m.A[1][1] = yAxis;
	m.A[2][2] = zAxis;
	return m;
}

CMatrix3D CMatrix3D::CreateTransferMatrix(const CVector3D& v)
{
	CMatrix3D m;
	m.A[3][0] = v.dx;
	m.A[3][1] = v.dy;
	m.A[3][2] = v.dz;
	return m;
}


//////////////////////////////////////////////////////////////////////
// 输出函数
//////////////////////////////////////////////////////////////////////

double GetAngle(CVector3D v1, CVector3D v2)
{
	v1.Normalize();
	v2.Normalize();
	return acos(v1 | v2);
}
double Angle(const CVector3D& v1, const CVector3D& v2)
{
	double dotProduct;//点乘
	dotProduct = v1 | v2;
	if (dotProduct>1) dotProduct = 1;
	if (dotProduct<-1) dotProduct = -1;
	return acos(dotProduct);
}
double GetDistance(const CPoint3D& pt1, const CPoint3D& pt2)
{
	CVector3D vec = pt1 - pt2;
	return vec.GetLength();
}

BOOL IsParallel(const CVector3D& v1, const CVector3D& v2)
{
	if (0.0 == (v1*v2).dz)
		return TRUE;
	else
		return FALSE;
}

BOOL IsParallel(const CVector3D* v1, const CVector3D* v2)
{
	if (0.0 == (v1->dx*v2->dy - v1->dy*v2->dx))
		return TRUE;
	else
		return FALSE;
}

BOOL IsOrhogonal(const CVector3D& v1, const CVector3D& v2)
{
	if (0.0 == (v1 | v2))
		return TRUE;
	else
		return FALSE;
}

double Round(const double& d, const int& iHowManyAfterDot)  // 保留5位小数
{
	if (5 == iHowManyAfterDot)
	{
		int i = (int)((d + 0.000005) * 100000);
		return (double)i / (double)100000;
	}
	else
	{
		CString str;

		str = _T("1");
		for (int i = 0; i<iHowManyAfterDot; i++)
		{
			str += _T("0");
		}
		int t = _wtoi(str);

		str = _T("0.5");
		for (int i = 0; i<iHowManyAfterDot; i++)
		{
			str.Insert(2, '0');
		}
		double dot = _wtof(str);

		int i = (int)((d + dot)*t);
		return (double)i / (double)t;
	}
}

BOOL Is2SegsCross(const CPoint3D& ptS_seg1, const CPoint3D& ptE_seg1, const CPoint3D& ptS_seg2, const CPoint3D& ptE_seg2) // 两条直线重合也是干涉的
{   // 干涉返回TURE
	CPoint3D ptCross;
	int state = GetCrossPoint(ptCross, ptS_seg1, ptE_seg1, ptS_seg2, ptE_seg2);
	if (0 == state) return FALSE;// 0平行，1重合，2相交
	if (1 == state) return TRUE;

	CVector3D vec1, vec2, vec3, vec4;
	vec1 = ptS_seg1 - ptCross;
	vec2 = ptE_seg1 - ptCross;
	vec3 = ptS_seg2 - ptCross;
	vec4 = ptE_seg2 - ptCross;
	if ((vec1 | vec2) <= 0 && (vec3 | vec4) <= 0)
		return TRUE;
	else
		return FALSE;
}

BOOL Is2SegsCross(CPoint3D& ptCross, const CPoint3D& ptS_seg1, const CPoint3D& ptE_seg1, const CPoint3D& ptS_seg2, const CPoint3D& ptE_seg2) // 两条直线重合也是干涉的
{   // 干涉返回TURE
	int state = GetCrossPoint(ptCross, ptS_seg1, ptE_seg1, ptS_seg2, ptE_seg2);
	if (0 == state) return FALSE;// 0平行，1重合，2相交
	if (1 == state) return TRUE;

	CVector3D vec1, vec2, vec3, vec4;
	vec1 = ptS_seg1 - ptCross;
	vec2 = ptE_seg1 - ptCross;
	vec3 = ptS_seg2 - ptCross;
	vec4 = ptE_seg2 - ptCross;
	if ((vec1 | vec2) <= 0 && (vec3 | vec4) <= 0)
		return TRUE;
	else
		return FALSE;
}

int GetCrossPoint(CPoint3D& pt_out, const CPoint3D& ptS_seg1, const CPoint3D& ptE_seg1, const CPoint3D& ptS_seg2, const CPoint3D& ptE_seg2) // 对于两条相交直线求交点
{
	// 0平行，1重合，2相交  
	// 2011/5/21 修改了两条直线求交点的函数，原来使用的是两直线方程，问题在于当直线竖直时会出现问题。现在用向量的方法
	CVector3D v1 = ptE_seg1 - ptS_seg1;
	CVector3D v2 = ptE_seg2 - ptS_seg2;
	if (0 == (v1*v2).dz)
	{
		CVector3D v3 = ptE_seg1 - ptE_seg2;
		if (0 == (v3*v1).dz) return 1;
		else return 0;
	}
	double lamda = ((ptS_seg2.x - ptS_seg1.x)*v2.dy - (ptS_seg2.y - ptS_seg1.y)*v2.dx) / (v1.dx*v2.dy - v1.dy*v2.dx);
	pt_out = ptS_seg1 + v1*lamda;
	return 2;
}

CVector3D GetNormalVector(const CPoint3D& ptS, const CPoint3D& ptE)
{
	CVector3D vec = ptE - ptS;
	CVector3D vec_r;
	if (0 == vec.dx)
		vec_r = CVector3D(1, 0, 0);
	else
		vec_r = CVector3D(-(vec.dy / vec.dx), 1, 0);

	if ((vec_r*vec).dz>0)  // 指向左边的
		vec_r = vec_r*(-1);  // 减小计算量 2011/4/26

	return vec_r;
}

CVector3D GetNormalVector(const CVector3D& vec)
{
	CVector3D vec_r;
	if (0 == vec.dx)
		vec_r = CVector3D(1, 0, 0);
	else
		vec_r = CVector3D(-(vec.dy / vec.dx), 1, 0);

	if ((vec_r*vec).dz>0)
		vec_r = vec_r*(-1);

	return vec_r;
}

double CalDis2PtToSeg(const CPoint3D& pt, const CPoint3D& ptS_seg, const CPoint3D& ptE_seg)
{
	if (ptS_seg == ptE_seg) return (pt - ptS_seg).GetLengthSquare();

	double lamda;
	CVector3D vec_P1P = pt - ptS_seg;
	CVector3D vec_P1P2 = ptE_seg - ptS_seg;
	lamda = (vec_P1P | vec_P1P2) / vec_P1P2.GetLengthSquare();

	if (lamda<0)
	{
		double x = vec_P1P.GetLengthSquare();
		return vec_P1P.GetLengthSquare();
	}
	if (lamda >= 0 && lamda <= 1)
	{
		CPoint3D pt_ = ptS_seg + vec_P1P2*lamda;
		CVector3D vec = pt_ - pt;
		return vec.GetLengthSquare();
	}
	if (lamda>1)
	{
		CVector3D vec = ptE_seg - pt;
		return vec.GetLengthSquare();
	}

	return 0.0;
}

double GetDist2PtToSeg(const CPoint3D& pt,
	const CPoint3D& ptS_seg,
	const CPoint3D& ptE_seg)
{
	if (ptS_seg == ptE_seg) return (pt - ptS_seg).GetMod2();

	double lamda;
	CVector3D vec_P1P = pt - ptS_seg;
	CVector3D vec_P1P2 = ptE_seg - ptS_seg;
	lamda = (vec_P1P | vec_P1P2) / vec_P1P2.GetMod2();

	if (lamda<0)
	{
		double x = vec_P1P.GetMod2();
		return vec_P1P.GetMod2();
	}
	if (lamda >= 0 && lamda <= 1)
	{
		CPoint3D pt_ = ptS_seg + vec_P1P2*lamda;
		CVector3D vec = pt_ - pt;
		return vec.GetMod2();
	}
	if (lamda>1)
	{
		CVector3D vec = ptE_seg - pt;
		return vec.GetMod2();
	}

	return 0.0;
}

CPoint3D CalPlaneLineIntersectPoint(const CVector3D& planeVector, const CPoint3D& planePoint, const CVector3D& lineVector, const CPoint3D& linePoint)
{
	CPoint3D ret;
	double vp1, vp2, vp3, n1, n2, n3, v1, v2, v3, m1, m2, m3, t, vpt;

	vp1 = planeVector.dx;
	vp2 = planeVector.dy;
	vp3 = planeVector.dz;
	n1 = planePoint.x;
	n2 = planePoint.y;
	n3 = planePoint.z;
	v1 = lineVector.dx;
	v2 = lineVector.dy;
	v3 = lineVector.dz;
	m1 = linePoint.x;
	m2 = linePoint.y;
	m3 = linePoint.z;
	vpt = vp1*v1 + vp2*v2 + vp3*v3;
	if (vpt == 0)
	{
		ret.x = 0.0;
		ret.y = 0.0;
		ret.z = 0.0;
	}
	else
	{
		t = ((n1 - m1)*vp1 + (n2 - m2)*vp2 + (n3 - m3)*vp3) / vpt;
	}
	ret.x = m1 + v1*t;
	ret.y = m2 + v2*t;
	ret.z = m3 + v3*t;

	return ret;
}

double CalPointtoPlane(const CPoint3D& point, const CVector3D& planeVector, const CPoint3D& planePoint)
{
	double distance;
//	double ptp;
//	ptp = ::GetDistance(point, planePoint);
	CVector3D v = CVector3D(planePoint, point);
	distance = (v | planeVector) / planeVector.GetLength();
	return distance;
}

int PointtoPlane(const CPoint3D& point, const CVector3D& planeVector, const CPoint3D& planePoint)
{
	double angle = 0;
	CVector3D v = CVector3D(planePoint, point);
	if ((v | planeVector) > 0)
	{
		return PLANE_PLUS;
	}
	else if ((v | planeVector) == 0)
	{
		return PLANE_ON;
	}
	else
	{
		return PLANE_MINUS;
	}
}
