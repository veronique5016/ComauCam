#include "stdafx.h"
#include "Global.h"

#include "LVertex.h"

//using namespace std;


void LinesToPolylines(vector<CLine>& lines, vector<CPolyline*>& boundaries, bool bPlanar)
//�������ӳɶ����
{
	vector<LinkPoint*> LPs;

	int szLine = lines.size();
	for (int i = 0; i<szLine; i++)
	{
		if (lines[i].m_pt1 != lines[i].m_pt2)             //���ֱ�ߵ������˵㲻���
		{
			LinkPoint* pLP1 = new LinkPoint(lines[i].m_pt1);//�ֱ�����µ����ӵ�������
			LinkPoint* pLP2 = new LinkPoint(lines[i].m_pt2);

			pLP1->pOther = pLP2;//�������㽨����Ӧ��ϵ
			pLP2->pOther = pLP1;

			LPs.push_back(pLP1);//ȫ������
			LPs.push_back(pLP2);
		}
	}

	bPlanar ? sort(LPs.begin(), LPs.end(), smaller_planar) : sort(LPs.begin(), LPs.end(), smaller);

	int szLP = LPs.size();
	for (int i = 0; i<szLP; i++)
	{
		LPs[i]->idx = i;//��ÿ��������ID
	}

	LinkPoint* pCurLP;     //��ǰ���ӵ� 
	int initIdx = 0, curIdx;  //��ʼID ��ǰID
	bool bClosed = false;
	vector<CPoint3D*> tmp_pts;//��ʱ�Ķ�̬���� ��ŵ�
	while (true)
	{
		bClosed = false;
		curIdx = initIdx;//��ǰIDΪ��ʼID
		pCurLP = LPs[curIdx];
		tmp_pts.push_back(new CPoint3D(*pCurLP));//����ǰ���ӵ������ʱ������
		pCurLP->bUsed = true;//Ȼ���������Ϊ��ʹ��
		pCurLP = LPs[curIdx]->pOther;//�ٽ�����������һ�㸳����ǰ��
		tmp_pts.push_back(new CPoint3D(*pCurLP));//����ʱ�ĵ�ǰ�������ʱ����
		curIdx = pCurLP->idx;//Ŀǰ��ID���ǵ�ǰ���ID
		pCurLP->bUsed = true;//�ٽ��������Ϊ���� �����Ͱ�����������ĵ����������

		while (true)
		{   //�����ǰ�����ǰһ�������� ��ǰһ������δ��� ��Ŀǰ���ǰһ��ĵ�ַ��ͬ
			if ((curIdx - 1 >= 0) && false == LPs[curIdx - 1]->bUsed && (*pCurLP ^= *LPs[curIdx - 1]))
			{
				pCurLP = LPs[curIdx - 1]->pOther;//��ID-1�����һ�㸳����ǰ��
				tmp_pts.push_back(new CPoint3D(*pCurLP));//����
				LPs[curIdx - 1]->bUsed = true;//���
				pCurLP->bUsed = true;//��ǵ�ǰ
				curIdx = pCurLP->idx;
			}
			else if ((curIdx + 1<szLP) && false == LPs[curIdx + 1]->bUsed && (*pCurLP ^= *LPs[curIdx + 1])) // shouldn't have problem
			{   //��һ��ID�ĵ���� ����һ����δ��� �ҵ�ǰ�����һ���ַ��ͬ
				pCurLP = LPs[curIdx + 1]->pOther;//��ID+1�����һ�㸳����ǰ��
				tmp_pts.push_back(new CPoint3D(*pCurLP));//����
				LPs[curIdx + 1]->bUsed = true;//���
				pCurLP->bUsed = true;//��ǵ�ǰ
				curIdx = pCurLP->idx;//���¸�ID��ֵ
			}
			else
			{
				int idx = SearchInLps(pCurLP, LPs);
				if (-1 == idx)
				{
					break;//δ�ҵ�
				}
				else
				{
					pCurLP = LPs[idx]->pOther;//��ID�����һ�㸳����ǰ��
					tmp_pts.push_back(new CPoint3D(*pCurLP));//����
					LPs[idx]->bUsed = true;//���
					pCurLP->bUsed = true;//��ǵ�ǰ
					curIdx = pCurLP->idx;//���¸�ID��ֵ
				}
			}

			if (*pCurLP ^= *tmp_pts[0])
			{
				bClosed = true;//�����պ�
				break;
			}
		}

		if (false == bClosed)//  ������պ�
		{
			curIdx = initIdx;
			pCurLP = LPs[curIdx];
			while (true)
			{
				if ((curIdx - 1 >= 0) && false == LPs[curIdx - 1]->bUsed && (*pCurLP ^= *LPs[curIdx - 1]))
				{
					pCurLP = LPs[curIdx - 1]->pOther;
					tmp_pts.insert(tmp_pts.begin(), new CPoint3D(*pCurLP));//������
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
		if (2 == szPt && bClosed)   // ���ֻ�����������ұպϵ��ߣ�ֻ�����߶ηǳ���ʱ�ᷢ���������
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
				pBoundary->AddPt(tmp_pts[i]);//����ӵ�pBoundary��
			tmp_pts.clear();
			//pBoundary->DelColinearPt();//ɾ�����ߵĵ�
			boundaries.push_back(pBoundary);
		}
		initIdx = FindAnUnusedLP(LPs);//�һ�δ��ǵĵ�
		if (-1 == initIdx)
			break;//û�еĻ�����
	}


	Lineses lineses;
	double lineInterval = 0.2;

//	GntSweeplines(boundaries, lineInterval, angle, lineses, bBoundaryRotateBack = 1);


	int sz = LPs.size();
	for (int i = sz - 1; i >= 0; i--)//�ͷ�
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

int SearchInLps(LinkPoint* pLP, vector<LinkPoint*> LPs)//�����ӵ�������
{
	int sz = LPs.size();
	for (int i = 0; i<sz; i++)
	{
		if (LPs[i]->bUsed)//����ѱ����
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
	if (A.z >= z && B.z >= z && C.z >= z)//������ж���zֵ�����ڣ�˵�����ཻ
		return false;
	if (A.z <= z && B.z <= z && C.z <= z)//������ж���zֵ��С�ڣ�˵�����ཻ
		return false;

	CPoint3D A_, B_, C_;
	CVector3D vec1, vec2;
	double lamda1, lamda2;

	//��ƽ����������������
	if ((A.z - z)*(B.z - z)>0)//˵��ab��ͬһ��
	{
		A_ = C;//�����㸳��A
		B_ = A;
		C_ = B;//��������ԭ˳��һ��
		goto NO_POINT_ON_SURFACE;
	}
	if ((B.z - z)*(C.z - z)>0)//˵��cb��ͬһ��
	{
		A_ = A;//�����㸳��A
		B_ = B;
		C_ = C;//��������ԭ˳��һ��
		goto NO_POINT_ON_SURFACE;
	}
	if ((A.z - z)*(C.z - z)>0)//˵��ac��ͬһ��
	{
		A_ = B;//�����㸳��A
		B_ = A;
		C_ = C;//��������ԭ˳��һ��
		goto NO_POINT_ON_SURFACE;
	}
	//������������һ���Ƕ�������
	if (A.z == z)//ǡ����a����ƽ����
	{
		A_ = A;
		B_ = B;
		C_ = C;
		goto ONE_POINT_ON_SURFACE;
	}
	if (B.z == z)//ǡ����b����ƽ����
	{
		A_ = B;
		B_ = A;
		C_ = C;
		goto ONE_POINT_ON_SURFACE;
	}
	if (C.z == z)//ǡ����c����ƽ����
	{
		A_ = C;
		B_ = B;
		C_ = A;
		goto ONE_POINT_ON_SURFACE;
	}

NO_POINT_ON_SURFACE://û�ж�����ƽ����
	lamda1 = (z - A_.z) / (B_.z - A_.z);
	lamda2 = (z - A_.z) / (C_.z - A_.z);
	vec1 = B_ - A_;
	vec2 = C_ - A_;
	line_out.m_pt1 = A_ + vec1*lamda1;//��ͨ���˶�ʽ������������������
	line_out.m_pt2 = A_ + vec2*lamda2;//��Ϊ��һ��ֱ��������������ͬ�ı���
	line_out.m_pt1.z = z;  // ����z  
	line_out.m_pt2.z = z; //Ϊʲô��Ҫ�����أ��������ϼ�������ľ���ʵ��ֵ����Ϊ�˷�ֹ�����о��Ȳ�ͬ��
	if (line_out.m_pt1 == line_out.m_pt2)
	{
		return false;
	}
	else
	{
		return true;
	}


ONE_POINT_ON_SURFACE://ǡ��һ�������ǽ���
	lamda1 = (z - B_.z) / (C_.z - B_.z);
	vec1 = C_ - B_;
	line_out.m_pt1 = A_;
	line_out.m_pt2 = B_ + vec1*lamda1;
	line_out.m_pt1.z = z;  // ����z
	line_out.m_pt2.z = z;
	if (line_out.m_pt1 == line_out.m_pt2)//˵��ֻ��һ��������ƽ���ϣ����ֲ����ཻ
	{
		return false;
	}
	else
	{
		return true;
	}
}