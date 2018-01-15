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

#define MIN 1
#define MID 2
#define MAX 3
struct Layer;

struct LPoint : public CPoint3D
{
public:
	LPoint();
	~LPoint();
	LPoint(const LPoint& lpoint);
	LPoint(const CPoint3D& pt);
	LPoint(double x, double y, double z);

public:
	LPoint *p_prev, *p_next;
};

struct LLine
{
public:
	LLine();
	LLine(const LPoint* startpoint, const LPoint* endpoint);
	~LLine();

public:
	LPoint pstart, pend;
	Layer* layer;
};

struct PolyLine
{
public:
	PolyLine();
	~PolyLine();

public:
	vector<LPoint*> m_Linkpoints;
	vector<LLine*> m_Linklines;
};

struct Layer
{
public:
	Layer();
	~Layer();

public:
	CPoint3D layerPoint;
	CVector3D layer_coordinate[3];
	vector<PolyLine*> m_Polylines;   //�ؽ��õ��������������޸� size = 1
};

class CSlice
{
public:
	CSlice(void);
	~CSlice(void);
	void loadSTLModel(CSTLModel* models);	//����ģ�ͣ��������˹�ϵ
	void slice(CSTLModel* model);  //��Ƭ����
	void getPolylinePoints(Layer* layer);
	void drawLayer(bool showPolygon, int start, int end);
	double getTurnHeight();     // ��ȡģ��ת�۵�
	CVector3D getTurnVec();

public:
	void getInterSectEdge(Layer* layer, LTriangle* pCurFace);   //�������㷨��ʼ���������һ�����ߣ��͵�һ�����㣻
	void calIntersectPoint(Layer* layer, LEdge * edge, LTriangle*pCurFace, LPoint* point);	// ��������
	void judgeFaceType(Layer* layer, LTriangle* pCurFace);    //�ж�����Ƭƽ���ཻ��Ƭ������
	void judgeOtherLine(Layer* layer, LTriangle* pCurFace);   //�ж���һ���ཻ��
	double compareThreeNumber(double v1, double v2, double v3, int type);

public:
	void begin3DSlice(double z_min, double z_max, double& z, double dz);
	void begin5DSlice(double z_min, double z_max, double& z, double dz);
	bool isBoundaryCCW(Layer* layer);	//�������ںܴ����⣬��������ĳ��������ֵ�ǳ�Сʱ�����ܻ����һЩĪ����������� 
	void makeBoundaryCCW(Layer* layer);
	void deletePoints(Layer* layer);
	bool lineNeedSupport();

public:
	vector<LEdge*> m_Slice_edge;     //�洢�ཻ��
	vector<Layer*> m_layers;         //�洢Ƭ��
	vector<LTriangle*> m_tris_slice;   //����һ��������Ƭ����Ϊ��ʼ����
	vector<LTriangle*> z_tris;

	double height;

	bool need_support;
};

