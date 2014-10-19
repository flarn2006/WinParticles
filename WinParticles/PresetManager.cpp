#include "stdafx.h"
#include "PresetManager.h"
#include <fstream>

CPresetManager::CPresetManager(CParticleSys *psys)
{
	this->psys = psys;
	gradient.AddStep(0.0, 0xFF);
	gradient.AddStep(1.0, 0xFF0000);

	filename[0] = (TCHAR)0;
	fileDlg.lStructSize = sizeof(OPENFILENAME);
	fileDlg.lpstrFilter = L"Presets (*.wpp)\0*.wpp\0All files (*.*)\0*.*\0\0";
	fileDlg.lpstrCustomFilter = NULL;
	fileDlg.nFilterIndex = 0;
	fileDlg.lpstrFile = filename;
	fileDlg.nMaxFile = FILENAME_MAX_LENGTH;
	fileDlg.lpstrFileTitle = NULL;
	fileDlg.lpstrInitialDir = L"Presets";
	fileDlg.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT; //ignored in save and open dialogs respectively
	fileDlg.lpstrDefExt = L"wpp";
	fileDlg.FlagsEx = 0;
}

CPresetManager::~CPresetManager()
{
}

LPTSTR CPresetManager::SplitString(LPTSTR string, TCHAR splitChar)
{
	int length = lstrlen(string);
	for (int i = 0; i < length; i++) {
		if (string[i] == splitChar) {
			string[i] = (TCHAR)0;
			return &string[i + 1];
		}
	}
	return NULL; //if splitChar was not found
}

bool CPresetManager::SavePreset(LPCTSTR filename)
{
	std::ofstream file(filename, std::ofstream::trunc);
	if (file.bad()) return false;

	CParticleSys::VelocityMode velMode = psys->GetVelocityMode();
	double temp1, temp2;

	file << "VelocityMode=" << CParticleSys::VelocityModeText(velMode) << std::endl;

	if (velMode == CParticleSys::VelocityMode::MODE_POLAR) {
		psys->GetVelocity(&temp1, &temp2);
		file << "MinVelocity=" << temp1 << std::endl;
		file << "MaxVelocity=" << temp2 << std::endl;
	} else {
		psys->GetRectVelocityX(&temp1, &temp2);
		file << "MinVelocityX=" << temp1 << std::endl;
		file << "MaxVelocityX=" << temp2 << std::endl;
		psys->GetRectVelocityY(&temp1, &temp2);
		file << "MinVelocityY=" << temp1 << std::endl;
		file << "MaxVelocityY=" << temp2 << std::endl;
	}

	psys->GetAcceleration(&temp1, &temp2);
	file << "AccelerationX=" << temp1 << std::endl;
	file << "AccelerationY=" << temp2 << std::endl;
	
	file << "MaximumAge=" << psys->GetMaxAge() << std::endl;
	file << "EmissionRate=" << psys->GetEmissionRate() << std::endl;
	file << "EmissionRadius=" << psys->GetEmissionRadius() << std::endl;

	file << std::endl;

	CGradient *gradient = psys->GetDefGradient();
	unsigned int stepCount = gradient->GetStepCount();
	for (unsigned int i = 0; i < stepCount; i++) {
		file << "GradientStep=" << gradient->GetStepPosition(i) << "," << std::hex << gradient->GetStepColor(i) << std::endl;
	}

	file.close();
	return true;
}

bool CPresetManager::LoadPreset(LPCTSTR filename)
{
	return false; //NYI
}

bool CPresetManager::SavePresetDlg(HWND parent)
{
	fileDlg.lpstrTitle = L"Save Preset";
	fileDlg.hwndOwner = parent;
	if (GetSaveFileName(&fileDlg))
		return SavePreset(fileDlg.lpstrFile);
	else
		return false;
}

bool CPresetManager::LoadPresetDlg(HWND parent)
{
	MessageBox(parent, L"Sorry, this function is not yet implemented.", L"Error", MB_ICONEXCLAMATION);
	return false;
}