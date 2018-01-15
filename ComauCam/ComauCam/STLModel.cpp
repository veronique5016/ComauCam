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
		file.Seek(80, CFile::begin);  //跳过前80字节
		unsigned int num = 0;
		file.Read(&num, 4);//得到面片数量
		int fileSize = 80 + 4 + 50 * num;//一个面片占50字节
		bool bBinary = false;

		if (fileSize == file.GetLength())//文件字节和计算字节一致
		{
			ReadBinarySTL(file);//说明是二进制格式
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
	file.Read(&n, 4);//得到n为面片数量
	for (unsigned int i = 0; i<n; i++)
	{
		file.Read(&f, 4); v.dx = f;//每个数值占4字节
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

		v.Normalize();//即将向量单位化
		CTriangle* pTri = new CTriangle(v, A, B, C);//分配一个新的三角
		m_tris.Add(pTri);
		//m_tris.push_back(pTri);

		file.Read(&b, 1);//最后两字节是预留字节没有信息
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
		strFlag = str.SpanExcluding(_T(" "));   //返回第一次出现空格前的所有字符，即endsolid
		if (_T("endsolid") == strFlag || _T("ENDSOLID") == strFlag)//文件结束
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

void CSTLModel::FindExtreme(double ext[]) // 找到 m_vertices 中 x、y、z 坐标的极值
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
			m_tris[i]->DrawTriangleFace(pDC);  //面片模式     
		}
	}
	else
	{
		for (int i = 0; i < sz; i++)
		{
			m_tris[i]->DrawTriangleFrame(pDC);  //线框模式
		}
	}
	//pDC->SetMaterialColor(oldClr);
}

void CSTLModel::Topologize()	// 建立拓扑关系
{
	vector<LVertex> tmp_vertices;

	int szTri = m_tris.GetSize();    // 得到面片的数量
	for (int i = 0; i < szTri; i++)	 //将m_tris里存储的面片的顶点导入到新建的这个拓扑数组中
	{
		tmp_vertices.push_back(LVertex(m_tris[i]->A));
		tmp_vertices.push_back(LVertex(m_tris[i]->B));
		tmp_vertices.push_back(LVertex(m_tris[i]->C));
	}

	sort(tmp_vertices.begin(), tmp_vertices.end());	//sort函数对给定区间所有元素进行排序

	int szV = tmp_vertices.size();    // szV == 3* szTri

	LVertex* pVertex = new LVertex(tmp_vertices[0]);
	m_vertices.push_back(pVertex); //将tmp_vertices里的顶点信息存到半边结构定义的m_vertices里去

	for (int i = 1; i < szV; i++)
	{
		if (tmp_vertices[i] == tmp_vertices[i - 1])
		{
			continue;
		}
		else
		{
			LVertex* pVertex = new LVertex(tmp_vertices[i]); //将不同的点存到半边结构定义的m_vertices里去
			m_vertices.push_back(pVertex);
		}

	}

	for (int i = 0; i<szTri; i++)
	{
		LTriangle* pFace = new LTriangle();
		CVector3D* pNormal = new CVector3D(m_tris[i]->Normal);
		m_normals.push_back(pNormal);     //将原来存在m_tris里的向量信息存在半边结构定义的m_normals里去

		pFace->v1 = SearchPtInVertices(m_tris[i]->A);//即将面表和顶点表对应了起来
		pFace->v2 = SearchPtInVertices(m_tris[i]->B);
		pFace->v3 = SearchPtInVertices(m_tris[i]->C);
		pFace->n = pNormal;                           //将面表和向量表也对应了起来

													  //将信息全部存在了半边结构的面数组里
		m_ltris.push_back(pFace);   // 在faces序列中三角面片还是随机排放的
	}

	for (int i = 0; i<szTri; i++)
	{
		LEdge *e1, *e2, *e3;
		e1 = new LEdge(m_ltris[i]->v1, m_ltris[i]->v2);    //1、2两点组成第一条边
		e2 = new LEdge(m_ltris[i]->v2, m_ltris[i]->v3);
		e3 = new LEdge(m_ltris[i]->v3, m_ltris[i]->v1);
		e1->t = e2->t = e3->t = m_ltris[i];            //三条边都属于同一个三角片

		e1->e_prev = e2->e_next = e3;     //第一条边的前一条和第二条边的后一条边即为第三边
		e2->e_prev = e3->e_next = e1;
		e3->e_prev = e1->e_next = e2;     //即建立了边的顺序关系

		m_edges.push_back(e1);            //然后将这些边存入了半边结构的边数组中
		m_edges.push_back(e2);
		m_edges.push_back(e3);


		m_ltris[i]->e1 = e1;    //将面和边建立联系 对应关系
		m_ltris[i]->e2 = e2;
		m_ltris[i]->e3 = e3;
	}

	int szE = m_edges.size();
	vector<LEdgeHull> edgeHulls;
	LEdgeHull edgeHull;            //定义一个结构变量
	for (int i = 0; i<szE; i++)
	{
		edgeHull.edge = m_edges[i];//先将半边结构中的边存到结构变量中
		edgeHulls.push_back(edgeHull);//再将结构变量中的数据存到动态数组中
	}

	sort(edgeHulls.begin(), edgeHulls.end());//排序？？？

	for (int i = 0; i<szE - 1; i++)
	{
		if (edgeHulls[i].IsOpposite(edgeHulls[i + 1]))//如果两条边的方向是相反的
		{
			edgeHulls[i].edge->e_adja = edgeHulls[i + 1].edge;//说明两条边互成半边关系！！！
			edgeHulls[i + 1].edge->e_adja = edgeHulls[i].edge;//互为伙伴半边
			i++;
		}
	}
}

LVertex* CSTLModel::SearchPtInVertices(const CPoint3D& pt) //在半边结构的顶点数组中寻找这个点								  
{
	LVertex vertex(pt);
	int sz = m_vertices.size();
	int min_i = 0;              //最小编号
	int max_i = sz - 1;           //最大编号

	if (*m_vertices[min_i] == vertex)
	{
		return m_vertices[min_i];
	}
	if (*m_vertices[max_i] == vertex)     //如果刚好是最大或最小编号的那个则直接返回
	{
		return m_vertices[max_i];
	}

	int mid_i;
	while (true)
	{
		mid_i = (min_i + max_i) / 2;//中间编号
		if (*m_vertices[mid_i] < vertex)   //否则的话用二分法直到在顶点数组中找到这个点为止 
		{
			min_i = mid_i;
		}
		else if (*m_vertices[mid_i] > vertex)
		{
			max_i = mid_i;
		}
		else     //即刚好相等的情况
		{
			return m_vertices[mid_i];
		}
	}
}
