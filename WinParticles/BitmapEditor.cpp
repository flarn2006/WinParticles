#include "stdafx.h"
#include "BitmapEditor.h"
#include "resource.h"

extern HINSTANCE hInst;

CBitmapEditor::CBitmapEditor(CParticleBitmap *bitmap)
{
	this->bitmap = bitmap;
	borderPen = CreatePen(PS_SOLID, 1, 0x808080);
	sectionPen = CreatePen(PS_SOLID, 1, 0xFF8080);
	topRightPos.x = 0;
	topRightPos.y = 0;
	drawing = false;
	resizing = false;

	toolbarBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BMPTOOLBAR));
	toolbarDC = CreateCompatibleDC(NULL);
	SelectObject(toolbarDC, toolbarBmp);

	UpdateBounds();
}

CBitmapEditor::~CBitmapEditor()
{
	DeleteObject(borderPen);
	DeleteObject(sectionPen);
	DeleteDC(toolbarDC);
	DeleteObject(toolbarBmp);
}

void CBitmapEditor::UpdateBounds()
{
	bmpSize.cx = bitmap->GetCellWidth() * bitmap->GetCellCount();
	bmpSize.cy = bitmap->GetCellHeight();

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
	UpdateBounds();
	SelectObject(hDC, borderPen);
	for (int y = 0; y < bmpSize.cy; y++) {
		for (int x = 0; x < bmpSize.cx; x++) {
			SelectObject(hDC, GetStockObject(bitmap->GetPixel(x, y) ? WHITE_BRUSH : BLACK_BRUSH));
			int px = bounds.left + PIXEL_SIZE * x;
			int py = bounds.top + PIXEL_SIZE * y;
			Rectangle(hDC, px, py, px + PIXEL_SIZE + 1, py + PIXEL_SIZE + 1);
		}
	}

	SelectObject(hDC, sectionPen);
	SelectObject(hDC, GetStockObject(NULL_BRUSH));
	for (int i = 0; i < bitmap->GetCellCount(); i++) {
		int sectionWidth = PIXEL_SIZE * bitmap->GetCellWidth();
		int sectionHeight = PIXEL_SIZE * bitmap->GetCellHeight();
		Rectangle(hDC, bounds.left + sectionWidth * i, bounds.top, bounds.left + sectionWidth * (i + 1) + 1, bounds.top + sectionHeight + 1);
	}

	StretchBlt(hDC, toolbarRect.left, toolbarRect.top, TOOLBAR_BMP_WIDTH * 2, TOOLBAR_BMP_HEIGHT * 2, toolbarDC, 0, 0, TOOLBAR_BMP_WIDTH, TOOLBAR_BMP_HEIGHT, SRCCOPY);

	if (resizing) {
		Rectangle(hDC, bounds.left - 2, bounds.top - 2, bounds.right + 3, bounds.bottom + 3);
		SetDCBrushColor(hDC, 0xFFFF00);
		SelectObject(hDC, GetStockObject(DC_BRUSH));
		PatBlt(hDC, toolbarRect.left + 73, toolbarRect.top + 1, 24, 24, 0x500325);  // ROP 0x50: (PAT & !DEST)
		
		LPCTSTR resizeHelpText = L"[Arrows] Cell size\n[+/-]    # of cells";
		RECT textRect;
		textRect.left = bounds.left + 1;
		textRect.top = toolbarRect.top + 1;
		textRect.right = toolbarRect.left - 8;
		textRect.bottom = clientRect->bottom;
		
		SetTextColor(hDC, 0);
		DrawText(hDC, resizeHelpText, lstrlen(resizeHelpText), &textRect, 0);
		textRect.left--;
		textRect.top--;
		SetTextColor(hDC, 0xFFFF00);
		DrawText(hDC, resizeHelpText, lstrlen(resizeHelpText), &textRect, 0);
	}
}

void CBitmapEditor::OnMouseDown(int x, int y)
{
	if (PtInRect(&bounds, { x, y })) {
		int px = (x - bounds.left) / PIXEL_SIZE;
		int py = (y - bounds.top) / PIXEL_SIZE;
		drawing = true;
		drawingState = !bitmap->GetPixel(px, py);
		bitmap->SetPixel(px, py, drawingState);
	} else if (PtInRect(&toolbarRect, { x, y })) {
		int btnWidth = 2 * TOOLBAR_BMP_WIDTH / TOOLBAR_BUTTON_COUNT;
		int btnIndex = (x - toolbarRect.left) / btnWidth;
		switch (btnIndex) {
		case 0: //clear
			bitmap->Clear();
			break;
		case 1: //invert
			bitmap->InvertAll();
			break;
		case 2: //copy
			bitmap->CopyToOtherCells(0);
			break;
		case 3: //resize
			resizing = !resizing;
			break;
		case 4: //default
			bitmap->LoadDefaultBitmap();
			break;
		}
	}
}

void CBitmapEditor::OnMouseMove(int x, int y)
{
	if (drawing && PtInRect(&bounds, { x, y })) {
		int px = (x - bounds.left) / PIXEL_SIZE;
		int py = (y - bounds.top) / PIXEL_SIZE;
		bitmap->SetPixel(px, py, drawingState);
	}
}

void CBitmapEditor::OnMouseUp(int x, int y)
{
	drawing = false;
}

bool CBitmapEditor::OnKeyDown(UINT uCode)
{
	if (resizing) {
		int cx = bitmap->GetCellWidth();
		int cy = bitmap->GetCellHeight();
		int cz = bitmap->GetCellCount();
		switch (uCode) {
		case VK_RIGHT:
			if (--cx < 1) cx = 1;
			bitmap->Resize(cx, cy, cz);
			return true;
		case VK_LEFT:
			cx++;
			bitmap->Resize(cx, cy, cz);
			return true;
		case VK_UP:
			if (--cy < 1) cy = 1;
			bitmap->Resize(cx, cy, cz);
			return true;
		case VK_DOWN:
			cy++;
			bitmap->Resize(cx, cy, cz);
			return true;
		case VK_OEM_MINUS: case (UINT)'-':
			if (--cz < 1) cz = 1;
			bitmap->Resize(cx, cy, cz);
			return true;
		case VK_OEM_PLUS: case (UINT)'=':
			cz++;
			bitmap->Resize(cx, cy, cz);
			return true;
		default:
			return false;
		}
	} else {
		return false;
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