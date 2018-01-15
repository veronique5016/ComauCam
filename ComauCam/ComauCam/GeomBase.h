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
class CVector3D;    // �����������Ҫ�������޷�����
class CMatrix3D;    ////////////////////////////////////////////////


					//////////////////////////////////////////////////////////////////////////
					// ��
					//////////////////////////////////////////////////////////////////////////
class  AFX_CLASS_EXPORT CPoint3D :public CObject
{
public:     ////// constructor
	CPoint3D();                                  // ���캯�����������                                           
	CPoint3D(const double& ix, const double& iy, const double& iz);     // ���캯��
	CPoint3D(const double* p);                   // �����鹹�캯��
	CPoint3D(const CPoint3D& pt);                        // �õ㹹��
	virtual ~CPoint3D();                         // ��������           

public:     ////// operator
	const CPoint3D& operator=(const CPoint3D& pt);
	CPoint3D operator+(const CVector3D& v) const;        // ���������ӣ����ص�
	CPoint3D operator+(const CPoint3D& pt) const;
	void operator+=(const CVector3D& v);                 // ��������ӵ�
	CPoint3D operator-(const CVector3D& v) const;        // ���������������ص�
	void operator-=(const CVector3D& v);                 // �����������
	CVector3D operator-(const CPoint3D& pt) const;       // ���ȥ��õ�����
	BOOL operator==(const CPoint3D& pt) const;           // Complete the same
	BOOL operator^=(const CPoint3D& pt) const;           // More less the same ~==
	BOOL operator!=(const CPoint3D& pt) const;           // �Ƚϲ�����
	CPoint3D operator*(const CMatrix3D& matrix) const;   // ��;�����˵õ��㣬��ʵ���Ǳ任����
	CPoint3D operator*(const double& dFactor) const;
	void operator*=(const CMatrix3D& matrix);            // ����Ծ���   

public:
	CPoint3D *p_prev, *p_next;


public:
	double x;
	double y;
	double z;
};
typedef CTypedPtrArray<CObArray, CPoint3D*> Point3DAry;

//////////////////////////////////////////////////////////////////////////
// ����
//////////////////////////////////////////////////////////////////////////
class AFX_CLASS_EXPORT CVector3D :public CObject
{
public:
	CVector3D();                                  // ���캯��������������
	CVector3D(const double& ix, const double& iy, const double& iz);     // ���캯��
	CVector3D(const double* p);                         // ���캯���������鹹��
	CVector3D(const CVector3D& v);                        // ���캯��������������
	CVector3D(const CPoint3D p1, const CPoint3D p2);
	virtual ~CVector3D();                         // ��������

public:
	const CVector3D& operator=(const CVector3D& v);
	BOOL operator==(const CVector3D& v) const;
	CVector3D operator+(const CVector3D& v) const;       // ������������ӵõ�����
	void operator+=(const CVector3D& v);                 // �����������ӵ�
	CVector3D operator-(const CVector3D& v) const;       // ��������������õ�����
	void operator-=(const CVector3D& v);                 // ��������������

	CVector3D operator-();                               // ���ţ�ȡ������

	CVector3D operator*(const double& d) const;          // ���������˵õ�����
	void operator*=(const double& d);                    // ���������˵�
	CVector3D operator/(const double& d) const;          // �����������õ�����
	void operator/=(const double& d);                    // ������������

	CVector3D operator*(const CVector3D& v) const;       // �������������
	double operator|(const CVector3D& v) const;          // �������������

	CVector3D operator*(const CMatrix3D& matrix) const;  // �����;�����˵õ�����
	void operator*=(const CMatrix3D& matrix);            // �����;���˵�

	double GetLength() const;                            // ����ģ
	double GetLengthSquare() const;
	// ����ƽ��
	double GetMod2() const;
	double GetLengthXY() const;                          // 
	double GetLengthYZ() const;                          //
	double GetLengthZX() const;                          // const��ʾ�������ò���ı��Ա������ֵ��
	double GetAngle(CVector3D v1, CVector3D v2) const;    // ȡ�������ļн�

	CVector3D GetNormal() const;                         // �õ������ĵ�λ���������ص�λ����
	void Normalize();                                    // ������λ��

	BOOL IsZeroLength() const;                           // �ж�����ģ�ǲ�����

public:
	double dx;
	double dy;
	double dz;
};



//////////////////////////////////////////////////////////////////////////
// ����
//////////////////////////////////////////////////////////////////////////
class AFX_CLASS_EXPORT CMatrix3D :public CObject
{
public:
	CMatrix3D();                                         // ���캯�������쵥λ����
	CMatrix3D(const CMatrix3D& matrix);                   // ���캯�����þ�����
	CMatrix3D(const double* p);                                // ���캯���������鹹��
	virtual ~CMatrix3D();                                // ��������

public:
	const CMatrix3D& operator=(const CMatrix3D& matrix);
	CMatrix3D operator*(const CMatrix3D& matrix) const;  // ������ˣ����ؾ���
	void operator*=(const CMatrix3D& matrix);            // ����˵�

