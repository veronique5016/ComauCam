#include "stdafx.h"
#include "STLModel.h"


CSTLModel::CSTLModel(void)
{
}

CSTLModel::~CSTLModel(void)
{
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
		for (int i = 0; i<sz; i++)
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
	AfxMessageBox(_T("finish"));
}

void CSTLModel::ReleaseMem()
{
	int sz = m_tris.GetSize();
	for (int i = 0; i<sz; i++)
		delete m_tris[i];
	m_tris.RemoveAll();
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

void CSTLModel::Draw(COpenGLDC* pDC)
{
	COLORREF oldClr;

	//pDC->GetMaterialColor(oldClr);
	//if (GetIsSelected())
	//	pDC->SetMaterialColor(CLR_SELECTED);
	//else
	//	pDC->SetMaterialColor(m_clr);

	int sz = m_tris.GetSize();

	for (int i = 0; i<sz; i++)
	{
		m_tris[i]->DrawTriangleFace(pDC);            //��Ƭģʽ
		//m_tris[i]->DrawTriangleFrame(pDC);       //�߿�ģʽ
	}

	//pDC->SetMaterialColor(oldClr);
}