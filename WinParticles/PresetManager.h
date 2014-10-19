#pragma once
#include <Windows.h>
#include <commdlg.h>
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

	static LPTSTR SplitString(LPTSTR string, TCHAR splitChar);

public:
	CPresetManager(CParticleSys *psys);
	~CPresetManager();
	bool SavePreset(LPCTSTR filename);
	bool LoadPreset(LPCTSTR filename);
	bool SavePresetDlg(HWND parent);
	bool LoadPresetDlg(HWND parent);
};

