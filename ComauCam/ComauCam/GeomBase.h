#pragma once

#define  AXIS_X     1
#define  AXIS_Y     2
#define  AXIS_Z     3
#ifndef PI
#define PI        3.1415926535897932384626433832795
#define HALFPI		 PI/2.0
#endif

// ZERO marco is defined to compare double number
#define ZERO      0.0      
#define ONE       1.0

#define PLANE_PLUS 1
#define PLANE_MINUS -1
#define PLANE_ON 0

#define  POINT_EQUAL_ERROR    0.0000000001



class CPoint3D;     ////////////////////////////////////////////////
class CVector3D;    // 这个声明很重要，否则无法编译
class CMatrix3D;    ////////////////////////////////////////////////


					//////////////////////////////////////////////////////////////////////////
					// 点
					//////////////////////////////////////////////////////////////////////////
class  AFX_CLASS_EXPORT CPoint3D :public CObject
{
public:     ////// constructor
	CPoint3D();                                  // 构造函数：构造零点                                           
	CPoint3D(const double& ix, const double& iy, const double& iz);     // 构造函数
	CPoint3D(const double* p);                   // 用数组构造函数
	CPoint3D(const CPoint3D& pt);                        // 用点构造
	virtual ~CPoint3D();                         // 析构函数           

public:     ////// operator
	const CPoint3D& operator=(const CPoint3D& pt);
	CPoint3D operator+(const CVector3D& v) const;        // 点和向量相加，返回点
	CPoint3D operator+(const CPoint3D& pt) const;
	void operator+=(const CVector3D& v);                 // 点和向量加等
	CPoint3D operator-(const CVector3D& v) const;        // 点和向量相减，返回点
	void operator-=(const CVector3D& v);                 // 点和向量操作
	CVector3D operator-(const CPoint3D& pt) const;       // 点减去点得到向量
	BOOL operator==(const CPoint3D& pt) const;           // Complete the same
	BOOL operator^=(const CPoint3D& pt) const;           // More less the same ~==
	BOOL operator!=(const CPoint3D& pt) const;           // 比较操作符
	CPoint3D operator*(const CMatrix3D& matrix) const;   // 点和矩阵相乘得到点，其实就是变换操作
	CPoint3D operator*(const double& dFactor) const;
	void operator*=(const CMatrix3D& matrix);            // 点乘以矩阵   

public:
	CPoint3D *p_prev, *p_next;


public:
	double x;
	double y;
	double z;
};
typedef CTypedPtrArray<CObArray, CPoint3D*> Point3DAry;

//////////////////////////////////////////////////////////////////////////
// 向量
//////////////////////////////////////////////////////////////////////////
class AFX_CLASS_EXPORT CVector3D :public CObject
{
public:
	CVector3D();                                  // 构造函数，构造零向量
	CVector3D(const double& ix, const double& iy, const double& iz);     // 构造函数
	CVector3D(const double* p);                         // 构造函数：用数组构造
	CVector3D(const CVector3D& v);                        // 构造函数：用向量构造
	CVector3D(const CPoint3D p1, const CPoint3D p2);
	virtual ~CVector3D();                         // 析构函数

public:
	const CVector3D& operator=(const CVector3D& v);
	BOOL operator==(const CVector3D& v) const;
	CVector3D operator+(const CVector3D& v) const;       // 向量和向量相加得到向量
	void operator+=(const CVector3D& v);                 // 向量和向量加等
	CVector3D operator-(const CVector3D& v) const;       // 向量和向量相减得到向量
	void operator-=(const CVector3D& v);                 // 向量和向量减等

	CVector3D operator-();                               // 负号，取负操作

	CVector3D operator*(const double& d) const;          // 向量和数乘得到向量
	void operator*=(const double& d);                    // 向量和数乘等
	CVector3D operator/(const double& d) const;          // 向量和数除得到向量
	void operator/=(const double& d);                    // 向量和数除等

	CVector3D operator*(const CVector3D& v) const;       // 向量和向量叉乘
	double operator|(const CVector3D& v) const;          // 向量和向量点乘

