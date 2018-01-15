#pragma once
#include "GeomBase.h"
#include <vector>
using namespace std;
// 定义指针结构的点、边、三角形
struct CLVertex;
struct CLEdge;
struct CLTriangle;
struct CLEdgeHull;

struct CLVertex : public CPoint3D
{
	CLVertex();
	CLVertex(const CLVertex& vertex);
	CLVertex(const CPoint3D& pt);
	bool operator < (const CLVertex& vertex);  // 左下角 到 右上角
	bool operator > (const CLVertex& vertex);
	bool IsLower(const CLVertex& vertex);
	bool IsLower(CLVertex* pVertex);
};

struct CLEdge   // Half edge structure    半边数据结构
{
	CLEdge();
	CLEdge(CLVertex* _v1, CLVertex* _v2);

	CLVertex   *v1, *v2;     // Two vertices, v1: start   v2: end
	CLTriangle *t;              // The triangle the edge belong to  边从属的三角片
	CLEdge     *e_prev,    // previous edge
		*e_next,    // next edge
		*e_adja;    // adjacent edge together with which forms edge   伙伴半边
};

struct CLTriangle : public CObject
{
	CLTriangle();
	CLTriangle(CLVertex* v1, CLVertex* v2, CLVertex* v3);
	CLTriangle* GetNbTri1() const;
	CLTriangle* GetNbTri2() const;
	CLTriangle* GetNbTri3() const;
	CVector3D* GntNormal();

	CLVertex *v1, *v2, *v3;  // 逆时针排列
	CLEdge   *e1, *e2, *e3;  // 半边
	CVector3D* n;           // 法向量

	bool m_bUse;     //面片是否被使用
	int m_nFaceType;  //面片类型
	CLEdge* m_pIntersectLine1;     //上一面片已经求交的相交线，用于寻找下一相交线
	CLEdge* m_pIntersectLine2;     //另一条相交线
};

struct CLEdgeHull//外壳
{
	CLEdge* m_pEdge;

	bool IsOpposite(const CLEdgeHull& edgeHull);    //判断是否为伙伴半边的函数
	bool operator < (const CLEdgeHull& edgeHull);
};

struct CTopologySTL    // 具有拓扑关系的点线面集合
{
	CTopologySTL();
	~CTopologySTL();
	vector<CLVertex*>   m_vecpVertices;//顶点
	vector<CLEdge*>     m_vecpEdges;//边
	vector<CLTriangle*> m_vecpLTris;//面
	vector<CVector3D*> m_vecpNormals;//向量
};

