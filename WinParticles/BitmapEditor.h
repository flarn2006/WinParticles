#pragma once
#include "DisplayItem.h"

class CBitmapEditor : public CDisplayItem
{
private:
	HPEN borderPen;
	HDC bitmapDC;
	POINT topRightPos;
	SIZE bmpSize;
	RECT bounds;

	const int PIXEL_SIZE = 8;
	void UpdateBounds();

protected:
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual bool OccupiesPoint(int x, int y);

public:
	CBitmapEditor(HDC hBitmapDC, LONG width, LONG height);
	~CBitmapEditor();
	void SetTopRightPos(LONG x, LONG y);
};

