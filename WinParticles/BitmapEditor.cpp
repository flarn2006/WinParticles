#include "stdafx.h"
#include "BitmapEditor.h"
#include "resource.h"
#include "RootDisplay.h"

extern HINSTANCE hInst;
extern HFONT font;
extern int verbosity;
extern CRootDisplay *display;

CBitmapEditor::CBitmapEditor(CParticleBitmap *bitmap)
{
	this->bitmap = bitmap;
	borderPen = CreatePen(PS_SOLID, 1, 0x808080);
	borderPenLight = CreatePen(PS_SOLID, 1, 0x404040);
	sectionPen = CreatePen(PS_SOLID, 1, 0xFF8080);
	topRightPos.x = 0;
	topRightPos.y = 0;
	drawing = false;
	resizing = false;
	lastClientRect = { 0, 0, 0, 0 };

	toolbarBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BMPTOOLBAR));
	toolbarDC = CreateCompatibleDC(NULL);
	SelectObject(toolbarDC, toolbarBmp);

	const BYTE data[] = {
		0x7F, 0xFF, //01111111 11111111
		0xBF, 0xFF, //10111111 11111111
	};
	grayedPattern = CreateBitmap(2, 2, 1, 1, data);
	grayedBrush = CreatePatternBrush(grayedPattern);
}

CBitmapEditor::~CBitmapEditor()
{
	DeleteObject(borderPen);
	DeleteObject(borderPenLight);
	DeleteObject(sectionPen);
	DeleteDC(toolbarDC);
	DeleteObject(toolbarBmp);
	DeleteObject(grayedBrush);
	DeleteObject(grayedPattern);
}

void CBitmapEditor::UpdateBounds()
{
	UpdateBounds(&lastClientRect);
}

void CBitmapEditor::UpdateBounds(LPRECT clientRect)
{
	bmpSize.cx = bitmap->GetCellWidth() * bitmap->GetCellCount();
	bmpSize.cy = bitmap->GetCellHeight();

	bounds.right = topRightPos.x;
	bounds.top = topRightPos.y;

	pixelSize = 17;
	do {
		pixelSize--;
		bounds.left = bounds.right - pixelSize * bmpSize.cx;
		bounds.bottom = bounds.top + pixelSize * bmpSize.cy;
	} while ((bounds.left < clientRect->left + ((verbosity == 0) ? 16 : 500) || bounds.bottom > clientRect->bottom - 300) && pixelSize > 2);
	
	toolbarRect.right = bounds.right;
	toolbarRect.left = toolbarRect.right - TOOLBAR_BMP_WIDTH * 2 + 1;
	toolbarRect.top = bounds.bottom + 8;
	toolbarRect.bottom = toolbarRect.top + TOOLBAR_BMP_HEIGHT * 2 - 1;

	CopyRect(&lastClientRect, clientRect);
}

bool CBitmapEditor::CanScaleDown()
{
	// Bitmap cell width and height must both be even numbers greater than 1.
	int cellWidth = bitmap->GetCellWidth();
	int cellHeight = bitmap->GetCellHeight();
	return (cellWidth > 1) && (cellHeight > 1) && !(cellWidth & 1) && !(cellHeight & 1);
}

bool CBitmapEditor::CanScaleUp()
{
	return (bitmap->GetCellWidth() < 128) && (bitmap->GetCellHeight() < 128);
}

void CBitmapEditor::GetButtonTopLeft(int btnIndex, LPPOINT topLeft)
{
	topLeft->x = toolbarRect.left + 1 + btnIndex * 24;
	topLeft->y = toolbarRect.top + 1;
}

