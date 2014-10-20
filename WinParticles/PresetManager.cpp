#include "stdafx.h"
#include "PresetManager.h"
#include <fstream>
#include <sstream>

extern HDC particleBmpDC;

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

char *CPresetManager::SplitString(char *string, char splitChar)
{
	int length = lstrlenA(string);
	for (int i = 0; i < length; i++) {
		if (string[i] == splitChar) {
			string[i] = (TCHAR)0;
			return &string[i + 1];
		}
	}
	return NULL; //if splitChar was not found
}

bool CPresetManager::SplitString(std::string &input, char splitChar, std::string **left, std::string **right)
{
	std::string::size_type offset = input.find(splitChar);
	if (offset == std::string::npos) return false;
	*left = new std::string(input.substr(0, offset));
	*right = new std::string(input.substr(offset + 1));
	return true;
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

	file << std::endl;
	file << "StartBitmap" << std::endl;
	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 30; x++) {
			file << (GetPixel(particleBmpDC, x, y) > 0 ? '_' : '#');
		}
		file << std::endl;
	}

	file.close();
	return true;
}

bool CPresetManager::LoadPreset(LPCTSTR filename)
{
	includesGradient = false;

	std::ifstream file(filename);
	if (file.bad()) return false;

	int bitmapRow = -1; //stores current row of bitmap being read; -1 means it's not a bitmap line
	std::string line;
	while (std::getline(file, line)) {
		if (line.compare("StartBitmap") == 0) {
			bitmapRow = 0;
		} else if (bitmapRow == -1) {
			std::string *left;
			std::string *right;

			if (SplitString(line, '=', &left, &right)) {
				std::istringstream parseRight(*right);

				if (left->compare("VelocityMode") == 0) {
					if (right->compare("POLAR") == 0) {
						psys->SetVelocityMode(CParticleSys::VelocityMode::MODE_POLAR);
					} else if (right->compare("RECTANGULAR") == 0) {
						psys->SetVelocityMode(CParticleSys::VelocityMode::MODE_RECT);
					} else {
						loadError = L"Unrecognized velocity mode";
						return false;
					}

				} else if (left->compare("MinVelocity") == 0) {
					double min, max;
					psys->GetVelocity(&min, &max);
					parseRight >> min;
					psys->SetVelocity(min, max);

				} else if (left->compare("MaxVelocity") == 0) {
					double min, max;
					psys->GetVelocity(&min, &max);
					parseRight >> max;
					psys->SetVelocity(min, max);

				} else if (left->compare("MinVelocityX") == 0) {
					double min, max;
					psys->GetRectVelocityX(&min, &max);
					parseRight >> min;
					psys->SetRectVelocityX(min, max);

				} else if (left->compare("MaxVelocityX") == 0) {
					double min, max;
					psys->GetRectVelocityX(&min, &max);
					parseRight >> max;
					psys->SetRectVelocityX(min, max);

				} else if (left->compare("MinVelocityY") == 0) {
					double min, max;
					psys->GetRectVelocityY(&min, &max);
					parseRight >> min;
					psys->SetRectVelocityY(min, max);

				} else if (left->compare("MaxVelocityY") == 0) {
					double min, max;
					psys->GetRectVelocityY(&min, &max);
					parseRight >> max;
					psys->SetRectVelocityY(min, max);

				} else if (left->compare("AccelerationX") == 0) {
					double x, y;
					psys->GetAcceleration(&x, &y);
					parseRight >> x;
					psys->SetAcceleration(x, y);

				} else if (left->compare("AccelerationY") == 0) {
					double x, y;
					psys->GetAcceleration(&x, &y);
					parseRight >> y;
					psys->SetAcceleration(x, y);

				} else if (left->compare("MaximumAge") == 0) {
					double maxAge;
					parseRight >> maxAge;
					psys->SetMaxAge(maxAge);

				} else if (left->compare("EmissionRate") == 0) {
					double emissionRate;
					parseRight >> emissionRate;
					psys->SetEmissionRate(emissionRate);

				} else if (left->compare("EmissionRadius") == 0) {
					double emissionRadius;
					parseRight >> emissionRadius;
					psys->SetEmissionRadius(emissionRadius);

				} else if (left->compare("GradientStep") == 0) {
					std::string *left2;
					std::string *right2;
					if (SplitString(*right, ',', &left2, &right2)) {
						if (!includesGradient) {
							gradient.DeleteAllSteps();
							includesGradient = true;
						}
						std::istringstream parseLeft2(*left2);
						std::istringstream parseRight2(*right2);
						double pos;
						COLORREF color;
						parseLeft2 >> pos;
						parseRight2 >> std::hex >> color;
						gradient.AddStep(pos, color);
					} else {
						loadError = L"GradientStep without ','";
						return false;
					}
				}

				delete left;
				delete right;
			}
		} else {
			// bitmapRow is not -1; read a line of the bitmap
			if (line.length() < 30) {
				loadError = L"Unexpected end of line while reading bitmap data";
				return false;
			}

			for (int i = 0; i < 30; i++) {
				PatBlt(particleBmpDC, i, bitmapRow, 1, 1, (line[i] == '#') ? BLACKNESS : WHITENESS);
			}

			if (++bitmapRow > 4) bitmapRow = -1;
		}
	}
	return true;
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
	fileDlg.lpstrTitle = L"Open Preset";
	fileDlg.hwndOwner = parent;
	if (GetOpenFileName(&fileDlg)) {
		bool result = LoadPreset(fileDlg.lpstrFile);
		if (!result) MessageBox(parent, loadError, L"Error loading preset", MB_ICONERROR);
		return result;
	} else {
		return false;
	}
}

bool CPresetManager::DidLastPresetIncludeGradient()
{
	return includesGradient;
}

CGradient *CPresetManager::GetGradient()
{
	return &gradient;
}