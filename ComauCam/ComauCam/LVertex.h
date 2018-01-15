#pragma once
#include "GeomBase.h"
#include "OpenGL.h"
#include <vector>
using namespace std;
// ����ָ��ṹ�ĵ㡢�ߡ�������
struct LVertex;
struct LEdge;
struct LTriangle;
struct LEdgeHull;

struct LVertex : public CPoint3D
{
	LVertex();
	LVertex(const LVertex& vertex);
	LVertex(const CPoint3D& pt);
	bool operator < (const LVertex& vertex);  // ���½� �� ���Ͻ�
	bool operator > (const LVertex& vertex);
	bool IsLower(const LVertex& vertex);
	bool IsLower(LVertex* pVertex);
};

struct LEdge   // Half edge structure    ������ݽṹ
{
	LEdge();
	LEdge(LVertex* _v1, LVertex* _v2);

	LVertex   *v1, *v2;     // Two vertices, v1: start   v2: end
	LTriangle *t;              // The triangle the edge belong to  �ߴ���������Ƭ
	LEdge     *e_prev,    // previous edge
		*e_next,    // next edge
		*e_adja;    // adjacent edge together with which forms edge   �����
};

struct LTriangle : public CObject
{
	LTriangle();
	LTriangle(LVertex* v1, LVertex* v2, LVertex* v3);
	LTriangle* GetNbTri1() const;
	LTriangle* GetNbTri2() const;
	LTriangle* GetNbTri3() const;
	CVector3D* GntNormal();
	void Draw_LTriangle(COpenGLDC* pDC);   //����û������д�ĺ���ֱ���¶���ģ�ע��

	LVertex *v1, *v2, *v3;  // ��ʱ������
	LEdge   *e1, *e2, *e3;  // ���
	CVector3D* n;           // ������

	bool b_use;     //��Ƭ�Ƿ�ʹ��
	int FaceType;  //��Ƭ����
	LEdge* IntersectLine1;     //��һ��Ƭ�Ѿ��󽻵��ཻ�ߣ�����Ѱ����һ�ཻ��
	LEdge* IntersectLine2;     //��һ���ཻ��
};

struct LEdgeHull//���
{
	LEdge* edge;

	bool IsOpposite(const LEdgeHull& edgeHull);    //�ж��Ƿ�Ϊ����ߵĺ���
	bool operator < (const LEdgeHull& edgeHull);
};

struct TopologySTL    // �������˹�ϵ�ĵ����漯��
{
	TopologySTL();
	~TopologySTL();
	vector<LVertex*>   m_vertices;//����
	vector<LEdge*>     m_edges;//��
	vector<LTriangle*> m_ltris;//��
	vector<CVector3D*> m_normals;//����
};

