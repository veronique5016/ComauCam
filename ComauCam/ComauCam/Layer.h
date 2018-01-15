#pragma once
#include "SliceModel.h"
#include "Line.h"
#include "Polyline.h"
//#include "Global.h"
#include <vector>
using namespace std;

class CSliceModel;
typedef CPolyline CBoundary;
class CLayer;
class AFX_CLASS_EXPORT CLayer
{
public:
	CLayer(CSliceModel* pSliceModel);
	~CLayer(void);

	void   SetLayerZValue(double z);    //设置层片zvalue值
	double GetLayerZValue();            //获取zvalue
	double m_z;                         //z高度，层标识
	void LinesToBoundaries();           // 将线连成轮廓
	void IdentifyBoundaryDirections();  // 确认是顺时针还是逆时针
	vector<CLine> m_boundaryLines;      //水平面和三角形直接截交得到的截交线段集合
protected:
	CSliceModel* m_pSliceModel;
public:
	typedef vector<CBoundary*> Boundaries;

public:
	bool IsOutMost(CBoundary* pCurBoundary, vector<CBoundary*>& boundaries);//？？？

																			// Test if pContainer contains pElement
	bool IsContain(CBoundary* pElement, CBoundary* pContainer);//是否包含？？？	
	Boundaries  m_boundaries;   // 从SLC读进来的轮廓
};


