#pragma once
#include <Windows.h>

double Interpolate(double value, double inputMin, double inputMax, double outputMin, double outputMax);

COLORREF MultiplyColors(COLORREF a, COLORREF b);

double RandInRange(double min, double max);

BOOL WorkingMaskBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, HBITMAP hbmMask, int xMask, int yMask, DWORD dwRop);