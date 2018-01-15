#include "stdafx.h"
#include "STLModel.h"


CSTLModel::CSTLModel(void)
{
}

CSTLModel::~CSTLModel(void)
{
	unsigned int sz = m_vecpTris.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		delete m_vecpTris[i];
		m_vecpTris[i] = NULL;
	}
	m_vecpTris.clear();
}


bool CSTLModel::ReadSTL(CString sFilePath)
{
	CFile file;
	if (file.Open(sFilePath, CFile::modeRead))
	{
		file.Seek(80, CFile::begin);  //����ǰ80�ֽ�
		unsigned int num = 0;
		file.Read(&num, 4);//�õ���Ƭ����
		int fileSize = 80 + 4 + 50 * num;//һ����Ƭռ50�ֽ�
		bool bBinary = false;

		if (fileSize == file.GetLength())//�ļ��ֽںͼ����ֽ�һ��
		{
			ReadBinarySTL(file);//˵���Ƕ����Ƹ�ʽ
			file.Close();
		}
		else
		{
			file.Close();
			CStdioFile stdioFile;
			if (stdioFile.Open(sFilePath, CFile::modeRead))
			{
				ReadAsciiSTL(stdioFile);
				stdioFile.Close();
			}
		}
	}
	return TRUE;
}


bool CSTLModel::ReadBinarySTL(CFile& file)
{
	unsigned int n = 0;
	float f;
	byte b;
	CVector3D v;
	CPoint3D A, B, C;
	file.Seek(80, CFile::begin);
	file.Read(&n, 4);//�õ�nΪ��Ƭ����
	for (unsigned int i = 0; i<n; i++)
	{
		file.Read(&f, 4); v.dx = f;//ÿ����ֵռ4�ֽ�
		file.Read(&f, 4); v.dy = f;
		file.Read(&f, 4); v.dz = f;

		file.Read(&f, 4); A.x = f;
		file.Read(&f, 4); A.y = f;
		file.Read(&f, 4); A.z = f;

		file.Read(&f, 4); B.x = f;
		file.Read(&f, 4); B.y = f;
		file.Read(&f, 4); B.z = f;

		file.Read(&f, 4); C.x = f;
		file.Read(&f, 4); C.y = f;
		file.Read(&f, 4); C.z = f;

		v.Normalize();//����������λ��
		CTriangle* pTri = new CTriangle(v, A, B, C);//����һ���µ�����

		RoundFour(pTri);
		m_vecpTris.push_back(pTri);

		file.Read(&b, 1);//������ֽ���Ԥ���ֽ�û����Ϣ
		file.Read(&b, 1);
	}

return true;
}

bool CSTLModel::ReadAsciiSTL(CStdioFile& file)
{
	CTriangle* pTriangle = NULL;
	CString str;
	CString strFlag;
	file.ReadString(str);
	while (file.ReadString(str))
	{
		strFlag = str.SpanExcluding(_T(" "));   //���ص�һ�γ��ֿո�ǰ�������ַ�����endsolid
		if (_T("endsolid") == strFlag || _T("ENDSOLID") == strFlag)//�ļ�����
		{
			break;
		}
		pTriangle = new CTriangle;
		pTriangle->Normal = FindNormal(str);
		pTriangle->Normal.Normalize();
		file.ReadString(str);
		file.ReadString(str);
		pTriangle->A = FindCord(str);
		file.ReadString(str);
		pTriangle->B = FindCord(str);
		file.ReadString(str);
		pTriangle->C = FindCord(str);

		file.ReadString(str);
		file.ReadString(str);

		RoundFour(pTriangle);
		m_vecpTris.push_back(pTriangle);
	}

	return true;
}

void CSTLModel::WriteSTL(CString sFilePath)
{
	CStdioFile fileout;
	//CStdioFile file (sFilePath,CFile::modeCreate | CFile::modeWrite);
	if (fileout.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		fileout.WriteString(_T("solid OBJECT\n"));
		int sz = m_vecpTris.size();
		//int sz=m_vecpTris.size();
		CString str;
		for (int i = 0; i < sz; i++)
		{
			str.Format(_T("  facet normal %.7f %.7f %.7f\n"),
				m_vecpTris[i]->Normal.dx, m_vecpTris[i]->Normal.dy, m_vecpTris[i]->Normal.dz);
			fileout.WriteString(str);
			fileout.WriteString(_T("    outer loop\n"));
			str.Format(_T("      vertex  %.7f %.7f %.7f\n"),
				m_vecpTris[i]->A.x, m_vecpTris[i]->A.y, m_vecpTris[i]->A.z);
			fileout.WriteString(str);
			str.Format(_T("      vertex  %.7f %.7f %.7f\n"),
				m_vecpTris[i]->B.x, m_vecpTris[i]->B.y, m_vecpTris[i]->B.z);
			fileout.WriteString(str);
			str.Format(_T("      vertex  %.7f %.7f %.7f\n"),
				m_vecpTris[i]->C.x, m_vecpTris[i]->C.y, m_vecpTris[i]->C.z);
			fileout.WriteString(str);
			fileout.WriteString(_T("    endloop\n"));
			fileout.WriteString(_T("  endfacet\n"));
		}
		fileout.WriteString(_T("endsolid OBJECT\n"));
		fileout.Close();
	}
}

