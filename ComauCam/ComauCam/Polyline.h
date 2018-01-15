#pragma once
#include "PointSet.h"
//#include "Slice.h"
//class CPolylineSet;
class CPolyline;
class CSlice;
class AFX_CLASS_EXPORT CPolyline : public CPointSet
{
public:
	CPolyline(void);
	virtual~CPolyline(void);
	CPolyline(int n, CPoint3D pt, ...);
	void Init();
	//	const CPolyline& operator=(const CPolyline& polyline);
	BOOL m_bIsClosed;
	void AddPt(CPoint3D* pPt);
	void AddPt(const CPoint3D& pt);
	//Point3DAry m_pts; 
#define  POINT_RESOLUTION     1e-8 * 1e-8
	void DelCoincidePt(double dist_resolution = POINT_RESOLUTION);
	void DelColinearPt(double angle_resolution = 1.0e-5 /*rad*/);
	void MakePolygonCCW();
	void MakePolygonCW();
	BOOL IsPolygonCCW();
	bool IsPtInPolygon(const CPoint3D& pt) const;
	bool IsPtOnPolygon(const CPoint3D& pt) const;
	CPoint3D GetFirstPt() const;
	CPoint3D GetLastPt() const;
	virtual void Rotate(const CMatrix3D& m);
	virtual void Draw(COpenGLDC* pDC, bool ShowTri);
protected:
	int O(int i, int sz);
	double O(const double& i, const int& nOffset, const int& sz);
private:
	bool m_bIsDrawPt;
};
