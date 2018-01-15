#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include <vector>
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

public:
	void GetBoundaryPoints(CSliceLayer* layer);
	void CalIntersectPoint(CSliceLayer* layer, CLEdge * edge, CLTriangle*pCurFace, CLPoint* point);	// ����Ƭƽ����ĳ�����ཻ�õ���������
	void GetInterSectEdge(CSliceLayer* layer, CLTriangle* pCurFace);   //�������㷨��ʼ���������һ�����ߣ��͵�һ�����㣻
	void JudgeFaceType(CSliceLayer* layer, CLTriangle* pCurFace);    //�ж�����Ƭƽ���ཻ��Ƭ������
	void JudgeOtherLine(CSliceLayer* layer, CLTriangle* pCurFace);   //�ж���һ���ཻ��
	double ZminofLayer(CSliceLayer* layer);
	int FindLowestSegment(CSliceLayer* layer);

public:
	void GetAddedLayerBoundary(CSliceLayer* layer, CSegment turnlayer_seg, CSegment layer_seg);

public:
	double CompareThreeNumber(double v1, double v2, double v3, int type);
	void ModifyTurnLayer(CSliceLayer* layer);	//�Ա䷨����ƽ�����������޸�
	void OptimizeBoundary(CSliceLayer* layer);
	bool IsBoundaryCCW(CSliceLayer* layer);		//�ж������Ƿ���ʱ��
	void MakeBoundaryCCW(CSliceLayer* layer);
	void RearrangeBoundary(CSliceLayer* layer);
	void DeletePoints(CSliceLayer* layer);		//ɾ�������

public:
	vector<CLEdge*> m_vecpSliceEdge;     //�洢�ཻ��
	vector<CSliceLayer*> m_vecpLayers;     //�洢Ƭ��
	vector<CLTriangle*> m_vecpSliceTris;   //����һ��������Ƭ����Ϊ��ʼ����
	double m_dHeight; // ��������
};

