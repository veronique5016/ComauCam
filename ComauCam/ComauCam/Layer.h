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

	void   SetLayerZValue(double z);    //���ò�Ƭzvalueֵ
	double GetLayerZValue();            //��ȡzvalue
	double m_z;                         //z�߶ȣ����ʶ
	void LinesToBoundaries();           // ������������
	void IdentifyBoundaryDirections();  // ȷ����˳ʱ�뻹����ʱ��
	vector<CLine> m_boundaryLines;      //ˮƽ���������ֱ�ӽؽ��õ��Ľؽ��߶μ���
protected:
	CSliceModel* m_pSliceModel;
public:
	typedef vector<CBoundary*> Boundaries;

public:
	bool IsOutMost(CBoundary* pCurBoundary, vector<CBoundary*>& boundaries);//������

																			// Test if pContainer contains pElement
	bool IsContain(CBoundary* pElement, CBoundary* pContainer);//�Ƿ����������	
	Boundaries  m_boundaries;   // ��SLC������������
};


