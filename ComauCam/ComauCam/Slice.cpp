#include "stdafx.h"
#include "Slice.h"
#include <algorithm>

PolyLine::PolyLine()
{
}
PolyLine::~PolyLine()
{
}

Layer::Layer()
{
}
Layer::~Layer()
{
}

CSlice::CSlice(void)
{

}

CSlice::~CSlice(void)
{

}

void CSlice::LoadSTLModel(CSTLModel* model)//载入stl模型
{
	model->Topologize();//给模型建立拓扑结构
	int szTri = model->m_ltris.size();
	for (int j = 0; j<szTri; j++)
	{
		m_tris_slice.push_back(model->m_ltris[j]);     //为CSlice对象拷贝一份三角面片数据
	}
}
void CSlice::slice(CSTLModel* model)
{
	AfxMessageBox(_T("Slice Begin!!"), MB_OK, 0);
	double z_Min, z_Max;  //模型极限尺寸
	double z;  //切片高度
	double dz;   //切片层厚
	double ex[6];
	model->FindExtreme(ex);   //查找模型的极限尺寸
	z_Min = ex[4];
	z_Max = ex[5];
	z = z_Min + 0.1;  //第一个切片层
	dz = 0.2;   //层高
	while (true)
	{
		getpolylinePoints(z);
		if (z >= (z_Max - dz))
		{
			break;
		}
		z += dz;
	}

}

void CSlice::getpolylinePoints(double z)
{
	vector<LTriangle*> status; //储存符合层高的，筛选出来的面片
	int szTri = m_tris_slice.size();
	for (int i = 0; i<szTri; i++)         //遍历所有面片,筛选出相交面片
	{
		double z_min = ReturnZmin(m_tris_slice[i]);
		double z_max = ReturnZmax(m_tris_slice[i]);
		if (z_min <= z&&z_max >= z)
		{
			if (z_min == z_max)
			{
				//跳过面片与切片重合的面片
			}
			else {
				status.push_back(m_tris_slice[i]);
			}       //存储与切片层相交面片
		}
	}
	int szStatus = status.size();
	for (int i = 0; i<szStatus; i++)       //对相交面片进行面片分类
	{
		JudgeFaceType(z, status[i]);
	}
	LTriangle* pCurFace = new LTriangle();
	//选第一个面片作为起始面片
	pCurFace = status[0];
	pCurFace->b_use = true;//把面片置为当前使用面片，标志位置为真
	while (true)
	{
		PolyLine* m_polyline = new PolyLine();
		InterSect(z, pCurFace);  //设定起始相交边，并存入m_Slice_edge容器
		CPoint3D* tmpLinkPoint = new CPoint3D();
		int sz0 = m_Slice_edge.size();
		pCurFace->SelectIntersectLine = m_Slice_edge[sz0 - 1];
		//由起始相交边求交点，并存入m_LinkPoint容器
		CalInerSectPoint(z, pCurFace->SelectIntersectLine, pCurFace, tmpLinkPoint);
		m_polyline->m_Linkpoints.push_back(new CPoint3D(*tmpLinkPoint));
		//第一个面片的另一相交边求交点，并存入m_LinkPoint容器
		JudgeOtherLine(z, pCurFace);
		CalInerSectPoint(z, pCurFace->OtherIntersectLine, pCurFace, tmpLinkPoint);
		m_polyline->m_Linkpoints.push_back(new CPoint3D(*tmpLinkPoint));
		//上面是轮廓第一个面片的特殊处理
		while (true)
		{
			//由最后存入相交边数组的边的伙伴半边寻找下一相交面片
			int sz = m_Slice_edge.size();
			pCurFace = m_Slice_edge[sz - 1]->e_adja->t;
			pCurFace->b_use = true;//把面片置为当前使用面片，标志位置为真
								   //最后存入m_Slice_edge的相交边是上一面片的第二条相交边，它的伙伴半边就是当前面片已选的相交边
			pCurFace->SelectIntersectLine = m_Slice_edge[sz - 1]->e_adja;
			//判断当前面片中的另一相交边，保存边
			JudgeOtherLine(z, pCurFace);
			//求交点，并保持点
			CalInerSectPoint(z, pCurFace->OtherIntersectLine, pCurFace, tmpLinkPoint);
			m_polyline->m_Linkpoints.push_back(new CPoint3D(*tmpLinkPoint));
			int szlinkPoint = m_polyline->m_Linkpoints.size();
			//判断交点是否为起始点，若是，跳出内层循环，此时生成一个轮廓点集，若否，直接返回内层循环
			if (*(m_polyline->m_Linkpoints[0]) ^= *(m_polyline->m_Linkpoints[szlinkPoint - 1]))
			{
				Layer* pLayer = new Layer();
				pLayer->z = z;
				pLayer->m_Polylines.push_back(m_polyline);
				m_layers.push_back(pLayer);
				int szpoint = m_polyline->m_Linkpoints.size();
				break;
			}
		}
		//跳出内层循环来到这，判断status中是否还有未被使用的面片，
		//如有，把面片置为当前，开始下一轮廓寻点；如没有，跳出外层循环
		break;
	}

}

