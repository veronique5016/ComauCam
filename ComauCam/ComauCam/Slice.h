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
	void getBoundaryPoints(SliceLayer* layer);
	void calIntersectPoint(SliceLayer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point);	// ����Ƭƽ����ĳ�����ཻ�õ���������
	void getInterSectEdge(SliceLayer* layer, LTriangle* pCurFace);   //�������㷨��ʼ���������һ�����ߣ��͵�һ�����㣻

public:
	void judgeFaceType(SliceLayer* layer, LTriangle* pCurFace);    //�ж�����Ƭƽ���ཻ��Ƭ������
	void judgeOtherLine(SliceLayer* layer, LTriangle* pCurFace);   //�ж���һ���ཻ��
	double compareThreeNumber(double v1, double v2, double v3, int type);
	double zminofLayer(SliceLayer* layer);
	void optimizeBoundary(SliceLayer* layer);
	bool isBoundaryCCW(SliceLayer* layer);	//�������ںܴ����⣬��������ĳ��������ֵ�ǳ�Сʱ�����ܻ����һЩĪ����������� 
	void makeBoundaryCCW(SliceLayer* layer);
	void deletePoints(SliceLayer* layer);
	void drawLayer(bool showPolygon, int start, int end);

public:
	vector<LEdge*> m_Slice_edge;     //�洢�ཻ��
	vector<SliceLayer*> m_layers;     //�洢Ƭ��
	vector<LTriangle*> m_tris_slice;   //����һ��������Ƭ����Ϊ��ʼ����
	vector<int> index;	//�������з�����Ϊ z �����������Ƭ���
	double height; // ��������
};

