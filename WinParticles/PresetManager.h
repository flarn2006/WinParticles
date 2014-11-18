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
	CGradient *gradient;
	OPENFILENAME fileDlg;
	TCHAR filename[FILENAME_MAX_LENGTH];
	LPCTSTR loadError;
	bool includesGradient;

	static bool SplitString(std::string &input, char splitChar, std::string &left, std::string &right);

public:
	enum Components {
		PMC_BASIC_PARAMS = 1,
		PMC_GRADIENT = 2,
		PMC_BITMAP = 4,
		PMC_ALL = 7,
	};

	CPresetManager(CParticleSys *psys);
	~CPresetManager();
	bool SavePreset(LPCTSTR filename, Components componentsToSave = PMC_ALL);
	bool LoadPreset(LPCTSTR filename);
	bool SavePresetDlg(HWND parent, Components componentsToSave = PMC_ALL);
	bool LoadPresetDlg(HWND parent);
	bool DidLastPresetIncludeGradient();
	CGradient *GetGradient();
};

