#pragma once
#include "STLModel.h"
#include "Triangle.h"
#include <vector>
#include "OpenGL.h"
using namespace std;


#define EXTRUDER_DIAMETER 0.4

//��Ƭ���� 
#define NO_POINT_ON_SURFACE 1
#define ONE_POINT_ON_SURFACE 2 
#define EDGE_ON_SURFACE 3
#define ONLY_ONE_POINT_ON_SURFACE 4

#define Z_MIN 1
#define Z_MID 2
#define Z_MAX 3
struct Layer;

struct LPoint : public CPoint3D
{
public:
	LPoint();
	~LPoint();
	LPoint(const LPoint& lpoint);
	LPoint(const CPoint3D& pt);

public:
	LPoint *p_prev, *p_next;
	LEdge* edge;
	Layer* layer;
};

struct PolyLine
{
public:
	PolyLine();
	~PolyLine();
	vector<LPoint*> m_Linkpoints;
};

struct Layer
{
public:
	Layer();
	~Layer();
	CPoint3D layerPoint;
	CVector3D layer_gravity;
	vector<PolyLine*> m_Polylines;   //�ؽ��õ��������������޸� size = 1
};

class CSlice
{
public:
	CSlice(void);
	~CSlice(void);
	void LoadSTLModel(CSTLModel* models);	//����ģ�ͣ��������˹�ϵ
	void slice(CSTLModel* model);  //��Ƭ����
	void getpolylinePoints(Layer* layer);
	void drawpolyline(bool showPolygon);
	double getTurnHeight();     // ��ȡģ��ת�۵�
	CVector3D getTurnVec();

public:
	void getInterSectEdge(Layer* layer, LTriangle* pCurFace);   //�������㷨��ʼ���������һ�����ߣ��͵�һ�����㣻
	void CalIntersectPoint(Layer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point);	// ��������
	void JudgeFaceType(Layer* layer, LTriangle* pCurFace);    //�ж�����Ƭƽ���ཻ��Ƭ������
	void JudgeOtherLine(Layer* layer, LTriangle* pCurFace);   //�ж���һ���ཻ��
//	double ReturnZmin(const LTriangle* Ltri);   //����һ����Ƭ����СZ����ֵ
//	double ReturnZmax(const LTriangle* Ltri);
//	double ReturnZmid(const LTriangle* Ltri);
	double ReturnZtype(double v1, double v2, double v3, int ztype);

public:
	void begin3DSlice(double z_min, double z_max, double& z, double dz);
	void begin5DSlice(double z_min, double z_max, double& z, double dz);
	bool isBoundaryCCW(Layer* layer);	//�������ںܴ����⣬��������ĳ��������ֵ�ǳ�Сʱ�����ܻ����һЩĪ����������� 
	void makeBoundaryCCW(Layer* layer);
//	void mergeTwoBoundary(Layer* layer1, Layer* layer2);
	void releaseMem();

	bool lineNeedSupport();

public:
	vector<LEdge*> m_Slice_edge;     //�洢�ཻ��
	vector<Layer*> m_layers;         //�洢Ƭ��
	vector<LTriangle*> m_tris_slice;   //����һ��������Ƭ����Ϊ��ʼ����
	vector<LTriangle*> z_tris;

	double height;
	CVector3D gravity;
	bool need_support;
};

