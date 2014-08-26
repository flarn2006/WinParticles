#pragma once
#include <Windows.h>

class CBackBuffer
{
private:
	HBITMAP bmp;
	HDC backDC;
	HWND hWnd;
	LONG cx, cy;

public:
	CBackBuffer(HWND hWnd);
	~CBackBuffer();

	HBITMAP GetBitmap();
	HDC GetDC();
	void UpdateSize();
	void CopyToFront(HDC frontDC);
};

