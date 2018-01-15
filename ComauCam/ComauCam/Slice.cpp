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

	void AdjustVertices()//������ߵ�
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

	LVertex* GetVertex(int idx) const//���ݲ������ض���
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
	LVertex      *v1, *v2, *v3;     // ָ�����������ָ��,v1 ��ͣ� v3 ���
	CVector3D*   n;                 // ���ڼ��������߶�
	double       eigenH;            // �����߶�
	CSliceModel* pSliceModel;       // ��Ƭģ��
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

void CSlice::LoadSTLModel(std::vector<CSTLModel*>& models)//����stlģ��
{
	int sz = models.size();
	for (int i = 0; i<sz; i++)
	{
		models[i]->Topologize();//��ÿ��ģ�ͽ������˽ṹ

		m_STLModels.push_back(models[i]);//�ٴ���m_STLModels��
	}
	AfxMessageBox(_T("Load Finished!"), MB_OK, 0);
}

void CSlice::CalcTrisEigenHeight()//�������������߶ȣ���������Ӧ����
{
	/*
	CString str;
	str.Format(_T("%f"),g_maxLayerHeight);
	AfxMessageBox(str);


	str.Format(_T("%f"),g_minLayerHeight);
	AfxMessageBox(str);*/
	int sz = m_tris.size();
	double a_max = atan(g_maxLayerHeight / g_stairError);//����߳��Բ���߶ȵķ�����
	double a_min = atan(g_minLayerHeight / g_stairError);
	CVector3D v(0, 0, 1);
	double a = 0;
	for (int i = 0; i<sz; i++)
	{
		a = Angle(v, *m_tris[i]->n);   //�õ���Ƭ��ʸ��xyƽ��ʸ�ļн�
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
void CSlice::Slice()                          //��ʼ��Ƭ
{
	AfxMessageBox(_T("Slice Start!"), MB_OK, 0);
	int sZ = m_STLModels.size();                //����Ӧ����ģ�͵�����
	for (int i = 0; i<sZ; i++)
	{
		CSliceModel* pSlice = new CSliceModel();
		//pSlice->SetSliceType(SLC_PART);   
		pSlice->SetSTLModel(m_STLModels[i]);    //SetSTLModel��slicemodel�ĺ��� ���Զ���һ����ָ��ָ���亯��
												//��ÿ��ģ�͵�ָ��ת��ȥ
		m_slices.push_back(pSlice);             //����Ƭ��Ϣ���浽m_slices������ȥ
		int szTris = m_STLModels[i]->m_ltris.size();//Ȼ���Ӧ�õ�ÿ��ģ�͵���Ƭ����
		for (int j = 0; j<szTris; j++)
		{
			Triangle* pTri = new Triangle(m_STLModels[i]->m_ltris[j], pSlice);
			m_tris.push_back(pTri);//��ģ�͵���Ƭ��Ϣ���ļ�����Ϣ�����浽�µ�tris��
		}
	}
	sort(m_tris.begin(), m_tris.end(), Triangle::IsLower);//����

														  //AfxMessageBox(_T("ca start!"),MB_OK,0);
	CalcTrisEigenHeight(); //���������߶�
						   //AfxMessageBox(_T("ca end!"),MB_OK,0);
	int szModel = m_STLModels.size();
	double model_z_min = HUGE;
	for (int i = 0; i<szModel; i++)
	{
		double ext[6];
		m_STLModels[i]->FindExtreme(ext);//��ÿ��ģ�Ͷ�����ȥ�ҵ����޳ߴ�
		if (ext[4] < model_z_min)//z_min
		{
			model_z_min = ext[4];//�ҵ�ģ�͵���Сz
		}
	}

	int szLayer = (model_z_min - g_baseLevel) / g_maxLayerHeight;//�߶Ȳ���������õ���С�Ĳ���
	szLayer++;

	double h = (model_z_min - g_baseLevel) / (double)szLayer;//�õ����

	for (int i = 1; i <= szLayer; i++)
	{
		m_Zs.push_back(g_baseLevel + (double)i*h);//���������ӵĸ߶�ֵ�浽m_Zs���������
	}

	vector<Triangle*> status;
	int sz = m_tris.size(), szStatus, szSlice = m_slices.size();

	double z_min = m_tris[0]->v1->z + 1.0e-10;//��Ϊ�Ѿ������򣬹ʶ���һ����Ƭ��v1��zֵ������С�ģ��ּ�һ�㣿��
	double z_max = m_tris[sz - 1]->v3->z;

	double z = z_min;
	int cnt = 0;
	//AfxMessageBox(_T("��ʼѭ��"),MB_OK,0);
	while (true)
	{
		szStatus = status.size();
		for (int i = szStatus - 1; i >= 0; i--)
		{
			if (status[i]->v3->z < z)//�����Ƭ�����zֵ������ƬֵС
			{
				status.erase(status.begin() + i);//vector::erase()����ָ������ɾ��ָ��λ�õ�Ԫ�ػ�ĳ�η�Χ�ڵ�Ԫ��
			}
		}

		while (true)
		{
			if (cnt < sz && m_tris[cnt]->v1->z <= z) //���һ����Ƭ��С���zֵС�ڵ�����Ƭֵʱ
			{
				if (m_tris[cnt]->v3->z >= z)//����Ƭ�������zֵ���ڵ�����С����ֵ
				{
					status.push_back(m_tris[cnt]);//�������Ƭд��status����
				}
				cnt++;
			}
			else//�Ѿ��������
			{
				break;
			}
		}

		szStatus = status.size();
		CLineEx line;                                      //����һ���ṹ����line ����CSliceModel* pSlice��Ϣ
//		std::vector<double> points;
		for (int i = 0; i<szStatus; i++)
		{
			if (CalcTriXYPlaneInst(status[i], z, line))       //����ཻ�Ļ�  ���ص��� z�߶�ֵ ����������
			{
//				points.push_back (line.m_pt1.x);
//				points.push_back(line.m_pt1.y); 
//				points.push_back(line.m_pt1.z);
//				points.push_back(line.m_pt2.x);
//				points.push_back(line.m_pt2.y);
//				points.push_back(line.m_pt2.z);
				line.pSlice->PushToSpecifiedLayer(z, line);  //���뵽��Ӧ�Ĳ�Ƭ��
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
		m_Zs.push_back(z);//����ʱ��zֵ����m_Zs

						  // next z calculation
		double dz = HUGE;
		for (int i = 0; i<szStatus; i++)
		{
			if (status[i]->eigenH < dz)
			{
				dz = status[i]->eigenH;
			}
		}

		z += dz;//�õ��µ�zֵ
		if (z > z_max)//������������
		{
			break;
		}
	}

	for (int i = 0; i<szSlice; i++)
	{

		m_slices[i]->GntLayerBoundaries();//�õ���Ƭ����

	}
	AfxMessageBox(_T("Slice Finished!"), MB_OK, 0);
}


bool CSlice::CalcTriXYPlaneInst(Triangle* pTri, double z, CLineEx& line)//�ж���xyƽ��Ĺ�ϵ����
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