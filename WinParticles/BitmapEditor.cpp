#include "stdafx.h"
#include "BitmapEditor.h"


CBitmapEditor::CBitmapEditor(HDC hBitmapDC, LONG width, LONG height)
{
	bitmapDC = hBitmapDC;
	borderPen = CreatePen(PS_SOLID, 1, 0x808080);
	topRightPos.x = 0;
	topRightPos.y = 0;
	bmpSize.cx = width;
	bmpSize.cy = height;
	UpdateBounds();
}

CBitmapEditor::~CBitmapEditor()
{
	DeleteObject(borderPen);
}

void CBitmapEditor::UpdateBounds()
{
	bounds.right = topRightPos.x;
	bounds.top = topRightPos.y;
	bounds.left = bounds.right - PIXEL_SIZE * bmpSize.cx;
	bounds.bottom = bounds.top + PIXEL_SIZE * bmpSize.cy;
}

void CBitmapEditor::OnDraw(HDC hDC, const LPRECT clientRect)
{
	SelectObject(hDC, borderPen);
	for (int y = 0; y < bmpSize.cy; y++) {
		for (int x = 0; x < bmpSize.cx; x++) {
			COLORREF pixel = GetPixel(bitmapDC, x, y);
			SelectObject(hDC, GetStockObject(pixel ? BLACK_BRUSH : WHITE_BRUSH));
			int px = bounds.left + PIXEL_SIZE * x;
			int py = bounds.top + PIXEL_SIZE * y;
			Rectangle(hDC, px, py, px + PIXEL_SIZE + 1, py + PIXEL_SIZE + 1);
		}
	}
}

void CBitmapEditor::OnMouseDown(int x, int y)
{
	int px = (x - bounds.left) / PIXEL_SIZE;
	int py = (y - bounds.top) / PIXEL_SIZE;
	PatBlt(bitmapDC, px, py, 1, 1, DSTINVERT);
}

bool CBitmapEditor::OccupiesPoint(int x, int y)
{
	return PtInRect(&bounds, { x, y });
}

void CBitmapEditor::SetTopRightPos(LONG x, LONG y)
{
	topRightPos.x = x;
	topRightPos.y = y;
	UpdateBounds();
}