#include "stdafx.h"
#include "KnobCtrl.h"
#include "resource.h"

extern HINSTANCE hInst;

int CKnobCtrl::instanceCount = 0;
HBITMAP CKnobCtrl::bitmap = NULL;
HDC CKnobCtrl::bitmapDC = NULL;

CKnobCtrl::CKnobCtrl()
{
	if (instanceCount == 0) {
		bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_KNOB));
		bitmapDC = CreateCompatibleDC(NULL);
		SelectObject(bitmapDC, bitmap);
	}
	instanceCount++;
	SetPosition(0, 0);
}

CKnobCtrl::~CKnobCtrl()
{
	instanceCount--;
	if (instanceCount == 0) {
		DeleteDC(bitmapDC);
		DeleteObject(bitmap);
	}
}

void CKnobCtrl::SetPosition(int x, int y)
{
	bounds.left = x;
	bounds.top = y;
	bounds.right = x + 57;
	bounds.bottom = y + 12;
}

void CKnobCtrl::SetCallback(const Callback &callback)
{
	this->callback = callback;
}

void CKnobCtrl::OnDraw(HDC hDC, const LPRECT clientRect)
{
	BitBlt(hDC, bounds.left, bounds.top, 58, 13, bitmapDC, 0, 13 * imageIndex, SRCCOPY);
}

void CKnobCtrl::OnMouseDown(int x, int y)
{
	mouseDown = true;
	lastX = x;
}

void CKnobCtrl::OnMouseMove(int x, int y)
{
	if (mouseDown) {
		int delta = x - lastX;
		if (delta != 0) {
			callback(delta);
			imageIndex = (unsigned)(imageIndex + delta) % 2;
			lastX = x;
		}
	}
}

void CKnobCtrl::OnMouseUp(int x, int y)
{
	mouseDown = false;
}

bool CKnobCtrl::OccupiesPoint(int x, int y)
{
	return PtInRect(&bounds, { x, y }) > 0;
}