void CSlice::drawpolyline()
{
	int szlayer = m_layers.size();
	for (int i = 0; i<szlayer; i++)
	{
		int szpolyline = m_layers[i]->m_Polylines.size();
		for (int j = 0; j<szpolyline; j++)
		{
			int szPoint = m_layers[i]->m_Polylines[j]->m_Linkpoints.size();
			for (int k = 0; k<(szPoint-1); k++)
			{
				CPoint3D* point1 = new CPoint3D(*(m_layers[i]->m_Polylines[j]->m_Linkpoints[k]));
				CPoint3D* point2 = new CPoint3D(*(m_layers[i]->m_Polylines[j]->m_Linkpoints[k + 1]));
				glLineWidth(1.5f);
				glBegin(GL_LINES);
				glColor3f(0.0f, 1.0f, 0.0f);
				glVertex3f(point1->x, point1->y, point1->z);
				glVertex3f(point2->x, point2->y, point2->z);
				glEnd();
			}
		}
	}

}

void CSlice::InterSect(double z, LTriangle* pCurFace)   //只要找到一条相交的边就可以跳出
{
	switch (pCurFace->FaceType)
	{
	case NO_POINT_ON_SURFACE:   //不相交的边不能选
	{
		if ((pCurFace->v1->z - z)*(pCurFace->v2->z - z)<0)   //说明e1边可以作为第一条相交的边
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT1;
		}
		if ((pCurFace->v2->z - z)*(pCurFace->v3->z - z)<0)   //说明e2边可以作为第一条相交的边,第一条边不相交，第二条边肯定相交
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT1;
		}
	ENDINIT1:
		break;
	}
	case ONE_POINT_ON_SURFACE:  //因为三条边都相交，随便一条都可以作为起始边，选e1好了
	{
		m_Slice_edge.push_back(pCurFace->e1);
		break;
	}
	case EDGE_ON_SURFACE:  //不能选在切平面上的边
	{
		if ((pCurFace->v1->z - z) != 0)   //v1不在切平面上，与之相连的两条边e1\e3都可选
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT2;
		}
		if ((pCurFace->v2->z - z) != 0)   //v2不在切平面上，与之相连的两条边e1\e2都可选
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT2;
		}
		if ((pCurFace->v3->z - z) != 0)   //v3不在切平面上，与之相连的两条边e2\e3都可选
		{
			m_Slice_edge.push_back(pCurFace->e3);
			goto ENDINIT2;
		}
	ENDINIT2:
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:
	{
		if ((pCurFace->v1->z - z) == 0)   //v1在切平面上，与之相连的两条边e1\e3都可选
		{
			m_Slice_edge.push_back(pCurFace->e1);
			goto ENDINIT3;
		}
		if ((pCurFace->v2->z - z) == 0)   //v2在切平面上，与之相连的两条边e1\e2都可选
		{
			m_Slice_edge.push_back(pCurFace->e2);
			goto ENDINIT3;
		}
		if ((pCurFace->v3->z - z) == 0)   //v3在切平面上，与之相连的两条边e2\e3都可选
		{
			m_Slice_edge.push_back(pCurFace->e3);
			goto ENDINIT3;
		}
	ENDINIT3:
		break;
	}
	}

}

void CSlice::JudgeFaceType(double z, LTriangle* pCurFace)
{
	double z_min = ReturnZmin(pCurFace);
	double z_max = ReturnZmax(pCurFace);
	double z_mid = ReturnZmid(pCurFace);
	//判断三角面片和切平面的相交情况,四种情况
	if (z_min<z&&z_max>z&&z_mid != z)
		pCurFace->FaceType = NO_POINT_ON_SURFACE;
	if (z_min<z&&z_max>z&&z_mid == z)
		pCurFace->FaceType = ONE_POINT_ON_SURFACE;
	if ((z_min<z&&z_max == z&&z_mid == z) || (z_min == z&&z_max>z&&z_mid == z))
		pCurFace->FaceType = EDGE_ON_SURFACE;
	if ((z_min<z&&z_max == z&&z_mid != z) || (z_min == z&&z_max>z&&z_mid != z))
		pCurFace->FaceType = ONLY_ONE_POINT_ON_SURFACE;
}

