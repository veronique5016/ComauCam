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
	double z;
	vector<PolyLine*> m_Polylines;
};

class CSlice
{
public:
	CSlice(void);
	~CSlice(void);
	void LoadSTLModel(CSTLModel* models);	//����ģ�ͣ��������˹�ϵ
	void slice(CSTLModel* model);  //��Ƭ����
	void getpolylinePoints(double z);
	void drawpolyline();

protected:
	void InterSect(double z, LTriangle* pCurFace);   //�������㷨��ʼ���������һ�����ߣ��͵�һ�����㣻
	void CalInerSectPoint(double z, LEdge * edge, LTriangle*pCurFace, CPoint3D* point);
	void JudgeFaceType(double z, LTriangle* pCurFace);    //�ж���Ƭ������
	void JudgeOtherLine(double z, LTriangle* pCurFace);   //�ж���һ���ཻ��
	double ReturnZmin(const LTriangle* Ltri);   //����һ����Ƭ����СZ����ֵ
	double ReturnZmax(const LTriangle* Ltri);
	double ReturnZmid(const LTriangle* Ltri);

public:
	vector<LEdge*> m_Slice_edge;     //�洢�ཻ��
	vector<Layer*> m_layers;         //�洢Ƭ��
	vector<LTriangle*> m_tris_slice;   //����һ��������Ƭ����Ϊ��ʼ����
};

