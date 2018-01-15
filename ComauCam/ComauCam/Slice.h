#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include <vector>
#include "OpenGL.h"
using namespace std;


//��Ƭ���� 
#define NO_POINT_ON_SURFACE 1
#define ONE_POINT_ON_SURFACE 2 
#define EDGE_ON_SURFACE 3
#define ONLY_ONE_POINT_ON_SURFACE 4

#define Z_MIN 1
#define Z_MID 2
#define Z_MAX 3

struct PolyLine
{
public:
	PolyLine();
	~PolyLine();
	vector<CPoint3D*> m_Linkpoints;
};

struct Layer
{
public:
	Layer();
	~Layer();
	CPoint3D layerPoint;
	CVector3D layer_gravity;
	vector<PolyLine*> m_Polylines;
};

class CSlice
{
public:
	CSlice(void);
	~CSlice(void);
	void LoadSTLModel(CSTLModel* models);	//����ģ�ͣ��������˹�ϵ
	void slice(CSTLModel* model);  //��Ƭ����
	void getpolylinePoints(Layer* layer);
	void drawpolyline();
	void getTurn();     // ��ȡģ��ת�۵�

protected:
	void InterSect(Layer* layer, LTriangle* pCurFace);   //�������㷨��ʼ���������һ�����ߣ��͵�һ�����㣻
	void CalIntersectPoint(Layer* layer, LEdge * edge, LTriangle*pCurFace, CPoint3D* point);
	void JudgeFaceType(Layer* layer, LTriangle* pCurFace);    //�ж���Ƭ������
	void JudgeOtherLine(Layer* layer, LTriangle* pCurFace);   //�ж���һ���ཻ��
	double ReturnZmin(const LTriangle* Ltri);   //����һ����Ƭ����СZ����ֵ
	double ReturnZmax(const LTriangle* Ltri);
	double ReturnZmid(const LTriangle* Ltri);

	double ReturnZtype(double v1, double v2, double v3, int ztype);

public:
	vector<LEdge*> m_Slice_edge;     //�洢�ཻ��
	vector<Layer*> m_layers;         //�洢Ƭ��
	vector<LTriangle*> m_tris_slice;   //����һ��������Ƭ����Ϊ��ʼ����

	vector<LTriangle*> z_tris;

	double height;
	CVector3D gravity;
};

