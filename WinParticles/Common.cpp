#include "stdafx.h"
#include "Common.h"
#include <tchar.h>

double Interpolate(double value, double inputMin, double inputMax, double outputMin, double outputMax)
{
	double x = (value - inputMin) / (inputMax - inputMin);
	return x * (outputMax - outputMin) + outputMin;
}

COLORREF MultiplyColors(COLORREF a, COLORREF b)
{
	int red1 = GetRValue(a), red2 = GetRValue(b);
	int green1 = GetGValue(a), green2 = GetGValue(b);
	int blue1 = GetBValue(a), blue2 = GetBValue(b);
	return RGB(red1 * red2 / 255, green1 * green2 / 255, blue1 * blue2 / 255);
}

double RandInRange(double min, double max)
{
	return ((double)rand() / RAND_MAX) * (max - min) + min;
}

BOOL WorkingMaskBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, HBITMAP hbmMask, int xMask, int yMask, DWORD dwRop)
{
	// Hack to fix a bug with negative destination coordinates in MaskBlt when using PATCOPY

	if (nXDest < 0) {
		int oldXDest = nXDest;
		nXDest = 0;
		nWidth += oldXDest;
		nXSrc -= oldXDest;
		xMask -= oldXDest;
	}

	if (nYDest < 0) {
		int oldYDest = nYDest;
		nYDest = 0;
		nHeight += oldYDest;
		nYSrc -= oldYDest;
		yMask -= oldYDest;
	}

	return MaskBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, hbmMask, xMask, yMask, dwRop);
}

void ColorToHTML(COLORREF color, std::string &str)
{
	char buf[8];
	sprintf_s(buf, 8, "#%02X%02X%02X", GetRValue(color), GetGValue(color), GetBValue(color));
	str = buf;
}

void ColorToHTML(COLORREF color, std::wstring &str)
{
	wchar_t buf[8];
	swprintf_s(buf, 8, L"#%02X%02X%02X", GetRValue(color), GetGValue(color), GetBValue(color));
	str = buf;
}