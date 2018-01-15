#pragma once
#include "Polyline.h"
#include "Layer.h"
#include "STLModel.h"
#include "Line.h"
#include <vector>

using namespace std;
class CLayer;
class AFX_CLASS_EXPORT CSliceModel : public CEntity
{
public:
	CSliceModel();
	~CSliceModel(void);
	virtual void Draw(COpenGLDC* pDC);   // �����ǿյģ��������Ҫ�Լ���д
	void SetSTLModel(CSTLModel* pSTLModel);

	void PushToSpecifiedLayer(double zValue, const CLine& line);   //���������ߴ浽��Ӧ�Ĳ�Ƭ��
	void GntLayerBoundaries();                                     //��ȡ��Ƭ������
	vector<CLayer*> m_layers;
protected:
	CSTLModel* m_pSTLModel;                                        //��Ҫ��ȡSTLģ�͵��ļ�����
};

