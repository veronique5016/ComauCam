#pragma once
#include "GeomBase.h"
#include <vector>
using namespace std;
// ����ָ��ṹ�ĵ㡢�ߡ�������
struct CLVertex;
struct CLEdge;
struct CLTriangle;
struct CLEdgeHull;

struct CLVertex : public CPoint3D
{
	CLVertex();
	CLVertex(const CLVertex& vertex);
	CLVertex(const CPoint3D& pt);
	bool operator < (const CLVertex& vertex);  // ���½� �� ���Ͻ�
	bool operator > (const CLVertex& vertex);
	bool IsLower(const CLVertex& vertex);
	bool IsLower(CLVertex* pVertex);
};

struct CLEdge   // Half edge structure    ������ݽṹ
{
	CLEdge();
	CLEdge(CLVertex* _v1, CLVertex* _v2);

	CLVertex   *v1, *v2;     // Two vertices, v1: start   v2: end
	CLTriangle *t;              // The triangle the edge belong to  �ߴ���������Ƭ
	CLEdge     *e_prev,    // previous edge
		*e_next,    // next edge
		*e_adja;    // adjacent edge together with which forms edge   �����
};

struct CLTriangle : public CObject
{
	CLTriangle();
	CLTriangle(CLVertex* v1, CLVertex* v2, CLVertex* v3);
	CLTriangle* GetNbTri1() const;
	CLTriangle* GetNbTri2() const;
	CLTriangle* GetNbTri3() const;
	CVector3D* GntNormal();

	CLVertex *v1, *v2, *v3;  // ��ʱ������
	CLEdge   *e1, *e2, *e3;  // ���
	CVector3D* n;           // ������

	bool m_bUse;     //��Ƭ�Ƿ�ʹ��
	int m_nFaceType;  //��Ƭ����
	CLEdge* m_pIntersectLine1;     //��һ��Ƭ�Ѿ��󽻵��ཻ�ߣ�����Ѱ����һ�ཻ��
	CLEdge* m_pIntersectLine2;     //��һ���ཻ��
};

struct CLEdgeHull//���
{
	CLEdge* m_pEdge;

	bool IsOpposite(const CLEdgeHull& edgeHull);    //�ж��Ƿ�Ϊ����ߵĺ���
	bool operator < (const CLEdgeHull& edgeHull);
};

struct CTopologySTL    // �������˹�ϵ�ĵ����漯��
{
	CTopologySTL();
	~CTopologySTL();
	vector<CLVertex*>   m_vecpVertices;//����
	vector<CLEdge*>     m_vecpEdges;//��
	vector<CLTriangle*> m_vecpLTris;//��
	vector<CVector3D*> m_vecpNormals;//����
};

