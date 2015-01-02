#pragma once
#include <Windows.h>
#include <string>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG_TO_RAD (M_PI / 180.0)
#define ROOT8_OF_2 1.0905077326652576592070106557607

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

template <typename T> T ClampValue(T value, T min, T max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

template <typename T> T WrapValue(T value, T min, T max)
{
	if (value < min) return max - (min - value);
	if (value > max) return min + (value - max);
	return value;
}

template <typename TSpecific, typename TGeneric> bool IsOfType(TGeneric *object)
{
	return (dynamic_cast<TSpecific*>(object) != NULL);
}