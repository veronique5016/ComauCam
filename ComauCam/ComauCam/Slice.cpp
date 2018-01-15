#include "stdafx.h"
#include "Slice.h"
#include "Global.h"
#include "Slice.h"
#include <fstream>

using namespace std;
//#include "SliceDlg.h"
double g_baseLevel = -10;

//double maxh;
//double minh;
//double c;
double g_maxLayerHeight = 0.2;
double g_minLayerHeight = 0.1;
double g_stairError = 0.1;
struct CSlice::Triangle
{
	Triangle(LVertex* v1, LVertex* v2, LVertex* v3, CVector3D* n, CSliceModel* pSlice)
	{
		Init(v1, v2, v3, n, pSlice);
	}
	Triangle(LTriangle* pTri, CSliceModel* pSlice)
	{
		Init(pTri->v1, pTri->v2, pTri->v3, pTri->n, pSlice);
	}

	void Init(LVertex* v1, LVertex* v2, LVertex* v3, CVector3D* n, CSliceModel* pSlice)
	{
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
		this->n = n;
		this->pSliceModel = pSlice;
		AdjustVertices();
	}

	void AdjustVertices()//调整最高点
	{
		if (v3->IsLower(v2))
		{
			swap(v2, v3);
		}
		if (v2->IsLower(v1))
		{
			swap(v1, v2);
		}
		if (v3->IsLower(v2))
		{
			swap(v2, v3);
		}
	}

	LVertex* GetVertex(int idx) const//根据参数返回顶点
	{
		switch (idx)
		{
		case 1:
			return v1;
		case 2:
			return v2;
		case 3:
			return v3;
		}
	}
	CSTLModel* GetModel() const
	{
		//return pModel;
	}

	static bool IsLower(Triangle* pTri1, Triangle* pTri2)
	{
		return pTri1->v1->IsLower(pTri2->v1);
	}

	//protected:
	LVertex      *v1, *v2, *v3;     // 指向三个顶点的指针,v1 最低， v3 最高
	CVector3D*   n;                 // 用于计算特征高度
	double       eigenH;            // 特征高度
	CSliceModel* pSliceModel;       // 切片模型
};

struct CSlice::CLineEx : public CLine
{
	CSliceModel* pSlice;
};
CSlice::CSlice(void)
{
}

CSlice::~CSlice(void)
{
	int sz = m_slices.size();
	for (int i = sz - 1; i >= 0; i--)
	{
		delete m_slices[i];
		m_slices[i] = NULL;
	}
	m_slices.clear();

	sz = m_tris.size();
	for (int i = sz - 1; i >= 0; i--)
	{
		delete m_tris[i];
		m_tris[i] = NULL;
	}
	m_tris.clear();

	m_Zs.clear();
}

void CSlice::LoadSTLModel(std::vector<CSTLModel*>& models)//载入stl模型
{
	int sz = models.size();
	for (int i = 0; i<sz; i++)
	{
		models[i]->Topologize();//给每个模型建立拓扑结构

		m_STLModels.push_back(models[i]);//再存入m_STLModels中
	}
	AfxMessageBox(_T("Load Finished!"), MB_OK, 0);
}

