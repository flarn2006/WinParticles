#pragma once
#include <Windows.h>
#include <string>
#include <sstream>

#ifdef _UNICODE
	typedef wchar_t tchar_t;
	typedef std::wstring tstring;
	typedef std::wistringstream tistringstream;
	typedef std::wostringstream tostringstream;
#else
	typedef char tchar_t;
	typedef std::string tstring;
	typedef std::istringstream tistringstream;
	typedef std::ostringstream tostringstream;
#endif

double Interpolate(double value, double inputMin, double inputMax, double outputMin, double outputMax);

COLORREF MultiplyColors(COLORREF a, COLORREF b);

double RandInRange(double min, double max);

BOOL WorkingMaskBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, HBITMAP hbmMask, int xMask, int yMask, DWORD dwRop);