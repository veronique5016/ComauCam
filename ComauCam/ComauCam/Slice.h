#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include <vector>
#include "OpenGL.h"
#include "Layer.h"
using namespace std;

#define EXTRUDER_DIAMETER 1

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
	void LoadSTLModel(CSTLModel* models);	//����ģ�ͣ��������˹�ϵ
	void Slice(CSTLModel* model);  //��Ƭ����
	void GetBoundaryPoints(CSliceLayer* layer);
	void CalIntersectPoint(CSliceLayer* layer, CLEdge * edge, CLTriangle*pCurFace, CLPoint* point);	// ����Ƭƽ����ĳ�����ཻ�õ���������
	void GetInterSectEdge(CSliceLayer* layer, CLTriangle* pCurFace);   //�������㷨��ʼ���������һ�����ߣ��͵�һ�����㣻

public:
	void JudgeFaceType(CSliceLayer* layer, CLTriangle* pCurFace);    //�ж�����Ƭƽ���ཻ��Ƭ������
	void JudgeOtherLine(CSliceLayer* layer, CLTriangle* pCurFace);   //�ж���һ���ཻ��
	double CompareThreeNumber(double v1, double v2, double v3, int type);
	double ZminofLayer(CSliceLayer* layer);
	int FindLowestSegment(CSliceLayer* layer);
	void ModifyTurnLayer(CSliceLayer* layer);
	void OptimizeBoundary(CSliceLayer* layer);
	bool IsBoundaryCCW(CSliceLayer* layer);
	void MakeBoundaryCCW(CSliceLayer* layer);
	void RearrangeBoundary(CSliceLayer* layer);
	void DeletePoints(CSliceLayer* layer);
	void DrawLayer(bool showPolygon, int start, int end);

public:
	vector<CLEdge*> m_vecpSliceEdge;     //�洢�ཻ��
	vector<CSliceLayer*> m_vecpLayers;     //�洢Ƭ��
	vector<CLTriangle*> m_vecpSliceTris;   //����һ��������Ƭ����Ϊ��ʼ����
	//vector<int> m_vecnIndex;	//�������з�����Ϊ z �����������Ƭ���
	double height; // ��������
};