void CBitmapEditor::OnDraw(HDC hDC, const LPRECT clientRect)
{
	UpdateBounds(clientRect);

	// Draw the bitmap
	SetBkColor(hDC, 0);
	SetTextColor(hDC, 0xFFFFFF);
	StretchBlt(hDC, bounds.left, bounds.top, bmpSize.cx * pixelSize, bmpSize.cy * pixelSize, bitmap->GetDC(), 0, 0, bmpSize.cx, bmpSize.cy, SRCCOPY);
	
	// Draw the middle grid lines (but only if the pixels are big enough)
	if (pixelSize > 2) {
		SelectObject(hDC, (pixelSize > 4) ? borderPen : borderPenLight);
		for (int x = 1; x < bmpSize.cx; x++) {
			MoveToEx(hDC, bounds.left + pixelSize * x, bounds.top, NULL);
			LineTo(hDC, bounds.left + pixelSize * x, bounds.bottom);
		}
		for (int y = 1; y < bmpSize.cy; y++) {
			MoveToEx(hDC, bounds.left, bounds.top + pixelSize * y, NULL);
			LineTo(hDC, bounds.right, bounds.top + pixelSize * y);
		}
	}

	// Draw the rectangles for each cell
	SelectObject(hDC, sectionPen);
	SelectObject(hDC, GetStockObject(NULL_BRUSH));
	for (int i = 0; i < bitmap->GetCellCount(); i++) {
		int sectionWidth = pixelSize * bitmap->GetCellWidth();
		int sectionHeight = pixelSize * bitmap->GetCellHeight();
		Rectangle(hDC, bounds.left + sectionWidth * i, bounds.top, bounds.left + sectionWidth * (i + 1) + 1, bounds.top + sectionHeight + 1);
	}

	// Draw the toolbar
	StretchBlt(hDC, toolbarRect.left, toolbarRect.top, TOOLBAR_BMP_WIDTH * 2, TOOLBAR_BMP_HEIGHT * 2, toolbarDC, 0, 0, TOOLBAR_BMP_WIDTH, TOOLBAR_BMP_HEIGHT, SRCCOPY);

	SelectObject(hDC, grayedBrush);
	SetTextColor(hDC, 0x808080);

	if (!CanScaleDown()) {
		POINT btnTopLeft;
		GetButtonTopLeft(4, &btnTopLeft);
		PatBlt(hDC, btnTopLeft.x + 1, btnTopLeft.y + 1, 22, 22, PATINVERT);
	}

	if (!CanScaleUp()) {
		POINT btnTopLeft;
		GetButtonTopLeft(5, &btnTopLeft);
		PatBlt(hDC, btnTopLeft.x + 1, btnTopLeft.y + 1, 22, 22, PATINVERT);
	}

	if (resizing) {
		SelectObject(hDC, GetStockObject(NULL_BRUSH));
		Rectangle(hDC, bounds.left - 2, bounds.top - 2, bounds.right + 3, bounds.bottom + 3);
		SetDCBrushColor(hDC, 0xFFFF00);
		SelectObject(hDC, GetStockObject(DC_BRUSH));
		POINT resizeBtnTopLeft;
		GetButtonTopLeft(3, &resizeBtnTopLeft);
		PatBlt(hDC, resizeBtnTopLeft.x, resizeBtnTopLeft.y, 24, 24, 0x500325);  // ROP 0x50: (PAT & !DEST)
		
		LPCTSTR resizeHelpText = TEXT("[WASD] Cell size\n[+/-]  # of cells\n[IJKL] Shift image");
		RECT textRect;
		textRect.left = bounds.left + 1;
		textRect.top = toolbarRect.top + 1;
		textRect.right = toolbarRect.left - 8;
		textRect.bottom = clientRect->bottom;

		SelectObject(hDC, font);
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
		int px = (x - bounds.left) / pixelSize;
		int py = (y - bounds.top) / pixelSize;
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
		case 4: //scale 1/2x
			if (CanScaleDown()) bitmap->Scale(-2);
			break;
		case 5: //scale 2x
			if (CanScaleUp()) bitmap->Scale(2);
			break;
		case 6: //default
			bitmap->LoadDefaultBitmap();
			break;
		}
	}
}

void CBitmapEditor::OnMouseMove(int x, int y)
{
	if (drawing && PtInRect(&bounds, { x, y })) {
		int px = (x - bounds.left) / pixelSize;
		int py = (y - bounds.top) / pixelSize;
		bitmap->SetPixel(px, py, drawingState);
	} else {
		tstring text;
		for (int i = 0; i < TOOLBAR_BUTTON_COUNT; i++) {
			POINT topLeft;
			RECT btnRect;
			GetButtonTopLeft(i, &topLeft);
			btnRect.left = topLeft.x;
			btnRect.top = topLeft.y;
			btnRect.right = btnRect.left + 23;
			btnRect.bottom = btnRect.top + 23;
			if (PtInRect(&btnRect, { x, y })) {
				switch (i) {
				case 0: text = TEXT("Erases the entire bitmap."); break;
				case 1: text = TEXT("Inverts the entire bitmap."); break;
				case 2: text = TEXT("Copies the first sub-image to all others."); break;
				case 3: text = resizing ? TEXT("Click when done resizing.") : TEXT("Click to edit the dimensions of the image."); break;
				case 4: text = TEXT("Scales the image down by a factor of 1/2."); break;
				case 5: text = TEXT("Scales the image up by a factor of 2."); break;
				case 6: text = TEXT("Click to restore the default bitmap."); break;
				}
				display->SetHelpText(text);
			}
		}
	}
}

void CBitmapEditor::OnMouseUp(int x, int y)
{
	drawing = false;
}

void CBitmapEditor::OnRightClick(int x, int y)
{
	int px = (x - bounds.left) / pixelSize;
	int py = (y - bounds.top) / pixelSize;
	bitmap->FloodFill(px, py);
}

bool CBitmapEditor::OnKeyDown(UINT uCode)
{
	if (resizing) {
		int cx = bitmap->GetCellWidth();
		int cy = bitmap->GetCellHeight();
		int cz = bitmap->GetCellCount();
		switch (uCode) {
		case VK_RIGHT: case (UINT)'D':
			if (--cx < 1) cx = 1;
			bitmap->Resize(cx, cy, cz);
			return true;
		case VK_LEFT: case (UINT)'A':
			cx++;
			bitmap->Resize(cx, cy, cz);
			return true;
		case VK_UP: case (UINT)'W':
			if (--cy < 1) cy = 1;
			bitmap->Resize(cx, cy, cz);
			return true;
		case VK_DOWN: case (UINT)'S':
			cy++;
			bitmap->Resize(cx, cy, cz);
			return true;
		case (UINT)'I':
			bitmap->ShiftAllCells(0, -1);
			return true;
		case (UINT)'J':
			bitmap->ShiftAllCells(-1, 0);
			return true;
		case (UINT)'K':
			bitmap->ShiftAllCells(0, 1);
			return true;
		case (UINT)'L':
			bitmap->ShiftAllCells(1, 0);
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