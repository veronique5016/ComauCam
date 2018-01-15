#pragma once
#include <algorithm>
#include <vector>

#include "Line.h"
#include "Polyline.h"

using namespace std;

typedef vector<vector<CLine>> Lineses;

void LinesToPolylines(vector<CLine>& lines, vector<CPolyline*> &boundaries, bool bPlanar);

struct LinkPoint : public CPoint3D
{
	LinkPoint();

	LinkPoint(const CPoint3D& pt);
	const LinkPoint& operator=(const LinkPoint& ptLink)
	{
		x = ptLink.x;
		y = ptLink.y;
		z = ptLink.z;
		pOther = ptLink.pOther;
		idx = ptLink.idx;
		bUsed = ptLink.bUsed;
		return *this;
	}
	LinkPoint* pOther;
	bool bUsed;
	int  idx;

};

int SearchInLps(LinkPoint* pLP, vector<LinkPoint*> LPs);//在连接点中搜索
int FindAnUnusedLP(vector<LinkPoint*> LPs);
bool smaller_planar(LinkPoint* sp1, LinkPoint* sp2);
bool smaller(LinkPoint* sp1, LinkPoint* sp2);
