#pragma once
#include <Windows.h>
#include <commdlg.h>
#include <string>
#include "ParticleSys.h"
#include "Gradient.h"

class CPresetManager
{
private:
	static const int FILENAME_MAX_LENGTH = 1024;

	CParticleSys *psys;
	CGradient gradient;
	OPENFILENAME fileDlg;
	TCHAR filename[FILENAME_MAX_LENGTH];
	LPCTSTR loadError;
	bool includesGradient;

	static bool SplitString(std::string &input, char splitChar, std::string &left, std::string &right);

public:
	CPresetManager(CParticleSys *psys);
	~CPresetManager();
	bool SavePreset(LPCTSTR filename);
	bool LoadPreset(LPCTSTR filename);
	bool SavePresetDlg(HWND parent);
	bool LoadPresetDlg(HWND parent);
	bool DidLastPresetIncludeGradient();
	CGradient *GetGradient();
};