void CSlice::JudgeOtherLine(double z, LTriangle* pCurFace)
{
	switch (pCurFace->FaceType)
	{
	case NO_POINT_ON_SURFACE:
	{
		//已选的相交边的相邻前一边判断是否相交
		if ((pCurFace->SelectIntersectLine->e_prev->v1->z - z)*(pCurFace->SelectIntersectLine->e_prev->v2->z - z)<0)
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		else//否则另一条就是相交边
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		break;
	}
	case ONE_POINT_ON_SURFACE:  //这种情况比较特殊，要先判断已选的相交边是哪种类型
	{
		//已选边的一端点在切平面上
		if ((pCurFace->SelectIntersectLine->v1->z - z)*(pCurFace->SelectIntersectLine->v2->z - z) == 0)
		{
			//已选的相交边的相邻前一边判断是否相交
			if ((pCurFace->SelectIntersectLine->e_prev->v1->z - z)*(pCurFace->SelectIntersectLine->e_prev->v2->z - z)<0)
				pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
			else//否则另一条就是相交边
				pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		}
		else//已选边的一端点不在切平面上，/任意一条相邻的边都可以作为另一相交边
		{
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		}
		break;
	}
	case EDGE_ON_SURFACE:  //相交不能选在切平面上的边
	{
		//已选的相交边的相邻前一边判断是否有一个点不在z切平面上
		if ((pCurFace->SelectIntersectLine->e_prev->v1->z - z) != 0 || (pCurFace->SelectIntersectLine->e_prev->v2->z - z) != 0)
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		else//否则另一条就是相交边
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		break;
	}
	case ONLY_ONE_POINT_ON_SURFACE:  //哪一条边其中的一个端点在切平面上，就是相交边
	{

		//已选的相交边的相邻前一边判断是否有一个点不在z切平面上
		if ((pCurFace->SelectIntersectLine->e_prev->v1->z - z) == 0)
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_prev;
		else//否则另一条就是相交边
			pCurFace->OtherIntersectLine = pCurFace->SelectIntersectLine->e_next;
		break;
	}
	}
	m_Slice_edge.push_back(pCurFace->OtherIntersectLine);
}


void CSlice::CalInerSectPoint(double z, LEdge * edge, LTriangle*pCurFace, CPoint3D* point)
{
	if (pCurFace->FaceType != ONLY_ONE_POINT_ON_SURFACE) {
		if (edge->v1->z == z)   //如果点和切面重合，把端点赋值给交点
		{
			point->x = edge->v1->x;
			point->y = edge->v1->y;
		}
		else if (edge->v2->z == z)
		{
			point->x = edge->v2->x;
			point->y = edge->v2->y;
		}
		else    //两端点都不在切片面上
		{
			double lamda;
			CPoint3D* vec = new CPoint3D();
			lamda = (z - edge->v1->z) / (edge->v2->z - edge->v1->z);
			vec->x = edge->v2->x - edge->v1->x;
			vec->y = edge->v2->y - edge->v1->y;
			point->x = edge->v1->x + lamda*vec->x;
			point->y = edge->v1->y + lamda*vec->y;
		}
		point->z = z;
	}
}

double CSlice::ReturnZmin(const LTriangle* Ltri)
{
	double z1, z2, z3;
	z1 = Ltri->v1->z;
	z2 = Ltri->v2->z;
	z3 = Ltri->v3->z;
	if (z1>z2)
	{
		swap(z1, z2);
	}
	if (z2>z3)
	{
		swap(z2, z3);
	}
	if (z1>z2)
	{
		swap(z1, z2);
	}
	return z1;

}
double CSlice::ReturnZmax(const LTriangle* Ltri)
{
	double z1, z2, z3;
	z1 = Ltri->v1->z;
	z2 = Ltri->v2->z;
	z3 = Ltri->v3->z;
	if (z1>z2)
	{
		swap(z1, z2);
	}
	if (z2>z3)
	{
		swap(z2, z3);
	}
	if (z1>z2)
	{
		swap(z1, z2);
	}
	return z3;
}

double CSlice::ReturnZmid(const LTriangle* Ltri)
{
	double z1, z2, z3;
	z1 = Ltri->v1->z;
	z2 = Ltri->v2->z;
	z3 = Ltri->v3->z;
	if (z1>z2)
	{
		swap(z1, z2);
	}
	if (z2>z3)
	{
		swap(z2, z3);
	}
	if (z1>z2)
	{
		swap(z1, z2);
	}
	return z2;
}

