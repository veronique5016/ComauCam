#include "stdafx.h"
#include "Global.h"

#include "LVertex.h"

//using namespace std;


void LinesToPolylines(vector<CLine>& lines, vector<CPolyline*>& boundaries, bool bPlanar)
//将线连接成多边形
{
	vector<LinkPoint*> LPs;

	int szLine = lines.size();
	for (int i = 0; i<szLine; i++)
	{
		if (lines[i].m_pt1 != lines[i].m_pt2)             //如果直线的两个端点不相等
		{
			LinkPoint* pLP1 = new LinkPoint(lines[i].m_pt1);//分别存入新的连接点数组中
			LinkPoint* pLP2 = new LinkPoint(lines[i].m_pt2);

			pLP1->pOther = pLP2;//将两个点建立对应关系
			pLP2->pOther = pLP1;

			LPs.push_back(pLP1);//全部存入
			LPs.push_back(pLP2);
		}
	}

	bPlanar ? sort(LPs.begin(), LPs.end(), smaller_planar) : sort(LPs.begin(), LPs.end(), smaller);

	int szLP = LPs.size();
	for (int i = 0; i<szLP; i++)
	{
		LPs[i]->idx = i;//给每个点设置ID
	}

	LinkPoint* pCurLP;     //当前连接点 
	int initIdx = 0, curIdx;  //初始ID 当前ID
	bool bClosed = false;
	vector<CPoint3D*> tmp_pts;//暂时的动态数组 存放点
	while (true)
	{
		bClosed = false;
		curIdx = initIdx;//当前ID为初始ID
		pCurLP = LPs[curIdx];
		tmp_pts.push_back(new CPoint3D(*pCurLP));//将当前连接点存入暂时点数组
		pCurLP->bUsed = true;//然后将这个点标记为已使用
		pCurLP = LPs[curIdx]->pOther;//再将这个点的另外一点赋给当前点
		tmp_pts.push_back(new CPoint3D(*pCurLP));//将此时的当前点存入暂时数组
		curIdx = pCurLP->idx;//目前的ID就是当前点的ID
		pCurLP->bUsed = true;//再将这个点标记为已用 这样就把两个相关联的点存入了数组

		while (true)
		{   //如果当前点存在前一个关联点 且前一关联点未标记 且目前点和前一点的地址不同
			if ((curIdx - 1 >= 0) && false == LPs[curIdx - 1]->bUsed && (*pCurLP ^= *LPs[curIdx - 1]))
			{
				pCurLP = LPs[curIdx - 1]->pOther;//将ID-1点的另一点赋给当前点
				tmp_pts.push_back(new CPoint3D(*pCurLP));//存入
				LPs[curIdx - 1]->bUsed = true;//标记
				pCurLP->bUsed = true;//标记当前
				curIdx = pCurLP->idx;
			}
			else if ((curIdx + 1<szLP) && false == LPs[curIdx + 1]->bUsed && (*pCurLP ^= *LPs[curIdx + 1])) // shouldn't have problem
			{   //下一个ID的点存在 且下一个点未标记 且当前点和下一点地址不同
				pCurLP = LPs[curIdx + 1]->pOther;//将ID+1点的另一点赋给当前点
				tmp_pts.push_back(new CPoint3D(*pCurLP));//存入
				LPs[curIdx + 1]->bUsed = true;//标记
				pCurLP->bUsed = true;//标记当前
				curIdx = pCurLP->idx;//重新给ID赋值
			}
			else
			{
				int idx = SearchInLps(pCurLP, LPs);
				if (-1 == idx)
				{
					break;//未找到
				}
				else
				{
					pCurLP = LPs[idx]->pOther;//将ID点的另一点赋给当前点
					tmp_pts.push_back(new CPoint3D(*pCurLP));//存入
					LPs[idx]->bUsed = true;//标记
					pCurLP->bUsed = true;//标记当前
					curIdx = pCurLP->idx;//重新给ID赋值
				}
			}

			if (*pCurLP ^= *tmp_pts[0])
			{
				bClosed = true;//轮廓闭合
				break;
			}
		}

		if (false == bClosed)//  如果不闭合
		{
			curIdx = initIdx;
			pCurLP = LPs[curIdx];
			while (true)
			{
				if ((curIdx - 1 >= 0) && false == LPs[curIdx - 1]->bUsed && (*pCurLP ^= *LPs[curIdx - 1]))
				{
					pCurLP = LPs[curIdx - 1]->pOther;
					tmp_pts.insert(tmp_pts.begin(), new CPoint3D(*pCurLP));//？？？
					LPs[curIdx - 1]->bUsed = true;
					pCurLP->bUsed = true;
					curIdx = pCurLP->idx;
				}
				else if ((curIdx + 1<szLP) && false == LPs[curIdx + 1]->bUsed && (*pCurLP ^= *LPs[curIdx + 1]))
				{
					pCurLP = LPs[curIdx + 1]->pOther;
					tmp_pts.insert(tmp_pts.begin(), new CPoint3D(*pCurLP));
					LPs[curIdx + 1]->bUsed = true;
					pCurLP->bUsed = true;
					curIdx = pCurLP->idx;
				}
				else
				{
					int idx = SearchInLps(pCurLP, LPs);
					if (-1 == idx)
					{
						break;
					}
					else
					{
						pCurLP = LPs[idx]->pOther;
						tmp_pts.insert(tmp_pts.begin(), new CPoint3D(*pCurLP));
						LPs[idx]->bUsed = true;
						pCurLP->bUsed = true;
						curIdx = pCurLP->idx;
					}
				}
			}
		}
		int szPt = tmp_pts.size();
		if (2 == szPt && bClosed)   // 清除只有两个顶点且闭合的线，只有在线段非常短时会发生这种情况
		{
			for (int i = 0; i<szPt; i++)
			{
				delete tmp_pts[i];
				tmp_pts[i] = NULL;
			}
			tmp_pts.clear();
		}
		else
		{
			CPolyline* pBoundary = new CPolyline();
			pBoundary->m_bIsClosed = bClosed;
			for (int i = 0; i<szPt; i++)
				pBoundary->AddPt(tmp_pts[i]);//将点加到pBoundary里
			tmp_pts.clear();
			//pBoundary->DelColinearPt();//删除共线的点
			boundaries.push_back(pBoundary);
		}
		initIdx = FindAnUnusedLP(LPs);//找还未标记的点
		if (-1 == initIdx)
			break;//没有的话跳出
	}


	Lineses lineses;
	double lineInterval = 0.2;

//	GntSweeplines(boundaries, lineInterval, angle, lineses, bBoundaryRotateBack = 1);


	int sz = LPs.size();
	for (int i = sz - 1; i >= 0; i--)//释放
	{
		delete LPs[i];
		LPs[i] = NULL;
	}
	LPs.clear();

}

