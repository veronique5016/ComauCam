#include "stdafx.h"
#include "GCode.h"

GCode::GCode()
{
}

GCode::~GCode()
{
	unsigned int szL = m_gcode_layers.size();
	for (unsigned int i = 0; i < szL; i++)
	{
		delete m_gcode_layers[i];
		m_gcode_layers[i] = NULL;
	}
	m_gcode_layers.clear();
}

void GCode::writeGCode(CString sFilePath)
{
	CStdioFile fileout;
	if (fileout.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		fileout.WriteString(_T("G28 ;\n"));
		fileout.WriteString(_T("G1 Z15 F1500 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		fileout.WriteString(_T("G90 ;\n"));
		fileout.WriteString(_T("M82 ;\n"));
		fileout.WriteString(_T("M190 S50 ;\n"));
		fileout.WriteString(_T("M104 T0 S210 ;\n"));
		fileout.WriteString(_T("G92 E0 ;\n"));
		fileout.WriteString(_T("M109 T0 S210 ;\n"));
		fileout.WriteString(_T("M107 ;\n"));
		CString str;
		double length = 0.0000;
		double volumn = 0.0000;
		unsigned int szLayer = m_gcode_layers.size();
		for (unsigned int i = 0; i < szLayer; i++)
		{
			str.Format(_T("G0 X%.3f Y%.3f z%.3f\n"),
				m_gcode_layers[i]->m_Route[0]->x,
				m_gcode_layers[i]->m_Route[0]->y,
				m_gcode_layers[i]->m_Route[0]->z);
			fileout.WriteString(str);
			unsigned int sz = m_gcode_layers[i]->m_Route.size();

			for (unsigned int j = 1; j < sz; j++)
			{
				length = GetDistance(*m_gcode_layers[i]->m_Route[j - 1], *m_gcode_layers[i]->m_Route[j]);
				volumn += length * 0.2;
				str.Format(_T("G1 X%.3f Y%.3f Z%.3f i%.3f j%.3f k%.3f E%.5f\n"),
					m_gcode_layers[i]->m_Route[j]->x, m_gcode_layers[i]->m_Route[j]->y, m_gcode_layers[i]->m_Route[j]->z,
					m_gcode_layers[i]->layer_coordinate[2].dx,
					m_gcode_layers[i]->layer_coordinate[2].dy,
					m_gcode_layers[i]->layer_coordinate[2].dz,
					(volumn / 6.0));
				fileout.WriteString(str);
			}
			if (i != szLayer - 1)
			{
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f i%.3f j%.3f k%.3f\n"),
					m_gcode_layers[i + 1]->m_Route[0]->x, m_gcode_layers[i]->m_Route[0]->y, m_gcode_layers[i]->m_Route[0]->z,
					m_gcode_layers[i]->layer_coordinate[2].dx,
					m_gcode_layers[i]->layer_coordinate[2].dy,
					m_gcode_layers[i]->layer_coordinate[2].dz);
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

void GCode::writeFiveAxisGCode(CString sFilePath)
{
	CStdioFile fileout;
	if (fileout.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		fileout.WriteString(_T("G28 ;\n"));
		fileout.WriteString(_T("G280 ;\n"));
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
		unsigned int szLayer = m_gcode_layers.size();
		for (unsigned int i = 0; i < szLayer; i++)
		{
			if (m_gcode_layers[i]->layer_coordinate[2].dz == 1.0)
			{
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
					m_gcode_layers[i]->m_Route[0]->x,
					m_gcode_layers[i]->m_Route[0]->y,
					m_gcode_layers[i]->m_Route[0]->z);
				fileout.WriteString(str);
				unsigned int szB = m_gcode_layers[i]->m_Boundaries[0]->m_segments.size();
				for (unsigned int j = 1; j < szB+1; j++)
				{
					length = GetDistance(*m_gcode_layers[i]->m_Route[j - 1], *m_gcode_layers[i]->m_Route[j]);
					volumn += length * 0.2;
					str.Format(_T("G1 X%.3f Y%.3f E-%.5f\n"),
						m_gcode_layers[i]->m_Route[j]->x, m_gcode_layers[i]->m_Route[j]->y, (volumn / 6.0));
					fileout.WriteString(str);
				}
				str.Format(_T("G0 X%.3f Y%.3f\n"),
					m_gcode_layers[i]->m_Route[szB + 1]->x,
					m_gcode_layers[i]->m_Route[szB + 1]->y);
				fileout.WriteString(str);
				unsigned int sz = m_gcode_layers[i]->m_Route.size();
				for (unsigned int j = szB+2; j < sz; j++)
				{
					length = GetDistance(*m_gcode_layers[i]->m_Route[j - 1], *m_gcode_layers[i]->m_Route[j]);
					volumn += length * 0.2;
					str.Format(_T("G1 X%.3f Y%.3f E-%.5f\n"),
						m_gcode_layers[i]->m_Route[j]->x, m_gcode_layers[i]->m_Route[j]->y, (volumn / 6.0));
					fileout.WriteString(str);
				}
			}
			else
			{
				//喷头先往上抬20mm
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
					m_gcode_layers[i]->m_turnRoute[0]->x,
					m_gcode_layers[i]->m_turnRoute[0]->y,
					m_gcode_layers[i]->m_turnRoute[0]->z + 20.0);
				fileout.WriteString(str);
				//AC轴转动
				str.Format(_T("G0 A%.3f C%.3f\n"),
					(m_gcode_layers[i]->m_turnRoute[0]->A * 180 / PI),
					(m_gcode_layers[i]->m_turnRoute[0]->C * 180 / PI));
				fileout.WriteString(str);
				//喷头移动到要打印的点
				str.Format(_T("G0 X%.3f Y%.3f Z%.3f\n"),
					m_gcode_layers[i]->m_turnRoute[0]->x,
					m_gcode_layers[i]->m_turnRoute[0]->y,
					m_gcode_layers[i]->m_turnRoute[0]->z);
				fileout.WriteString(str);

				for (unsigned int j = 1; j < m_gcode_layers[i]->m_turnRoute.size(); j++)
				{
					length = GetDistance(*m_gcode_layers[i]->m_Route[j - 1], *m_gcode_layers[i]->m_Route[j]);
					volumn += length * 0.2*1.25;
					str.Format(_T("G1 X%.3f Y%.3f E-%.5f\n"),
						m_gcode_layers[i]->m_turnRoute[j]->x,
						m_gcode_layers[i]->m_turnRoute[j]->y, (volumn / 6.0));
					fileout.WriteString(str);
				}

				//喷头抬升50mm
				str.Format(_T("G0 Z%.3f\n"),
					m_gcode_layers[i]->m_turnRoute[0]->z + 20);
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

void GCode::loadSweepModel(CSweep* sweepModel)
{
	unsigned int sz = sweepModel->m_sweep_layers.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		m_gcode_layers.push_back(new SweepLayer(*sweepModel->m_sweep_layers[i]));
	}
}
