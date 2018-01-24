#include "stdafx.h"
#include "GCode.h"

CGCode::CGCode()
{
}

CGCode::~CGCode()
{
	int szL = m_vecpGCodeLayers.size();
	for (int i = 0; i < szL; i++)
	{
		delete m_vecpGCodeLayers[i];
		m_vecpGCodeLayers[i] = NULL;
	}
	m_vecpGCodeLayers.clear();
}

void CGCode::Write(CString sFilePath)
{
	CStdioFile fileout;
	if (fileout.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		fileout.WriteString(_T("G28 ;\n"));
		fileout.WriteString(_T("G1 Z300 F2000 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("M82 ;\n"));
		fileout.WriteString(_T("M104 T0 S210 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M109 T0 S210 ;\n"));
		fileout.WriteString(_T("G1 X-2.12 Y109.40 Z138.5 ;\n"));
		fileout.WriteString(_T("G92 X0 Y0 Z0 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		CString str;
		double length = 0.0000;
		double volumn = 0.0000;
		int szLayer = m_vecpGCodeLayers.size();
		for (int i = 0; i < szLayer; i++)
		{
			str.Format(_T("G0 X%.3f Y%.3f Z%.3f i%.3f j%.3f k%.3f\n"),
				m_vecpGCodeLayers[i]->m_vpRoute[0]->x,
				m_vecpGCodeLayers[i]->m_vpRoute[0]->y,
				m_vecpGCodeLayers[i]->m_vpRoute[0]->z,
				m_vecpGCodeLayers[i]->m_vCoordinate[2].dx,
				m_vecpGCodeLayers[i]->m_vCoordinate[2].dy,
				m_vecpGCodeLayers[i]->m_vCoordinate[2].dz);
			fileout.WriteString(str);
			int szB = m_vecpGCodeLayers[i]->m_vpBoundaries[0]->m_vpSegments.size();
			for (int j = 1; j < szB + 1; j++)
			{
				length = GetDistance(*m_vecpGCodeLayers[i]->m_vpRoute[j - 1], *m_vecpGCodeLayers[i]->m_vpRoute[j]);
				volumn += length * 0.2;
				str.Format(_T("G1 X%.3f Y%.3f Z%.3f i%.3f j%.3f k%.3f E-%.5f\n"),
					m_vecpGCodeLayers[i]->m_vpRoute[j]->x, m_vecpGCodeLayers[i]->m_vpRoute[j]->y, m_vecpGCodeLayers[i]->m_vpRoute[j]->z, (volumn / 6.0),
					m_vecpGCodeLayers[i]->m_vCoordinate[2].dx,
					m_vecpGCodeLayers[i]->m_vCoordinate[2].dy,
					m_vecpGCodeLayers[i]->m_vCoordinate[2].dz);
				fileout.WriteString(str);
			}
			str.Format(_T("G0 X%.3f Y%.3f Z%.3f i%.3f j%.3f k%.3f\n"),
				m_vecpGCodeLayers[i]->m_vpRoute[szB + 1]->x,
				m_vecpGCodeLayers[i]->m_vpRoute[szB + 1]->y,
				m_vecpGCodeLayers[i]->m_vpRoute[szB + 1]->z,
				m_vecpGCodeLayers[i]->m_vCoordinate[2].dx,
				m_vecpGCodeLayers[i]->m_vCoordinate[2].dy,
				m_vecpGCodeLayers[i]->m_vCoordinate[2].dz);
			fileout.WriteString(str);
			int sz = m_vecpGCodeLayers[i]->m_vpRoute.size();
			for (int j = szB + 2; j < sz; j++)
			{
				length = GetDistance(*m_vecpGCodeLayers[i]->m_vpRoute[j - 1], *m_vecpGCodeLayers[i]->m_vpRoute[j]);
				volumn += length * 0.2;
				str.Format(_T("G1 X%.3f Y%.3f Z%.3f i%.3f j%.3f k%.3f E-%.5f\n"),
					m_vecpGCodeLayers[i]->m_vpRoute[j]->x, m_vecpGCodeLayers[i]->m_vpRoute[j]->y, m_vecpGCodeLayers[i]->m_vpRoute[j]->z,(volumn / 6.0),
					m_vecpGCodeLayers[i]->m_vCoordinate[2].dx,
					m_vecpGCodeLayers[i]->m_vCoordinate[2].dy,
					m_vecpGCodeLayers[i]->m_vCoordinate[2].dz);
				fileout.WriteString(str);
			}
		}
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G91 ;\n"));
		fileout.WriteString(_T("T0 ;\n"));
		fileout.WriteString(_T("G1 E-1 ;\n"));
		fileout.WriteString(_T("M104 T0 S0 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M140 S0 ;\n"));
		fileout.WriteString(_T("M84 ;\n"));
		fileout.Close();
	}
}

void CGCode::WriteFiveAxis(CString sFilePath)
{
	CStdioFile fileout;
	if (fileout.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		fileout.WriteString(_T("G28 ;\n"));
		fileout.WriteString(_T("G1 Z300 F2000 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("M82 ;\n"));
		fileout.WriteString(_T("M104 T0 S210 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M109 T0 S210 ;\n"));
		fileout.WriteString(_T("G1 X-2.12 Y109.40 Z138.5 ;\n"));
		fileout.WriteString(_T("G92 X0 Y0 Z0 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		CString str;
		double length = 0.0000;
		double volumn = 0.0000;
		int szLayer = m_vecpGCodeLayers.size();
		for (int i = 0; i < szLayer; i++)
		{
			if (m_vecpGCodeLayers[i]->m_vCoordinate[2].dz == 1.0)
			{
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
					m_vecpGCodeLayers[i]->m_vpRoute[0]->x,
					m_vecpGCodeLayers[i]->m_vpRoute[0]->y,
					m_vecpGCodeLayers[i]->m_vpRoute[0]->z);
				fileout.WriteString(str);
				int szB = m_vecpGCodeLayers[i]->m_vpBoundaries[0]->m_vpSegments.size();
				for (int j = 1; j < szB+1; j++)
				{
					length = GetDistance(*m_vecpGCodeLayers[i]->m_vpRoute[j - 1], *m_vecpGCodeLayers[i]->m_vpRoute[j]);
					volumn += length * 0.2;
					str.Format(_T("G1 X%.3f Y%.3f E-%.5f\n"),
						m_vecpGCodeLayers[i]->m_vpRoute[j]->x, m_vecpGCodeLayers[i]->m_vpRoute[j]->y, (volumn / 6.0));
					fileout.WriteString(str);
				}
				str.Format(_T("G0 X%.3f Y%.3f\n"),
					m_vecpGCodeLayers[i]->m_vpRoute[szB + 1]->x,
					m_vecpGCodeLayers[i]->m_vpRoute[szB + 1]->y);
				fileout.WriteString(str);
				int sz = m_vecpGCodeLayers[i]->m_vpRoute.size();
				for (int j = szB+2; j < sz; j++)
				{
					length = GetDistance(*m_vecpGCodeLayers[i]->m_vpRoute[j - 1], *m_vecpGCodeLayers[i]->m_vpRoute[j]);
					volumn += length * 0.2;
					str.Format(_T("G1 X%.3f Y%.3f E-%.5f\n"),
						m_vecpGCodeLayers[i]->m_vpRoute[j]->x, m_vecpGCodeLayers[i]->m_vpRoute[j]->y, (volumn / 6.0));
					fileout.WriteString(str);
				}
			}
			else
			{
				//喷头先往上抬20mm
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
					m_vecpGCodeLayers[i]->m_vpTurnRoute[0]->x,
					m_vecpGCodeLayers[i]->m_vpTurnRoute[0]->y,
					m_vecpGCodeLayers[i]->m_vpTurnRoute[0]->z + 20.0);
				fileout.WriteString(str);
				//AC轴转动
				str.Format(_T("G0 A%.3f C%.3f\n"),
					(m_vecpGCodeLayers[i]->m_vpTurnRoute[0]->A * 180 / PI),
					(m_vecpGCodeLayers[i]->m_vpTurnRoute[0]->C * 180 / PI));
				fileout.WriteString(str);
				//喷头移动到要打印的点
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
					m_vecpGCodeLayers[i]->m_vpTurnRoute[0]->x,
					m_vecpGCodeLayers[i]->m_vpTurnRoute[0]->y,
					m_vecpGCodeLayers[i]->m_vpTurnRoute[0]->z);
				fileout.WriteString(str);

				for (int j = 1; j < m_vecpGCodeLayers[i]->m_vpTurnRoute.size(); j++)
				{
					length = GetDistance(*m_vecpGCodeLayers[i]->m_vpRoute[j - 1], *m_vecpGCodeLayers[i]->m_vpRoute[j]);
					volumn += length * 0.2*1.25;
					str.Format(_T("G1 X%.3f Y%.3f E-%.5f\n"),
						m_vecpGCodeLayers[i]->m_vpTurnRoute[j]->x,
						m_vecpGCodeLayers[i]->m_vpTurnRoute[j]->y, (volumn / 6.0));
					fileout.WriteString(str);
				}

				//喷头抬升20mm
				str.Format(_T("G0 Z%.3f\n"),
					m_vecpGCodeLayers[i]->m_vpTurnRoute[0]->z + 20);
				fileout.WriteString(str);
				//AC轴反转
				str.Format(_T("G0 C0 A0\n"));
				fileout.WriteString(str);
			}
		}
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G91 ;\n"));
		fileout.WriteString(_T("T0 ;\n"));
		fileout.WriteString(_T("G1 E-1 ;\n"));
		fileout.WriteString(_T("M104 T0 S0 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M140 S0 ;\n"));
		fileout.WriteString(_T("M84 ;\n"));
		fileout.Close();
	}
}

void CGCode::LoadSweepModel(CSweep* sweepModel)
{
	int sz = sweepModel->m_vpSweepLayers.size();
	for (int i = 0; i < sz; i++)
	{
		m_vecpGCodeLayers.push_back(new CSweepLayer(*sweepModel->m_vpSweepLayers[i]));
	}
}
