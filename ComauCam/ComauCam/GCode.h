#pragma once
#include "Sweep.h"
#include <vector>

using namespace std;

class CGCode
{
public:
	CGCode();
	~CGCode();

public:
	void Write(CString sFilePath);
	void WriteFiveAxis(CString sFilePath);
	void LoadSweepModel(CSweep* sweepModel);

public:
	vector<CSweepLayer*> m_vecpGCodeLayers;
};
