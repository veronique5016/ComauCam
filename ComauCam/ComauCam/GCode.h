#pragma once
#include "Sweep.h"
#include <vector>

using namespace std;

class GCode
{
public:
	GCode();
	~GCode();

public:
	void writeGCode(CString sFilePath);
	void writeFiveAxisGCode(CString sFilePath);
	void loadSweepModel(CSweep* sweepModel);

public:
	vector<SweepLayer*> m_gcode_layers;
};