void CSTLModel::ReleaseMem()
{
	int sz = m_vecpTris.size();
	for (int i = 0; i < sz; i++)
		delete m_vecpTris[i];
	m_vecpTris.clear();
}

void CSTLModel::FindExtreme(double ext[]) // �ҵ� m_vecpVertices �� x��y��z ����ļ�ֵ
{

	double x_min = m_vecpVertices[0]->x;
	double x_max = x_min;
	double y_min = m_vecpVertices[0]->y;
	double y_max = y_min;
	double z_min = m_vecpVertices[0]->z;
	double z_max = z_min;

	unsigned int sz = m_vecpVertices.size();
	for (unsigned int i = 1; i<sz; i++)
	{
		if (m_vecpVertices[i]->x > x_max)
		{
			x_max = m_vecpVertices[i]->x;
		}
		if (m_vecpVertices[i]->x < x_min)
		{
			x_min = m_vecpVertices[i]->x;
		}
		if (m_vecpVertices[i]->y > y_max)
		{
			y_max = m_vecpVertices[i]->y;
		}
		if (m_vecpVertices[i]->y < y_min)
		{
			y_min = m_vecpVertices[i]->y;
		}
		if (m_vecpVertices[i]->z > z_max)
		{
			z_max = m_vecpVertices[i]->z;
		}
		if (m_vecpVertices[i]->z < z_min)
		{
			z_min = m_vecpVertices[i]->z;
		}
	}

	ext[0] = x_min;
	ext[1] = x_max;
	ext[2] = y_min;
	ext[3] = y_max;
	ext[4] = z_min;
	ext[5] = z_max;
}

CPoint3D CSTLModel::FindCord(CString str)
{
	str += "  END";

	double cof = 1.0;  // this is a cofficient to amplify the original shape

	double x, y, z;
	int s, e;
	s = 0;
	while (!IsNumber(str[s]))
	{
		s++;
	}
	e = s;
	while (!IsSpace(str[e]))
	{
		e++;
	}
#ifdef UNICODE
	x = _wtof(str.Mid(s, e - s))*cof;
#else
	x = atof(str.Mid(s, e - s));
#endif


	s = e;
	while (!IsNumber(str[s]))
	{
		s++;
	}
	e = s;
	while (!IsSpace(str[e]))
	{
		e++;
	}
	y = _wtof(str.Mid(s, e - s))*cof;

	s = e;
	while (!IsNumber(str[s]))
	{
		s++;
	}
	e = s;
	while (!IsSpace(str[e]))
	{
		e++;
	}
	z = _wtof(str.Mid(s, e - s))*cof;

	return CPoint3D(x, y, z);
}


CVector3D CSTLModel::FindNormal(CString str)
{
	str += "  END";
	double x, y, z;
	int s, e;
	s = 0;
	while (!IsNumber(str[s]))
	{
		s++;
	}
	e = s;
	while (!IsSpace(str[e]))
	{
		e++;
	}
	x = _wtof(str.Mid(s, e - s));

	s = e;
	while (!IsNumber(str[s]))
	{
		s++;
	}
	e = s;
	while (!IsSpace(str[e]))
	{
		e++;
	}
	y = _wtof(str.Mid(s, e - s));

	s = e;
	while (!IsNumber(str[s]))
	{
		s++;
	}
	e = s;
	while (!IsSpace(str[e]))
	{
		e++;
	}
	z = _wtof(str.Mid(s, e - s));

	return CVector3D(x, y, z);
}


BOOL CSTLModel::IsNumber(TCHAR ch)
{
	int x = (int)ch;
	if ((x >= '0' && x <= '9') || x == '.' || x == '-')
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CSTLModel::IsSpace(TCHAR ch)
{
	if (' ' == ch)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CSTLModel::MoveModel(CVector3D vec)
{
	unsigned int sz = m_vecpTris.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		m_vecpTris[i]->Move(vec);
	}
}

void CSTLModel::RotateModel(double angle, CVector3D vec)
{
	unsigned int sz = m_vecpTris.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		m_vecpTris[i]->Rotate(angle, vec);
	}
}