LinkPoint::LinkPoint()
{
	pOther = NULL;
	idx = -1;
	bUsed = false;
}

LinkPoint::LinkPoint(const CPoint3D& pt)
{
	x = pt.x;
	y = pt.y;
	z = pt.z;
	bUsed = false;
}

int SearchInLps(LinkPoint* pLP, vector<LinkPoint*> LPs)//在连接点中搜索
{
	int sz = LPs.size();
	for (int i = 0; i<sz; i++)
	{
		if (LPs[i]->bUsed)//如果已被标记
		{
			continue;
		}

		if (*pLP ^= *LPs[i])
		{
			return i;
		}
	}

	return -1;
}

int FindAnUnusedLP(vector<LinkPoint*> LPs)
{
	int sz = LPs.size();
	for (int i = 0; i<sz; i++)
	{
		if (false == LPs[i]->bUsed)
		{
			return i;
		}
	}

	return -1;
}

bool smaller_planar(LinkPoint* sp1, LinkPoint* sp2)
{
	if ((sp1->x < sp2->x) ||
		((sp1->x == sp2->x) && (sp1->y < sp2->y)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool smaller(LinkPoint* sp1, LinkPoint* sp2)
{
	if (((float)sp1->x <  (float)sp2->x) ||
		(((float)sp1->x == (float)sp2->x) && ((float)sp1->y <  (float)sp2->y)) ||
		(((float)sp1->x == (float)sp2->x) && ((float)sp1->y == (float)sp2->y) && ((float)sp1->z < (float)sp2->z)))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CalcTriXYPlaneInst(const CPoint3D& A, const CPoint3D& B, const CPoint3D& C, double z, CLine& line_out)
{
	if (A.z >= z && B.z >= z && C.z >= z)//如果所有顶点z值都大于，说明不相交
		return false;
	if (A.z <= z && B.z <= z && C.z <= z)//如果所有顶点z值都小于，说明不相交
		return false;

	CPoint3D A_, B_, C_;
	CVector3D vec1, vec2;
	double lamda1, lamda2;

	//与平面有两个交点的情况
	if ((A.z - z)*(B.z - z)>0)//说明ab在同一侧
	{
		A_ = C;//孤立点赋给A
		B_ = A;
		C_ = B;//其他点与原顺序一致
		goto NO_POINT_ON_SURFACE;
	}
	if ((B.z - z)*(C.z - z)>0)//说明cb在同一侧
	{
		A_ = A;//孤立点赋给A
		B_ = B;
		C_ = C;//其他点与原顺序一致
		goto NO_POINT_ON_SURFACE;
	}
	if ((A.z - z)*(C.z - z)>0)//说明ac在同一侧
	{
		A_ = B;//孤立点赋给A
		B_ = A;
		C_ = C;//其他点与原顺序一致
		goto NO_POINT_ON_SURFACE;
	}
	//两个交点其中一个是顶点的情况
	if (A.z == z)//恰好是a点在平面上
	{
		A_ = A;
		B_ = B;
		C_ = C;
		goto ONE_POINT_ON_SURFACE;
	}
	if (B.z == z)//恰好是b点在平面上
	{
		A_ = B;
		B_ = A;
		C_ = C;
		goto ONE_POINT_ON_SURFACE;
	}
	if (C.z == z)//恰好是c点在平面上
	{
		A_ = C;
		B_ = B;
		C_ = A;
		goto ONE_POINT_ON_SURFACE;
	}

NO_POINT_ON_SURFACE://没有顶点在平面上
	lamda1 = (z - A_.z) / (B_.z - A_.z);
	lamda2 = (z - A_.z) / (C_.z - A_.z);
	vec1 = B_ - A_;
	vec2 = C_ - A_;
	line_out.m_pt1 = A_ + vec1*lamda1;//即通过此二式算出了两个交点的坐标
	line_out.m_pt2 = A_ + vec2*lamda2;//因为在一条直线上坐标满足相同的比例
	line_out.m_pt1.z = z;  // 修正z  
	line_out.m_pt2.z = z; //为什么需要修正呢？？理论上计算出来的就是实际值，是为了防止计算中精度不同吗
	if (line_out.m_pt1 == line_out.m_pt2)
	{
		return false;
	}
	else
	{
		return true;
	}


ONE_POINT_ON_SURFACE://恰好一个顶点是交点
	lamda1 = (z - B_.z) / (C_.z - B_.z);
	vec1 = C_ - B_;
	line_out.m_pt1 = A_;
	line_out.m_pt2 = B_ + vec1*lamda1;
	line_out.m_pt1.z = z;  // 修正z
	line_out.m_pt2.z = z;
	if (line_out.m_pt1 == line_out.m_pt2)//说明只有一个顶点在平面上，这种不算相交
	{
		return false;
	}
	else
	{
		return true;
	}
}