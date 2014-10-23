#pragma once
#include "DisplayItem.h"
#include "ParticleBitmap.h"

class CBitmapEditor : public CDisplayItem
{
private:
	HPEN borderPen, sectionPen;
	CParticleBitmap *bitmap;
	POINT topRightPos;
	SIZE bmpSize;
	RECT bounds;
	HBITMAP toolbarBmp;
	HDC toolbarDC;
	RECT toolbarRect;
	bool drawing;
	bool drawingState;
	bool resizing;
	int pixelSize;
	RECT lastClientRect;

	const int TOOLBAR_BMP_WIDTH = 61;
	const int TOOLBAR_BMP_HEIGHT = 13;
	const int TOOLBAR_BUTTON_COUNT = 5;

	void UpdateBounds();
	void UpdateBounds(LPRECT clientRect);

protected:
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseUp(int x, int y);
	virtual bool OnKeyDown(UINT uCode);
	virtual bool OccupiesPoint(int x, int y);

public:
	CBitmapEditor(CParticleBitmap *bitmap);
	~CBitmapEditor();
	void SetTopRightPos(LONG x, LONG y);
};