CVector3D CSTLModel::OnCenter()
{
	CVector3D vec;
	double z = 5;
	vector<CTriangle*> status;
	int szTri = m_vecpTris.size();
	for (int i = 0; i < szTri; i++)         //����������Ƭ,ɸѡ���ཻ��Ƭ
	{
		double z_min = ReturnZmin(m_vecpTris[i]);
		double z_max = ReturnZmax(m_vecpTris[i]);
		if (z_min <= z&&z_max >= z)
		{
			if (z_min == z_max)
			{
				//������Ƭ����Ƭ�غϵ���Ƭ
			}
			else {
				status.push_back(new CTriangle(m_vecpTris[i]->A, m_vecpTris[i]->B, m_vecpTris[i]->C));
			}       //�洢����Ƭ���ཻ��Ƭ
		}
	}
	double x_min = status[0]->A.x;
	double x_max = status[0]->A.x;
	double y_min = status[0]->A.y;
	double y_max = status[0]->A.y;
	for (unsigned int i = 0; i < status.size(); i++)
	{
		if (x_min >= status[i]->A.x)
			x_min = status[i]->A.x;
		if (x_min >= status[i]->B.x)
			x_min = status[i]->B.x;
		if (x_min >= status[i]->C.x)
			x_min = status[i]->C.x;
		if (x_max <= status[i]->A.x)
			x_max = status[i]->A.x;
		if (x_max <= status[i]->B.x)
			x_max = status[i]->B.x;
		if (x_max <= status[i]->C.x)
			x_max = status[i]->C.x;

		if (y_min >= status[i]->A.y)
			y_min = status[i]->A.y;
		if (y_min >= status[i]->B.y)
			y_min = status[i]->B.y;
		if (y_min >= status[i]->C.y)
			y_min = status[i]->C.y;
		if (y_max <= status[i]->A.y)
			y_max = status[i]->A.y;
		if (y_max <= status[i]->B.y)
			y_max = status[i]->B.y;
		if (y_max <= status[i]->C.y)
			y_max = status[i]->C.y;
	}
	vec = CVector3D(0.0-(x_max + x_min) / 2.0, 0.0-(y_max + y_min) / 2.0, 0);
	return vec;
}

