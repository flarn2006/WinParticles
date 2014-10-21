#include "stdafx.h"
#include "PresetManager.h"
#include "ParticleBitmap.h"
#include <fstream>
#include <sstream>

extern CParticleBitmap bitmap;

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

bool CPresetManager::SplitString(std::string &input, char splitChar, std::string &left, std::string &right)
{
	std::string::size_type offset = input.find(splitChar);
	if (offset == std::string::npos) return false;
	left = input.substr(0, offset);
	right = input.substr(offset + 1);
	return true;
}

bool CPresetManager::SavePreset(LPCTSTR filename, CPresetManager::Components componentsToSave)
{
	std::ofstream file(filename, std::ofstream::trunc);
	if (file.bad()) return false;

	CParticleSys::VelocityMode velMode = psys->GetVelocityMode();
	double temp1, temp2;

	if (componentsToSave & PMC_BASIC_PARAMS) {
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
	}

	if (componentsToSave & PMC_GRADIENT) {
		CGradient *gradient = psys->GetDefGradient();
		unsigned int stepCount = gradient->GetStepCount();
		for (unsigned int i = 0; i < stepCount; i++) {
			file << "GradientStep=" << gradient->GetStepPosition(i) << "," << std::hex << gradient->GetStepColor(i) << std::endl;
		}

		file << std::endl;
	}

	if (componentsToSave & PMC_BITMAP) {
		int width = bitmap.GetCellWidth() * bitmap.GetCellCount();
		int height = bitmap.GetCellHeight();
		file << "BitmapCellWidth=" << bitmap.GetCellWidth() << std::endl;
		file << "BitmapCellHeight=" << height << std::endl;
		file << "BitmapCellCount=" << bitmap.GetCellCount() << std::endl;
		file << "StartBitmap" << std::endl;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				file << (bitmap.GetPixel(x, y) ? '#' : '_');
			}
			file << std::endl;
		}
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
	int bitmapCellWidth = 5;
	int bitmapCellHeight = 5;
	int bitmapCellCount = 6;
	while (std::getline(file, line)) {
		if (line.compare("StartBitmap") == 0) {
			bitmapRow = 0;
			bitmap.Resize(bitmapCellWidth, bitmapCellHeight, bitmapCellCount);
		} else if (bitmapRow == -1) {
			std::string left, right;

			if (SplitString(line, '=', left, right)) {
				std::istringstream parseRight(right);

				if (left.compare("VelocityMode") == 0) {
					if (right.compare("POLAR") == 0) {
						psys->SetVelocityMode(CParticleSys::VelocityMode::MODE_POLAR);
					} else if (right.compare("RECTANGULAR") == 0) {
						psys->SetVelocityMode(CParticleSys::VelocityMode::MODE_RECT);
					} else {
						loadError = L"Unrecognized velocity mode";
						return false;
					}

				} else if (left.compare("MinVelocity") == 0) {
					double min, max;
					psys->GetVelocity(&min, &max);
					parseRight >> min;
					psys->SetVelocity(min, max);

				} else if (left.compare("MaxVelocity") == 0) {
					double min, max;
					psys->GetVelocity(&min, &max);
					parseRight >> max;
					psys->SetVelocity(min, max);

				} else if (left.compare("MinVelocityX") == 0) {
					double min, max;
					psys->GetRectVelocityX(&min, &max);
					parseRight >> min;
					psys->SetRectVelocityX(min, max);

				} else if (left.compare("MaxVelocityX") == 0) {
					double min, max;
					psys->GetRectVelocityX(&min, &max);
					parseRight >> max;
					psys->SetRectVelocityX(min, max);

				} else if (left.compare("MinVelocityY") == 0) {
					double min, max;
					psys->GetRectVelocityY(&min, &max);
					parseRight >> min;
					psys->SetRectVelocityY(min, max);

				} else if (left.compare("MaxVelocityY") == 0) {
					double min, max;
					psys->GetRectVelocityY(&min, &max);
					parseRight >> max;
					psys->SetRectVelocityY(min, max);

				} else if (left.compare("AccelerationX") == 0) {
					double x, y;
					psys->GetAcceleration(&x, &y);
					parseRight >> x;
					psys->SetAcceleration(x, y);

				} else if (left.compare("AccelerationY") == 0) {
					double x, y;
					psys->GetAcceleration(&x, &y);
					parseRight >> y;
					psys->SetAcceleration(x, y);

				} else if (left.compare("MaximumAge") == 0) {
					double maxAge;
					parseRight >> maxAge;
					psys->SetMaxAge(maxAge);

				} else if (left.compare("EmissionRate") == 0) {
					double emissionRate;
					parseRight >> emissionRate;
					psys->SetEmissionRate(emissionRate);

				} else if (left.compare("EmissionRadius") == 0) {
					double emissionRadius;
					parseRight >> emissionRadius;
					psys->SetEmissionRadius(emissionRadius);

				} else if (left.compare("GradientStep") == 0) {
					std::string left2, right2;
					if (SplitString(right, ',', left2, right2)) {
						if (!includesGradient) {
							gradient.DeleteAllSteps();
							includesGradient = true;
						}
						std::istringstream parseLeft2(left2);
						std::istringstream parseRight2(right2);
						double pos;
						COLORREF color;
						parseLeft2 >> pos;
						parseRight2 >> std::hex >> color;
						gradient.AddStep(pos, color);
					} else {
						loadError = L"GradientStep without ','";
						return false;
					}

				} else if (left.compare("BitmapCellWidth") == 0) {
					parseRight >> bitmapCellWidth;

				} else if (left.compare("BitmapCellHeight") == 0) {
					parseRight >> bitmapCellHeight;

				} else if (left.compare("BitmapCellCount") == 0) {
					parseRight >> bitmapCellCount;
				}

				if (includesGradient) gradient.PrecalculateColors();
			}
		} else {
			// bitmapRow is not -1; read a line of the bitmap
			int bitmapWidth = bitmapCellWidth * bitmapCellCount;

			if (line.length() < (unsigned)bitmapWidth) {
				loadError = L"Unexpected end of line while reading bitmap data";
				return false;
			}

			for (int i = 0; i < bitmapWidth; i++) {
				bitmap.SetPixel(i, bitmapRow, line[i] == '#');
			}

			if (++bitmapRow >= bitmapCellHeight) bitmapRow = -1;
		}
	}
	return true;
}

bool CPresetManager::SavePresetDlg(HWND parent, CPresetManager::Components componentsToSave)
{
	switch (componentsToSave) {
	case PMC_GRADIENT:
		fileDlg.lpstrTitle = L"Save Gradient";
		break;
	case PMC_BITMAP:
		fileDlg.lpstrTitle = L"Save Bitmap";
		break;
	default:
		fileDlg.lpstrTitle = L"Save Preset";
	}

	fileDlg.hwndOwner = parent;

	if (GetSaveFileName(&fileDlg))
		return SavePreset(fileDlg.lpstrFile, componentsToSave);
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