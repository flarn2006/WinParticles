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
	CParticleBitmap(const CParticleBitmap &other);
	~CParticleBitmap();
	const CParticleBitmap &operator=(const CParticleBitmap &other);

	int GetCellWidth();
	int GetCellHeight();
	int GetCellCount();
	int GetPixelCount();
	void Resize(int cellWidth, int cellHeight, int cellCount);

	void Draw(HDC hDC, int ctrX, int ctrY, COLORREF color, int cellNum);
	void Draw(HDC hDC, int ctrX, int ctrY, COLORREF color, double relativeAge);
	HBITMAP GetBitmap();
	HDC GetDC();
	bool GetPixel(int x, int y);
	void SetPixel(int x, int y, bool state);

	void Clear();
	void InvertAll();
	void CopyToOtherCells(int srcCell);
	void Scale(int factor);
	void LoadDefaultBitmap();
	void ShiftAllCells(int xOffset, int yOffset);
	void FloodFill(int x, int y);
};

