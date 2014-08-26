#include "stdafx.h"
#include "BackBuffer.h"


CBackBuffer::CBackBuffer(HWND hWnd)
{
	this->hWnd = hWnd;
	this->backDC = NULL;
	this->bmp = NULL;
	UpdateSize();
}


CBackBuffer::~CBackBuffer()
{
	DeleteObject(bmp);
	DeleteDC(backDC);
}

HBITMAP CBackBuffer::GetBitmap()
{
	return bmp;
}

HDC CBackBuffer::GetDC()
{
	return backDC;
}

void CBackBuffer::UpdateSize()
{
	RECT client;
	GetClientRect(hWnd, &client);
	cx = client.right - client.left;
	cy = client.bottom - client.top;

	HBITMAP oldBmp = bmp;
	HDC oldBackDC = backDC;
	HDC screenDC = GetWindowDC(NULL);

	bmp = CreateCompatibleBitmap(screenDC, cx, cy);
	backDC = CreateCompatibleDC(screenDC);
	SelectObject(backDC, bmp);

	if (oldBackDC != NULL) {
		BitBlt(backDC, 0, 0, cx, cy, oldBackDC, 0, 0, SRCCOPY);
		DeleteObject(oldBmp);
		DeleteDC(oldBackDC);
	}

	ReleaseDC(NULL, screenDC);
}

void CBackBuffer::CopyToFront(HDC frontDC)
{
	BitBlt(frontDC, 0, 0, cx, cy, backDC, 0, 0, SRCCOPY);
}