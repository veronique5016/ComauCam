#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include <vector>
#include "OpenGL.h"
#include "Layer.h"
using namespace std;

#define EXTRUDER_DIAMETER 0.4

//��Ƭ���� 
#define NO_POINT_ON_SURFACE 1
#define ONE_POINT_ON_SURFACE 2 
#define EDGE_ON_SURFACE 3
#define ONLY_ONE_POINT_ON_SURFACE 4

#define MIN 1
#define MID 2
#define MAX 3

class CSlice
{
public:
	CSlice(void);
	~CSlice(void);

public:
	void loadSTLModel(CSTLModel* models);	//����ģ�ͣ��������˹�ϵ
	void slice(CSTLModel* model);  //��Ƭ����
	double getTurnHeight();     // ��ȡģ��ת�۵�
	CVector3D getTurnVec();
	void begin3DSlice(double z_min, double z_max, double& z, double dz);
	void begin5DSlice(double z_min, double z_max, double& z, double dz);
	void getPolylinePoints(Layer* layer);
	void calIntersectPoint(Layer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point);	// ��������
	void getInterSectEdge(Layer* layer, LTriangle* pCurFace);   //�������㷨��ʼ���������һ�����ߣ��͵�һ�����㣻

public:
	void judgeFaceType(Layer* layer, LTriangle* pCurFace);    //�ж�����Ƭƽ���ཻ��Ƭ������
	void judgeOtherLine(Layer* layer, LTriangle* pCurFace);   //�ж���һ���ཻ��
	double compareThreeNumber(double v1, double v2, double v3, int type);
	bool isBoundaryCCW(Layer* layer);	//�������ںܴ����⣬��������ĳ��������ֵ�ǳ�Сʱ�����ܻ����һЩĪ����������� 
	void makeBoundaryCCW(Layer* layer);
	void deletePoints(Layer* layer);
	void drawLayer(bool showPolygon, int start, int end);
	bool lineNeedSupport();

public:
	vector<LEdge*> m_Slice_edge;     //�洢�ཻ��
	vector<Layer*> m_layers;         //�洢Ƭ��
	vector<LTriangle*> m_tris_slice;   //����һ��������Ƭ����Ϊ��ʼ����
	vector<LTriangle*> z_tris;

	double height;

	bool need_support;
};