	CVector3D operator*(const CMatrix3D& matrix) const;  // 向量和矩阵相乘得到向量
	void operator*=(const CMatrix3D& matrix);            // 向量和矩阵乘等

	double GetLength() const;                            // 向量模
	double GetLengthSquare() const;
	// 距离平方
	double GetMod2() const;
	double GetLengthXY() const;                          // 
	double GetLengthYZ() const;                          //
	double GetLengthZX() const;                          // const表示函数调用不会改变成员变量的值。
	double GetAngle(CVector3D v1, CVector3D v2) const;    // 取得向量的夹角

	CVector3D GetNormal() const;                         // 得到向量的单位向量，返回单位向量
	void Normalize();                                    // 向量单位化

	BOOL IsZeroLength() const;                           // 判断向量模是不是零

public:
	double dx;
	double dy;
	double dz;
};



//////////////////////////////////////////////////////////////////////////
// 矩阵
//////////////////////////////////////////////////////////////////////////
class AFX_CLASS_EXPORT CMatrix3D :public CObject
{
public:
	CMatrix3D();                                         // 构造函数，构造单位矩阵
	CMatrix3D(const CMatrix3D& matrix);                   // 构造函数，用矩阵构造
	CMatrix3D(const double* p);                                // 构造函数，用数组构造
	virtual ~CMatrix3D();                                // 析构函数

public:
	const CMatrix3D& operator=(const CMatrix3D& matrix);
	CMatrix3D operator*(const CMatrix3D& matrix) const;  // 矩阵相乘，返回矩阵
	void operator*=(const CMatrix3D& matrix);            // 矩阵乘等

	void IdenticalMatrix();                              // 单位化矩阵
	double GetValue() const;                             // 获取子行列式值

public:
	static double GetValue(const double& a00, const double& a01, const double& a02,
		const double& a10, const double& a11, const double& a12,
		const  double& a20, const double& a21, const double& a22);                      //
	static CMatrix3D CreateMirrorMatrix(CVector3D planeNorm);                      // 过原点的平面对称矩阵
	static CMatrix3D CreateRotateMatrix(const double& angle, CVector3D axis);              // 向量都是过原点的,逆时针
	static CMatrix3D CreateRotateMatrix(const double& angle, int nAxis);
	static CMatrix3D CreateScaleMatrix(const double& xyzAxis);                            // 三轴一起缩放
	static CMatrix3D CreateScaleMatrix(const double& xAxis, const double& yAxis, const double& zAxis);    // x,y,z三个轴分别缩放
	static CMatrix3D CreateTransferMatrix(const CVector3D& vec);                          // 平移矩阵

public:
	double A[4][4];  // 第一个是行，第二个是列
};

class AFX_CLASS_EXPORT CRect3D : public CObject
{
public:
	CRect3D() {}
	~CRect3D() {}
	CRect3D(const double& left_in, const double& right_in, const double& bottom_in, const double& top_in)
	{
		left = left_in;
		right = right_in;
		bottom = bottom_in;
		top = top_in;
	}
	CRect3D(const CRect3D& rt)
	{
		left = rt.left;
		right = rt.right;
		bottom = rt.bottom;
		top = rt.top;
	}
	CRect3D(const CPoint3D& ptCenter, const double& offset_dis)
	{
		left = ptCenter.x - offset_dis;
		right = ptCenter.x + offset_dis;
		bottom = ptCenter.y - offset_dis;
		top = ptCenter.y + offset_dis;
	}
	const CRect3D& operator=(const CRect3D& rt)
	{
		left = rt.left;
		right = rt.right;
		bottom = rt.bottom;
		top = rt.top;
		return *this;
	}

	BOOL operator*(const CRect3D& rt) // if two rect intersect?
	{
		if (right < rt.left)
		{
			return FALSE;
		}
		else if (left > rt.right)
		{
			return FALSE;
		}
		else if (top < rt.bottom)
		{
			return FALSE;
		}
		else if (bottom > rt.top)
		{
			return FALSE;
		}

		return TRUE;
	}

