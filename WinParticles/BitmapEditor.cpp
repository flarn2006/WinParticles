#include "stdafx.h"
#include "BitmapEditor.h"
#include "resource.h"

extern HINSTANCE hInst;

CBitmapEditor::CBitmapEditor(HDC hBitmapDC, LONG width, LONG height)
{
	bitmapDC = hBitmapDC;
	borderPen = CreatePen(PS_SOLID, 1, 0x808080);
	topRightPos.x = 0;
	topRightPos.y = 0;
	bmpSize.cx = width;
	bmpSize.cy = height;

	toolbarBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BMPTOOLBAR));
	toolbarDC = CreateCompatibleDC(NULL);
	SelectObject(toolbarDC, toolbarBmp);

	UpdateBounds();
}

CBitmapEditor::~CBitmapEditor()
{
	DeleteObject(borderPen);
	DeleteDC(toolbarDC);
	DeleteObject(toolbarBmp);
}

void CBitmapEditor::UpdateBounds()
{
	bounds.right = topRightPos.x;
	bounds.top = topRightPos.y;
	bounds.left = bounds.right - PIXEL_SIZE * bmpSize.cx;
	bounds.bottom = bounds.top + PIXEL_SIZE * bmpSize.cy;
	
	toolbarRect.right = bounds.right;
	toolbarRect.left = toolbarRect.right - TOOLBAR_BMP_WIDTH * 2 + 1;
	toolbarRect.top = bounds.bottom + 8;
	toolbarRect.bottom = toolbarRect.top + TOOLBAR_BMP_HEIGHT * 2 - 1;
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
	StretchBlt(hDC, toolbarRect.left, toolbarRect.top, TOOLBAR_BMP_WIDTH * 2, TOOLBAR_BMP_HEIGHT * 2, toolbarDC, 0, 0, TOOLBAR_BMP_WIDTH, TOOLBAR_BMP_HEIGHT, SRCCOPY);
}

void CBitmapEditor::OnMouseDown(int x, int y)
{
	if (PtInRect(&bounds, { x, y })) {
		int px = (x - bounds.left) / PIXEL_SIZE;
		int py = (y - bounds.top) / PIXEL_SIZE;
		PatBlt(bitmapDC, px, py, 1, 1, DSTINVERT);
	} else if (PtInRect(&toolbarRect, { x, y })) {
		int btnWidth = 2 * TOOLBAR_BMP_WIDTH / 4;
		int btnIndex = (x - toolbarRect.left) / btnWidth;
		switch (btnIndex) {
		case 0: //clear
			PatBlt(bitmapDC, 0, 0, 30, 5, WHITENESS);
			break;
		case 1: //invert
			PatBlt(bitmapDC, 0, 0, 30, 5, DSTINVERT);
			break;
		case 2: //copy
			for (int i = 1; i <= 5; i++) {
				BitBlt(bitmapDC, 5 * i, 0, 5, 5, bitmapDC, 0, 0, SRCCOPY);
			}
			break;
		case 3: //default
			HBITMAP newBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PARTICLES));
			HGDIOBJ oldBitmap = SelectObject(bitmapDC, newBitmap);
			DeleteObject(oldBitmap);
			break;
		}
	}
}

bool CBitmapEditor::OccupiesPoint(int x, int y)
{
	POINT pt = { x, y };
	return PtInRect(&bounds, pt) || PtInRect(&toolbarRect, pt);
}

void CBitmapEditor::SetTopRightPos(LONG x, LONG y)
{
	topRightPos.x = x;
	topRightPos.y = y;
	UpdateBounds();
}