	void IdenticalMatrix();                              // ��λ������
	double GetValue() const;                             // ��ȡ������ʽֵ

public:
	static double GetValue(const double& a00, const double& a01, const double& a02,
		const double& a10, const double& a11, const double& a12,
		const  double& a20, const double& a21, const double& a22);                      //
	static CMatrix3D CreateMirrorMatrix(CVector3D planeNorm);                      // ��ԭ���ƽ��Գƾ���
	static CMatrix3D CreateRotateMatrix(const double& angle, CVector3D axis);              // �������ǹ�ԭ���,��ʱ��
	static CMatrix3D CreateRotateMatrix(const double& angle, int nAxis);
	static CMatrix3D CreateScaleMatrix(const double& xyzAxis);                            // ����һ������
	static CMatrix3D CreateScaleMatrix(const double& xAxis, const double& yAxis, const double& zAxis);    // x,y,z������ֱ�����
	static CMatrix3D CreateTransferMatrix(const CVector3D& vec);                          // ƽ�ƾ���

public:
	double A[4][4];  // ��һ�����У��ڶ�������
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
// �������
//////////////////////////////////////////////////////////////////////////
double AFX_API_EXPORT GetAngle(CVector3D v1, CVector3D v2);
double AFX_API_EXPORT Angle(const CVector3D& v1, const CVector3D& v2); // v1, v2 has been normlized
double AFX_API_EXPORT GetDistance(const CPoint3D& pt1, const CPoint3D& pt2);
BOOL   AFX_API_EXPORT IsParallel(const CVector3D& v1, const CVector3D& v2);
BOOL   AFX_API_EXPORT IsParallel(const CVector3D* v1, const CVector3D* v2);
BOOL   AFX_API_EXPORT IsOrhogonal(const CVector3D& v1, const CVector3D& v2);
double AFX_API_EXPORT Round(const double& d, const int& iHowManyAfterDot = 5); // ����С�������λ

BOOL   AFX_API_EXPORT Is2SegsCross(const CPoint3D& ptS_seg1, const CPoint3D& ptE_seg1, const CPoint3D& ptS_seg2, const CPoint3D& ptE_seg2); // ����ֱ���غ�Ҳ�Ǹ����
BOOL   AFX_API_EXPORT Is2SegsCross(CPoint3D& ptCross, const CPoint3D& ptS_seg1, const CPoint3D& ptE_seg1, const CPoint3D& ptS_seg2, const CPoint3D& ptE_seg2); // ����ֱ���غ�Ҳ�Ǹ����

																																							   // --------------------��ȡ����ֱ�ߵĽ���---------------------- //
																																							   // ����ֵ 0 ����ֱ��ƽ�У�1 ����ֱ���غϣ� 2 ����ֱ���ཻ�н��� //
																																							   // ����1�н���ʱ���صĽ������꣬����2��3�ǵ�һ��ֱ����������    //
																																							   // ����4��5�ǵڶ���ֱ����������                                 //
int    AFX_API_EXPORT GetCrossPoint(CPoint3D& pt_out, const CPoint3D& ptS_seg1, const CPoint3D& ptE_seg1, const CPoint3D& ptS_seg2, const CPoint3D& ptE_seg2); // ���������ֱཻ���󽻵�
																																							   // ------��XYƽ���ڻ�ȡһ��XYƽ����ֱ�ߵĴ�ֱ����----- //
																																							   // ����ֵΪ��ֱ����ĵ�λ����,ָ����ԭ������ֱ�ߵ���� //
																																							   // ����1��2��Ҫ���ֱ���ϵ�������                      //
CVector3D AFX_API_EXPORT GetNormalVector(const CPoint3D& ptS, const CPoint3D& ptE);
CVector3D AFX_API_EXPORT GetNormalVector(const CVector3D& vec);

// ����㵽�߶εľ���,�����Ǿ����ƽ�� //
double    AFX_API_EXPORT CalDis2PtToSeg(const CPoint3D& pt, const CPoint3D& ptS_seg, const CPoint3D& ptE_seg);

double AFX_API_EXPORT GetDist2PtToSeg(const CPoint3D& pt,
	const CPoint3D& ptS_seg,
	const CPoint3D& ptE_seg);


// ��ֱ����ƽ�潻�㣬��������Ϊƽ�淨������ƽ������һ�㡢ֱ�߷���������ֱ������һ��
CPoint3D AFX_API_EXPORT CalPlaneLineIntersectPoint(const CVector3D& planeVector, const CPoint3D& planePoint, const CVector3D& lineVector, const CPoint3D& linePoint);

// ����㵽ƽ��ľ���
double AFX_API_EXPORT CalPointtoPlane(const CPoint3D& point, const CVector3D& planeVector, const CPoint3D& planePoint);

// �жϵ��ƽ��Ĺ�ϵ��������ƽ�淨��������������Ϊ�棬
int  AFX_API_EXPORT PointtoPlane(const CPoint3D& point, const CVector3D& planeVector, const CPoint3D& planePoint);

// ����㵽ֱ�ߵľ���
double AFX_API_EXPORT CalPointtoLine(const CPoint3D& point, const CPoint3D& startPoint, const CPoint3D& endPoint);