	double GetMinBorder()
	{
		double wide = right - left;
		double height = top - bottom;
		double minBorder = wide;
		if (height<minBorder)
			minBorder = height;

		return minBorder;
	}

public:
	double left;
	double right;
	double bottom;
	double top;
};

//////////////////////////////////////////////////////////////////////////
// 输出函数
//////////////////////////////////////////////////////////////////////////
double AFX_API_EXPORT GetAngle(CVector3D v1, CVector3D v2);
double AFX_API_EXPORT Angle(const CVector3D& v1, const CVector3D& v2); // v1, v2 has been normlized
double AFX_API_EXPORT GetDistance(const CPoint3D& pt1, const CPoint3D& pt2);
BOOL   AFX_API_EXPORT IsParallel(const CVector3D& v1, const CVector3D& v2);
BOOL   AFX_API_EXPORT IsParallel(const CVector3D* v1, const CVector3D* v2);
BOOL   AFX_API_EXPORT IsOrhogonal(const CVector3D& v1, const CVector3D& v2);
double AFX_API_EXPORT Round(const double& d, const int& iHowManyAfterDot = 5); // 保留小数点后三位

BOOL   AFX_API_EXPORT Is2SegsCross(const CPoint3D& ptS_seg1, const CPoint3D& ptE_seg1, const CPoint3D& ptS_seg2, const CPoint3D& ptE_seg2); // 两条直线重合也是干涉的
BOOL   AFX_API_EXPORT Is2SegsCross(CPoint3D& ptCross, const CPoint3D& ptS_seg1, const CPoint3D& ptE_seg1, const CPoint3D& ptS_seg2, const CPoint3D& ptE_seg2); // 两条直线重合也是干涉的

																																							   // --------------------获取两条直线的交点---------------------- //
																																							   // 返回值 0 两条直线平行，1 两条直线重合， 2 两条直线相交有交点 //
																																							   // 参数1有交点时返回的交点坐标，参数2，3是第一条直线上两个点    //
																																							   // 参数4，5是第二条直线上两个点                                 //
int    AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, const CPoint3D& ptS_seg1, const CPoint3D& ptE_seg1, const CPoint3D& ptS_seg2, const CPoint3D& ptE_seg2); // 对于两条相交直线求交点
																																							   // ------在XY平面内获取一条XY平面内直线的垂直方向----- //
																																							   // 返回值为垂直方向的单位向量,指向在原来方向直线的左边 //
																																							   // 参数1，2是要求的直线上的两个点                      //
CVector3D AFX_API_EXPORT GetNormalVector(const CPoint3D& ptS, const CPoint3D& ptE);
CVector3D AFX_API_EXPORT GetNormalVector(const CVector3D& vec);

// 计算点到线段的距离,返回是距离的平方 //
double    AFX_API_EXPORT CalDis2PtToSeg(const CPoint3D& pt, const CPoint3D& ptS_seg, const CPoint3D& ptE_seg);

double AFX_API_EXPORT GetDist2PtToSeg(const CPoint3D& pt,
	const CPoint3D& ptS_seg,
	const CPoint3D& ptE_seg);


// 求直线与平面交点，参数依次为平面法向量、平面上任一点、直线方向向量、直线上任一点
CPoint3D AFX_API_EXPORT CalPlaneLineIntersectPoint(const CVector3D& planeVector, const CPoint3D& planePoint, const CVector3D& lineVector, const CPoint3D& linePoint);

// 计算点到平面的距离
double AFX_API_EXPORT CalPointtoPlane(const CPoint3D& point, const CVector3D& planeVector, const CPoint3D& planePoint);

// 判断点和平面的关系，若点在平面法向量的正方向，则为真，
int  AFX_API_EXPORT PointtoPlane(const CPoint3D& point, const CVector3D& planeVector, const CPoint3D& planePoint);

// 计算点到直线的距离
double AFX_API_EXPORT CalPointtoLine(const CPoint3D& point, const CPoint3D& startPoint, const CPoint3D& endPoint);