double CSTLModel::ReturnZmin(const CTriangle* Ltri)
{
	double z1, z2, z3;
	z1 = Ltri->A.z;
	z2 = Ltri->B.z;
	z3 = Ltri->C.z;
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
double CSTLModel::ReturnZmax(const CTriangle* Ltri)
{
	double z1, z2, z3;
	z1 = Ltri->A.z;
	z2 = Ltri->B.z;
	z3 = Ltri->C.z;
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

void CSTLModel::Topologize()	// �������˹�ϵ
{
	vector<CLVertex> tmp_vertices;

	int szTri = m_vecpTris.size();    // �õ���Ƭ������
	for (int i = 0; i < szTri; i++)	 //��m_vecpTris��洢����Ƭ�Ķ��㵼�뵽�½����������������
	{
		tmp_vertices.push_back(CLVertex(m_vecpTris[i]->A));
		tmp_vertices.push_back(CLVertex(m_vecpTris[i]->B));
		tmp_vertices.push_back(CLVertex(m_vecpTris[i]->C));
	}

	sort(tmp_vertices.begin(), tmp_vertices.end());	//sort�����Ը�����������Ԫ�ؽ�������

	unsigned int szV = tmp_vertices.size();    // szV == 3* szTri

	CLVertex* pVertex = new CLVertex(tmp_vertices[0]);
	m_vecpVertices.push_back(pVertex); //��tmp_vertices��Ķ�����Ϣ�浽��߽ṹ�����m_vecpVertices��ȥ

	for (unsigned int i = 1; i < szV; i++)
	{
		if (tmp_vertices[i] == tmp_vertices[i - 1])
		{
			continue;
		}
		else
		{
			CLVertex* pVertex = new CLVertex(tmp_vertices[i]); //����ͬ�ĵ�浽��߽ṹ�����m_vecpVertices��ȥ
			m_vecpVertices.push_back(pVertex);
		}

	}

	for (int i = 0; i<szTri; i++)
	{
		CLTriangle* pFace = new CLTriangle();
		CVector3D* pNormal = new CVector3D(m_vecpTris[i]->Normal);
		m_vecpNormals.push_back(pNormal);     //��ԭ������m_vecpTris���������Ϣ���ڰ�߽ṹ�����m_normals��ȥ

		pFace->v1 = SearchPtInVertices(m_vecpTris[i]->A);//�������Ͷ�����Ӧ������
		pFace->v2 = SearchPtInVertices(m_vecpTris[i]->B);
		pFace->v3 = SearchPtInVertices(m_vecpTris[i]->C);
		pFace->n = pNormal;                           //������������Ҳ��Ӧ������
													  //����Ϣȫ�������˰�߽ṹ����������
		m_vecpLTris.push_back(pFace);   // ��faces������������Ƭ��������ŷŵ�
	}

	for (int i = 0; i<szTri; i++)
	{
		CLEdge *e1, *e2, *e3;
		e1 = new CLEdge(m_vecpLTris[i]->v1, m_vecpLTris[i]->v2);    //1��2������ɵ�һ����
		e2 = new CLEdge(m_vecpLTris[i]->v2, m_vecpLTris[i]->v3);
		e3 = new CLEdge(m_vecpLTris[i]->v3, m_vecpLTris[i]->v1);
		e1->t = e2->t = e3->t = m_vecpLTris[i];            //�����߶�����ͬһ������Ƭ

		e1->e_prev = e2->e_next = e3;     //��һ���ߵ�ǰһ���͵ڶ����ߵĺ�һ���߼�Ϊ������
		e2->e_prev = e3->e_next = e1;
		e3->e_prev = e1->e_next = e2;     //�������˱ߵ�˳���ϵ

		m_vecpEdges.push_back(e1);            //Ȼ����Щ�ߴ����˰�߽ṹ�ı�������
		m_vecpEdges.push_back(e2);
		m_vecpEdges.push_back(e3);


		m_vecpLTris[i]->e1 = e1;    //����ͱ߽�����ϵ ��Ӧ��ϵ
		m_vecpLTris[i]->e2 = e2;
		m_vecpLTris[i]->e3 = e3;
	}

	unsigned int szE = m_vecpEdges.size();
	vector<CLEdgeHull> edgeHulls;
	CLEdgeHull edgeHull;            //����һ���ṹ����
	for (unsigned int i = 0; i<szE; i++)
	{
		edgeHull.m_pEdge = m_vecpEdges[i];//�Ƚ���߽ṹ�еıߴ浽�ṹ������
		edgeHulls.push_back(edgeHull);//�ٽ��ṹ�����е����ݴ浽��̬������
	}

	sort(edgeHulls.begin(), edgeHulls.end());//���򣿣���

	for (unsigned int i = 0; i<szE - 1; i++)
	{
		if (edgeHulls[i].IsOpposite(edgeHulls[i + 1]))//��������ߵķ������෴��
		{
			edgeHulls[i].m_pEdge->e_adja = edgeHulls[i + 1].m_pEdge;//˵�������߻��ɰ�߹�ϵ������
			edgeHulls[i + 1].m_pEdge->e_adja = edgeHulls[i].m_pEdge;//��Ϊ�����
			i++;
		}
	}

	for (unsigned int i = 0; i < m_vecpEdges.size(); i++)
	{
		if(m_vecpEdges[i]->e_adja == NULL)
			AfxMessageBox(_T("Topologize failed!!"), MB_OK, 0);
	}
}

CLVertex* CSTLModel::SearchPtInVertices(const CPoint3D& pt) //�ڰ�߽ṹ�Ķ���������Ѱ�������								  
{
	CLVertex vertex(pt);
	unsigned int sz = m_vecpVertices.size();
	int min_i = 0;              //��С���
	int max_i = sz - 1;           //�����

	if (*m_vecpVertices[min_i] == vertex)
	{
		return m_vecpVertices[min_i];
	}
	if (*m_vecpVertices[max_i] == vertex)     //����պ���������С��ŵ��Ǹ���ֱ�ӷ���
	{
		return m_vecpVertices[max_i];
	}

	int mid_i;
	while (true)
	{
		mid_i = (min_i + max_i) / 2;//�м���
		if (*m_vecpVertices[mid_i] < vertex)   //����Ļ��ö��ַ�ֱ���ڶ����������ҵ������Ϊֹ 
		{
			min_i = mid_i;
		}
		else if (*m_vecpVertices[mid_i] > vertex)
		{
			max_i = mid_i;
		}
		else     //���պ���ȵ����
		{
			return m_vecpVertices[mid_i];
		}
	}
}

void CSTLModel::RoundFour(CTriangle* pTri)
{
	int number = 4;
	pTri->A.x = Round(pTri->A.x, number);
	pTri->A.y = Round(pTri->A.y, number);
	pTri->A.z = Round(pTri->A.z, number);

	pTri->B.x = Round(pTri->B.x, number);
	pTri->B.y = Round(pTri->B.y, number);
	pTri->B.z = Round(pTri->B.z, number);

	pTri->C.x = Round(pTri->C.x, number);
	pTri->C.y = Round(pTri->C.y, number);
	pTri->C.z = Round(pTri->C.z, number);

	pTri->Normal.dx = Round(pTri->Normal.dx, number);
	pTri->Normal.dy = Round(pTri->Normal.dy, number);
	pTri->Normal.dz = Round(pTri->Normal.dz, number);

}