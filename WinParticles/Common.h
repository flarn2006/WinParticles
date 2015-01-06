#pragma once
#include <Windows.h>
#include <string>
#include <sstream>
#include <vector>

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

// Note: "HTML" is simply referring to the color format (#RRGGBB). This program does not manipulate HTML files.
void ColorToHTML(COLORREF color, std::string &str);
void ColorToHTML(COLORREF color, std::wstring &str);

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

template <typename T> void DeleteVectorItem(std::vector<T> &vector, typename std::vector<T>::size_type index)
{
	std::vector<T>::size_type size = vector.size();
	std::vector<T> newVec;
	for (std::vector<T>::size_type i = 0; i < size; i++) {
		newVec.push_back(vector[i >= index ? i + 1 : i]);
	}
	vector = newVec;
}