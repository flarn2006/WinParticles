#include "stdafx.h"

double Interpolate(double value, double inputMin, double inputMax, double outputMin, double outputMax)
{
	double x = (value - inputMin) / (inputMax - inputMin);
	return x * (outputMax - outputMin) + outputMin;
}

COLORREF MultiplyColors(COLORREF a, COLORREF b)
{
	float red1 = (float)GetRValue(a) / 255, red2 = (float)GetRValue(b) / 255;
	float green1 = (float)GetGValue(a) / 255, green2 = (float)GetGValue(b) / 255;
	float blue1 = (float)GetBValue(a) / 255, blue2 = (float)GetBValue(b) / 255;
	return RGB((BYTE)(255.0 * red1 * red2), (BYTE)(255.0 * green1 * green2), (BYTE)(255.0 * blue1 * blue2));
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