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
	virtual void Draw(COpenGLDC* pDC);   // 里面是空的，后面可能要自己重写
	void SetSTLModel(CSTLModel* pSTLModel);

	void PushToSpecifiedLayer(double zValue, const CLine& line);   //将横截面的线存到对应的层片里
	void GntLayerBoundaries();                                     //获取层片的轮廓
	vector<CLayer*> m_layers;
protected:
	CSTLModel* m_pSTLModel;                                        //主要获取STL模型的文件名用
};

