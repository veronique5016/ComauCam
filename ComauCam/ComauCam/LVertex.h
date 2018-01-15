#pragma once
#include "GeomBase.h"
#include "OpenGL.h"
#include <vector>
using namespace std;
// 定义指针结构的点、边、三角形
struct LVertex;
struct LEdge;
struct LTriangle;
struct LEdgeHull;

struct LVertex : public CPoint3D
{
	LVertex();
	LVertex(const LVertex& vertex);
	LVertex(const CPoint3D& pt);
	bool operator < (const LVertex& vertex);  // 左下角 到 右上角
	bool operator > (const LVertex& vertex);
	bool IsLower(const LVertex& vertex);
	bool IsLower(LVertex* pVertex);
};

struct LEdge   // Half edge structure    半边数据结构
{
	LEdge();
	LEdge(LVertex* _v1, LVertex* _v2);

	LVertex   *v1, *v2;     // Two vertices, v1: start   v2: end
	LTriangle *t;              // The triangle the edge belong to  边从属的三角片
	LEdge     *e_prev,    // previous edge
		*e_next,    // next edge
		*e_adja;    // adjacent edge together with which forms edge   伙伴半边
};

struct LTriangle : public CObject
{
	LTriangle();
	LTriangle(LVertex* v1, LVertex* v2, LVertex* v3);
	LTriangle* GetNbTri1() const;
	LTriangle* GetNbTri2() const;
	LTriangle* GetNbTri3() const;
	CVector3D* GntNormal();
	void Draw_LTriangle(COpenGLDC* pDC);   //这里没有用重写的函数直接新定义的，注意

	LVertex *v1, *v2, *v3;  // 逆时针排列
	LEdge   *e1, *e2, *e3;  // 半边
	CVector3D* n;           // 法向量

	bool b_use;     //面片是否被使用
	int FaceType;  //面片类型
	LEdge* IntersectLine1;     //上一面片已经求交的相交线，用于寻找下一相交线
	LEdge* IntersectLine2;     //另一条相交线
};

struct LEdgeHull//外壳
{
	LEdge* edge;

	bool IsOpposite(const LEdgeHull& edgeHull);    //判断是否为伙伴半边的函数
	bool operator < (const LEdgeHull& edgeHull);
};

struct TopologySTL    // 具有拓扑关系的点线面集合
{
	TopologySTL();
	~TopologySTL();
	vector<LVertex*>   m_vertices;//顶点
	vector<LEdge*>     m_edges;//边
	vector<LTriangle*> m_ltris;//面
	vector<CVector3D*> m_normals;//向量
};