void CSlice::CalcTrisEigenHeight()//计算三角特征高度，用于自适应层切
{
	/*
	CString str;
	str.Format(_T("%f"),g_maxLayerHeight);
	AfxMessageBox(str);


	str.Format(_T("%f"),g_minLayerHeight);
	AfxMessageBox(str);*/
	int sz = m_tris.size();
	double a_max = atan(g_maxLayerHeight / g_stairError);//最大层高除以残余高度的反余弦
	double a_min = atan(g_minLayerHeight / g_stairError);
	CVector3D v(0, 0, 1);
	double a = 0;
	for (int i = 0; i<sz; i++)
	{
		a = Angle(v, *m_tris[i]->n);   //得到面片法矢和xy平法矢的夹角
		if (a > HALFPI)
		{
			a = PI - a;
		}
		if (a >= a_max)
		{
			m_tris[i]->eigenH = g_maxLayerHeight;
		}
		else if (a <= a_min)
		{
			m_tris[i]->eigenH = g_minLayerHeight;
		}
		else
		{
			m_tris[i]->eigenH = g_stairError*tan(a);
		}
	}

}
void CSlice::Slice()                          //开始切片
{
	AfxMessageBox(_T("Slice Start!"), MB_OK, 0);
	int sZ = m_STLModels.size();                //这里应该是模型的数量
	for (int i = 0; i<sZ; i++)
	{
		CSliceModel* pSlice = new CSliceModel();
		//pSlice->SetSliceType(SLC_PART);   
		pSlice->SetSTLModel(m_STLModels[i]);    //SetSTLModel是slicemodel的函数 所以定义一个其指针指向其函数
												//把每个模型的指针转过去
		m_slices.push_back(pSlice);             //将切片信息保存到m_slices数组中去
		int szTris = m_STLModels[i]->m_ltris.size();//然后对应得到每个模型的面片数量
		for (int j = 0; j<szTris; j++)
		{
			Triangle* pTri = new Triangle(m_STLModels[i]->m_ltris[j], pSlice);
			m_tris.push_back(pTri);//将模型的面片信息和文件名信息都保存到新的tris中
		}
	}
	sort(m_tris.begin(), m_tris.end(), Triangle::IsLower);//排序

														  //AfxMessageBox(_T("ca start!"),MB_OK,0);
	CalcTrisEigenHeight(); //计算特征高度
						   //AfxMessageBox(_T("ca end!"),MB_OK,0);
	int szModel = m_STLModels.size();
	double model_z_min = HUGE;
	for (int i = 0; i<szModel; i++)
	{
		double ext[6];
		m_STLModels[i]->FindExtreme(ext);//把每个模型都调过去找到极限尺寸
		if (ext[4] < model_z_min)//z_min
		{
			model_z_min = ext[4];//找到模型的最小z
		}
	}

	int szLayer = (model_z_min - g_baseLevel) / g_maxLayerHeight;//高度差除以最大层厚得到最小的层数
	szLayer++;

	double h = (model_z_min - g_baseLevel) / (double)szLayer;//得到层厚

	for (int i = 1; i <= szLayer; i++)
	{
		m_Zs.push_back(g_baseLevel + (double)i*h);//将不断增加的高度值存到m_Zs这个数组里
	}

	vector<Triangle*> status;
	int sz = m_tris.size(), szStatus, szSlice = m_slices.size();

	double z_min = m_tris[0]->v1->z + 1.0e-10;//因为已经过排序，故而第一个面片的v1的z值就是最小的，又加一点？？
	double z_max = m_tris[sz - 1]->v3->z;

	double z = z_min;
	int cnt = 0;
	//AfxMessageBox(_T("开始循环"),MB_OK,0);
	while (true)
	{
		szStatus = status.size();
		for (int i = szStatus - 1; i >= 0; i--)
		{
			if (status[i]->v3->z < z)//如果面片的最大z值都比切片值小
			{
				status.erase(status.begin() + i);//vector::erase()：从指定容器删除指定位置的元素或某段范围内的元素
			}
		}

		while (true)
		{
			if (cnt < sz && m_tris[cnt]->v1->z <= z) //如果一个面片最小点的z值小于等于切片值时
			{
				if (m_tris[cnt]->v3->z >= z)//该面片的最大点的z值大于等于最小理论值
				{
					status.push_back(m_tris[cnt]);//把这个面片写入status数组
				}
				cnt++;
			}
			else//已经遍历完成
			{
				break;
			}
		}

		szStatus = status.size();
		CLineEx line;                                      //定义一个结构变量line 包含CSliceModel* pSlice信息
//		std::vector<double> points;
		for (int i = 0; i<szStatus; i++)
		{
			if (CalcTriXYPlaneInst(status[i], z, line))       //如果相交的话  返回的是 z高度值 和这条交线
			{
//				points.push_back (line.m_pt1.x);
//				points.push_back(line.m_pt1.y); 
//				points.push_back(line.m_pt1.z);
//				points.push_back(line.m_pt2.x);
//				points.push_back(line.m_pt2.y);
//				points.push_back(line.m_pt2.z);
				line.pSlice->PushToSpecifiedLayer(z, line);  //存入到对应的层片里
			}
		}
/*		CString sFilePath = L"E:\\test.txt";
		CStdioFile fileout;
		
		
		if (fileout.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			CString str;
			int pointSize = points.size();
			for (int i = 0; i < pointSize; i += 6)
			{
				str.Format(_T("points  %.7f  %.7f  %.7f\n"),
				points[i], points[i + 1], points[i + 2]);
				fileout.WriteString(str);
				str.Format(_T("points  %.7f  %.7f  %.7f\n"),
				points[i + 3], points[i + 4], points[i + 5]);
				fileout.WriteString(str);
				fileout.WriteString(_T("\n"));
			}
		}
		fileout.Close();
*/
		m_Zs.push_back(z);//将此时的z值存入m_Zs

						  // next z calculation
		double dz = HUGE;
		for (int i = 0; i<szStatus; i++)
		{
			if (status[i]->eigenH < dz)
			{
				dz = status[i]->eigenH;
			}
		}

		z += dz;//得到新的z值
		if (z > z_max)//超出极限跳出
		{
			break;
		}
	}

	for (int i = 0; i<szSlice; i++)
	{

		m_slices[i]->GntLayerBoundaries();//得到层片轮廓

	}
	AfxMessageBox(_T("Slice Finished!"), MB_OK, 0);
}


bool CSlice::CalcTriXYPlaneInst(Triangle* pTri, double z, CLineEx& line)//判断与xy平面的关系？？
{
	if (::CalcTriXYPlaneInst(*pTri->v1, *pTri->v2, *pTri->v3, z, line))
	{
		line.pSlice = pTri->pSliceModel;
		return true;
	}
	else
	{
		return false;
	}
}