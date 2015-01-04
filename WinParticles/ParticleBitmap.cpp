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
	LoadDefaultBitmap();
}

CParticleBitmap::CParticleBitmap(const CParticleBitmap &other) : CParticleBitmap()
{
	*this = other;
}

CParticleBitmap::~CParticleBitmap()
{
	DeleteDC(bitmapDC);
	DeleteObject(bitmap);
}

const CParticleBitmap &CParticleBitmap::operator=(const CParticleBitmap &other)
{
	Resize(other.cellWidth, other.cellHeight, other.cellCount);
	BitBlt(bitmapDC, 0, 0, cellWidth * cellCount, cellHeight, other.bitmapDC, 0, 0, SRCCOPY);
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

int CParticleBitmap::GetPixelCount()
{
	return cellWidth * cellHeight * cellCount;
}

void CParticleBitmap::Resize(int cellWidth, int cellHeight, int cellCount)
{
	int width = cellWidth * cellCount;
	HBITMAP oldBitmap = bitmap;
	HDC oldBitmapDC = bitmapDC;
	bitmap = CreateCompatibleBitmap(bitmapDC, width, cellHeight);
	bitmapDC = CreateCompatibleDC(bitmapDC);
	SelectObject(bitmapDC, bitmap);
	
	PatBlt(bitmapDC, 0, 0, width, cellHeight, WHITENESS);
	for (int i = 0; i < this->cellCount; i++) {
		// Copy over each cell from the old bitmap
		BitBlt(bitmapDC, i * cellWidth, 0, this->cellWidth, this->cellHeight, oldBitmapDC, i * this->cellWidth, 0, SRCCOPY);
	}
	
	DeleteDC(oldBitmapDC);
	DeleteObject(oldBitmap);

	this->cellWidth = cellWidth;
	this->cellHeight = cellHeight;
	this->cellCount = cellCount;
}

void CParticleBitmap::Draw(HDC hDC, int ctrX, int ctrY, COLORREF color, int cellNum)
{
	LONG x = ctrX - cellWidth / 2;
	LONG y = ctrY - cellHeight / 2;

	if (additiveDrawing) {
		SetBkColor(hDC, 0);
		SetTextColor(hDC, color);
		BitBlt(hDC, x, y, cellWidth, cellHeight, bitmapDC, cellNum * cellWidth, 0, SRCPAINT);
	} else {
		SelectObject(hDC, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hDC, color);
		WorkingMaskBlt(hDC, x, y, cellWidth, cellHeight, hDC, 0, 0, bitmap, cellNum * cellWidth, 0, MAKEROP4(0xAA0029, PATCOPY));
	}
}

void CParticleBitmap::Draw(HDC hDC, int ctrX, int ctrY, COLORREF color, double relativeAge)
{
	Draw(hDC, ctrX, ctrY, color, (int)(relativeAge * (double)cellCount));
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

void CParticleBitmap::Scale(int factor)
{
	// Negative factor scales by 1/abs(factor)
	int width = cellWidth * cellCount;
	int newWidth, newHeight;
	if (factor < 0) {
		factor = -factor;
		newWidth = width / factor;
		newHeight = cellHeight / factor;
	} else {
		newWidth = width * factor;
		newHeight = cellHeight * factor;
	}

	HBITMAP oldBitmap = bitmap;
	HDC oldBitmapDC = bitmapDC;

	bitmap = CreateCompatibleBitmap(bitmapDC, newWidth, newHeight);
	bitmapDC = CreateCompatibleDC(bitmapDC);
	SelectObject(bitmapDC, bitmap);

	StretchBlt(bitmapDC, 0, 0, newWidth, newHeight, oldBitmapDC, 0, 0, width, cellHeight, SRCCOPY);

	DeleteDC(oldBitmapDC);
	DeleteObject(oldBitmap);
	
	cellWidth = newWidth / cellCount;
	cellHeight = newHeight;
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

void CParticleBitmap::ShiftAllCells(int xOffset, int yOffset)
{
	HBITMAP tempBitmap = CreateCompatibleBitmap(bitmapDC, cellWidth, cellHeight);
	HDC tempDC = CreateCompatibleDC(NULL);
	SelectObject(tempDC, tempBitmap);

	for (int i = 0; i < cellCount; i++) {
		PatBlt(tempDC, 0, 0, cellWidth, cellHeight, WHITENESS);
		BitBlt(tempDC, xOffset, yOffset, cellWidth, cellHeight, bitmapDC, i * cellWidth, 0, SRCCOPY);
		BitBlt(bitmapDC, i * cellWidth, 0, cellWidth, cellHeight, tempDC, 0, 0, SRCCOPY);
	}

	DeleteObject(tempBitmap);
	DeleteDC(tempDC);
}

void CParticleBitmap::FloodFill(int x, int y)
{
	COLORREF color = ::GetPixel(bitmapDC, x, y);
	SelectObject(bitmapDC, GetStockObject(color ? BLACK_BRUSH : WHITE_BRUSH));
	ExtFloodFill(bitmapDC, x, y, color, FLOODFILLSURFACE);
}