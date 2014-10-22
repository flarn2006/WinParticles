#pragma once
#include <Windows.h>

class CParticleBitmap
{
private:
	static const int DEFAULT_CELL_WIDTH = 5;
	static const int DEFAULT_HEIGHT = 5;
	static const int DEFAULT_CELL_COUNT = 6;
	HBITMAP bitmap;
	HDC bitmapDC;
	int cellWidth, cellHeight, cellCount;

public:
	CParticleBitmap();
	~CParticleBitmap();

	int GetCellWidth();
	int GetCellHeight();
	int GetCellCount();
	void Resize(int cellWidth, int cellHeight, int cellCount);

	void Draw(HDC hDC, int ctrX, int ctrY, COLORREF color, double relativeAge);
	HBITMAP GetBitmap();
	HDC GetDC();
	bool GetPixel(int x, int y);
	void SetPixel(int x, int y, bool state);

	void Clear();
	void InvertAll();
	void CopyToOtherCells(int srcCell);
	void LoadDefaultBitmap();
	void ShiftAllCells(int xOffset, int yOffset);
};

