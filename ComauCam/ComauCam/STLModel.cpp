#include "stdafx.h"
#include "STLModel.h"


CSTLModel::CSTLModel(void)
{
}

CSTLModel::~CSTLModel(void)
{
	int sz = m_tris.GetSize();
	for (int i = 0; i < sz; i++)
	{
		delete m_tris[i];
	}
	m_tris.RemoveAll();
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
		m_tris.Add(pTri);
		//m_tris.push_back(pTri);

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

		m_tris.Add(pTriangle);
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
		int sz = m_tris.GetSize();
		//int sz=m_tris.size();
		CString str;
		for (int i = 0; i < sz; i++)
		{
			str.Format(_T("  facet normal %.7f %.7f %.7f\n"),
				m_tris[i]->Normal.dx, m_tris[i]->Normal.dy, m_tris[i]->Normal.dz);
			fileout.WriteString(str);
			fileout.WriteString(_T("    outer loop\n"));
			str.Format(_T("      vertex  %.7f %.7f %.7f\n"),
				m_tris[i]->A.x, m_tris[i]->A.y, m_tris[i]->A.z);
			fileout.WriteString(str);
			str.Format(_T("      vertex  %.7f %.7f %.7f\n"),
				m_tris[i]->B.x, m_tris[i]->B.y, m_tris[i]->B.z);
			fileout.WriteString(str);
			str.Format(_T("      vertex  %.7f %.7f %.7f\n"),
				m_tris[i]->C.x, m_tris[i]->C.y, m_tris[i]->C.z);
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
	int sz = m_tris.GetSize();
	for (int i = 0; i < sz; i++)
		delete m_tris[i];
	m_tris.RemoveAll();
}

void CSTLModel::FindExtreme(double ext[]) // �ҵ� m_vertices �� x��y��z ����ļ�ֵ
{

	double x_min = m_vertices[0]->x;
	double x_max = x_min;
	double y_min = m_vertices[0]->y;
	double y_max = y_min;
	double z_min = m_vertices[0]->z;
	double z_max = z_min;

	int sz = m_vertices.size();
	for (int i = 1; i<sz; i++)
	{
		if (m_vertices[i]->x > x_max)
		{
			x_max = m_vertices[i]->x;
		}
		if (m_vertices[i]->x < x_min)
		{
			x_min = m_vertices[i]->x;
		}
		if (m_vertices[i]->y > y_max)
		{
			y_max = m_vertices[i]->y;
		}
		if (m_vertices[i]->y < y_min)
		{
			y_min = m_vertices[i]->y;
		}
		if (m_vertices[i]->z > z_max)
		{
			z_max = m_vertices[i]->z;
		}
		if (m_vertices[i]->z < z_min)
		{
			z_min = m_vertices[i]->z;
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

void CSTLModel::Draw(COpenGLDC* pDC, bool ShowTri)
{
	//COLORREF oldClr;

	//pDC->GetMaterialColor(oldClr);
	//if (GetIsSelected())
	//	pDC->SetMaterialColor(CLR_SELECTED);
	//else
	//	pDC->SetMaterialColor(m_clr);

	int sz = m_tris.GetSize();
	if (ShowTri)
	{
		for (int i = 0; i<sz; i++)
		{
			m_tris[i]->DrawTriangleFace(pDC);  //��Ƭģʽ     
		}
	}
	else
	{
		for (int i = 0; i < sz; i++)
		{
			m_tris[i]->DrawTriangleFrame(pDC);  //�߿�ģʽ
		}
	}
	//pDC->SetMaterialColor(oldClr);
}

void CSTLModel::Topologize()	// �������˹�ϵ
{
	vector<LVertex> tmp_vertices;

	int szTri = m_tris.GetSize();    // �õ���Ƭ������
	for (int i = 0; i < szTri; i++)	 //��m_tris��洢����Ƭ�Ķ��㵼�뵽�½����������������
	{
		tmp_vertices.push_back(LVertex(m_tris[i]->A));
		tmp_vertices.push_back(LVertex(m_tris[i]->B));
		tmp_vertices.push_back(LVertex(m_tris[i]->C));
	}

	sort(tmp_vertices.begin(), tmp_vertices.end());	//sort�����Ը�����������Ԫ�ؽ�������

	int szV = tmp_vertices.size();    // szV == 3* szTri

	LVertex* pVertex = new LVertex(tmp_vertices[0]);
	m_vertices.push_back(pVertex); //��tmp_vertices��Ķ�����Ϣ�浽��߽ṹ�����m_vertices��ȥ

	for (int i = 1; i < szV; i++)
	{
		if (tmp_vertices[i] == tmp_vertices[i - 1])
		{
			continue;
		}
		else
		{
			LVertex* pVertex = new LVertex(tmp_vertices[i]); //����ͬ�ĵ�浽��߽ṹ�����m_vertices��ȥ
			m_vertices.push_back(pVertex);
		}

	}

	for (int i = 0; i<szTri; i++)
	{
		LTriangle* pFace = new LTriangle();
		CVector3D* pNormal = new CVector3D(m_tris[i]->Normal);
		m_normals.push_back(pNormal);     //��ԭ������m_tris���������Ϣ���ڰ�߽ṹ�����m_normals��ȥ

		pFace->v1 = SearchPtInVertices(m_tris[i]->A);//�������Ͷ�����Ӧ������
		pFace->v2 = SearchPtInVertices(m_tris[i]->B);
		pFace->v3 = SearchPtInVertices(m_tris[i]->C);
		pFace->n = pNormal;                           //������������Ҳ��Ӧ������

													  //����Ϣȫ�������˰�߽ṹ����������
		m_ltris.push_back(pFace);   // ��faces������������Ƭ��������ŷŵ�
	}

	for (int i = 0; i<szTri; i++)
	{
		LEdge *e1, *e2, *e3;
		e1 = new LEdge(m_ltris[i]->v1, m_ltris[i]->v2);    //1��2������ɵ�һ����
		e2 = new LEdge(m_ltris[i]->v2, m_ltris[i]->v3);
		e3 = new LEdge(m_ltris[i]->v3, m_ltris[i]->v1);
		e1->t = e2->t = e3->t = m_ltris[i];            //�����߶�����ͬһ������Ƭ

		e1->e_prev = e2->e_next = e3;     //��һ���ߵ�ǰһ���͵ڶ����ߵĺ�һ���߼�Ϊ������
		e2->e_prev = e3->e_next = e1;
		e3->e_prev = e1->e_next = e2;     //�������˱ߵ�˳���ϵ

		m_edges.push_back(e1);            //Ȼ����Щ�ߴ����˰�߽ṹ�ı�������
		m_edges.push_back(e2);
		m_edges.push_back(e3);


		m_ltris[i]->e1 = e1;    //����ͱ߽�����ϵ ��Ӧ��ϵ
		m_ltris[i]->e2 = e2;
		m_ltris[i]->e3 = e3;
	}

	int szE = m_edges.size();
	vector<LEdgeHull> edgeHulls;
	LEdgeHull edgeHull;            //����һ���ṹ����
	for (int i = 0; i<szE; i++)
	{
		edgeHull.edge = m_edges[i];//�Ƚ���߽ṹ�еıߴ浽�ṹ������
		edgeHulls.push_back(edgeHull);//�ٽ��ṹ�����е����ݴ浽��̬������
	}

	sort(edgeHulls.begin(), edgeHulls.end());//���򣿣���

	for (int i = 0; i<szE - 1; i++)
	{
		if (edgeHulls[i].IsOpposite(edgeHulls[i + 1]))//��������ߵķ������෴��
		{
			edgeHulls[i].edge->e_adja = edgeHulls[i + 1].edge;//˵�������߻��ɰ�߹�ϵ������
			edgeHulls[i + 1].edge->e_adja = edgeHulls[i].edge;//��Ϊ�����
			i++;
		}
	}
}

LVertex* CSTLModel::SearchPtInVertices(const CPoint3D& pt) //�ڰ�߽ṹ�Ķ���������Ѱ�������								  
{
	LVertex vertex(pt);
	int sz = m_vertices.size();
	int min_i = 0;              //��С���
	int max_i = sz - 1;           //�����

	if (*m_vertices[min_i] == vertex)
	{
		return m_vertices[min_i];
	}
	if (*m_vertices[max_i] == vertex)     //����պ���������С��ŵ��Ǹ���ֱ�ӷ���
	{
		return m_vertices[max_i];
	}

	int mid_i;
	while (true)
	{
		mid_i = (min_i + max_i) / 2;//�м���
		if (*m_vertices[mid_i] < vertex)   //����Ļ��ö��ַ�ֱ���ڶ����������ҵ������Ϊֹ 
		{
			min_i = mid_i;
		}
		else if (*m_vertices[mid_i] > vertex)
		{
			max_i = mid_i;
		}
		else     //���պ���ȵ����
		{
			return m_vertices[mid_i];
		}
	}
}
