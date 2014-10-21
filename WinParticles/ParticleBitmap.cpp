#include "stdafx.h"
#include "ParticleBitmap.h"
#include "Resource.h"
#include "Common.h"

extern HINSTANCE hInst;
extern bool additiveDrawing;

CParticleBitmap::CParticleBitmap()
{
	bitmap = NULL;
	bitmapDC = CreateCompatibleDC(NULL);
}

CParticleBitmap::~CParticleBitmap()
{
	DeleteDC(bitmapDC);
	DeleteObject(bitmap);
}

int CParticleBitmap::GetCellWidth()
{
	return cellWidth;
}

int CParticleBitmap::GetCellHeight()
{
	return cellHeight;
}

int CParticleBitmap::GetCellCount()
{
	return cellCount;
}

void CParticleBitmap::Resize(int cellWidth, int cellHeight, int cellCount)
{
	this->cellWidth = cellWidth;
	this->cellHeight = cellHeight;
	this->cellCount = cellCount;

	int width = cellWidth * cellCount;
	HBITMAP oldBitmap = bitmap;
	HDC oldBitmapDC = bitmapDC;
	bitmap = CreateCompatibleBitmap(bitmapDC, width, cellHeight);
	bitmapDC = CreateCompatibleDC(bitmapDC);
	SelectObject(bitmapDC, bitmap);
	BitBlt(bitmapDC, 0, 0, width, cellHeight, oldBitmapDC, 0, 0, SRCCOPY);
	
	DeleteDC(oldBitmapDC);
	DeleteObject(oldBitmap);
}

void CParticleBitmap::Draw(HDC hDC, int ctrX, int ctrY, COLORREF color, double relativeAge)
{
	LONG x = ctrX - cellWidth / 2;
	LONG y = ctrY - cellHeight / 2;

	if (additiveDrawing) {
		SetBkColor(hDC, 0);
		SetTextColor(hDC, color);
		BitBlt(hDC, x, y, cellWidth, cellHeight, bitmapDC, (int)(relativeAge * (double)cellCount) * cellWidth, 0, SRCPAINT);
	} else {
		SelectObject(hDC, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hDC, color);
		WorkingMaskBlt(hDC, x, y, cellWidth, cellHeight, hDC, 0, 0, bitmap, (int)(relativeAge * (double)cellCount) * cellWidth, 0, MAKEROP4(0xAA0029, PATCOPY));
	}
}

HBITMAP CParticleBitmap::GetBitmap()
{
	return bitmap;
}

HDC CParticleBitmap::GetDC()
{
	return bitmapDC;
}

bool CParticleBitmap::GetPixel(int x, int y)
{
	return ::GetPixel(bitmapDC, x, y) == 0;
}

void CParticleBitmap::SetPixel(int x, int y, bool state)
{
	PatBlt(bitmapDC, x, y, 1, 1, state ? BLACKNESS : WHITENESS);
}

void CParticleBitmap::Clear()
{
	PatBlt(bitmapDC, 0, 0, cellWidth * cellCount, cellHeight, WHITENESS);
}

void CParticleBitmap::InvertAll()
{
	PatBlt(bitmapDC, 0, 0, cellWidth * cellCount, cellHeight, DSTINVERT);
}

void CParticleBitmap::CopyToOtherCells(int srcCell)
{
	for (int i = 0; i < cellCount; i++) {
		if (i != srcCell) {
			BitBlt(bitmapDC, i * cellWidth, 0, cellWidth, cellHeight, bitmapDC, srcCell * cellWidth, 0, SRCCOPY);
		}
	}
}

void CParticleBitmap::LoadDefaultBitmap()
{
	cellWidth = DEFAULT_CELL_WIDTH;
	cellHeight = DEFAULT_HEIGHT;
	cellCount = DEFAULT_CELL_COUNT;
	HBITMAP oldBitmap = bitmap;
	bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PARTICLES));
	SelectObject(bitmapDC, bitmap);
	if (oldBitmap) DeleteObject(oldBitmap);
}