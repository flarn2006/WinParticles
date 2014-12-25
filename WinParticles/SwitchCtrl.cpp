#include "stdafx.h"
#include "SwitchCtrl.h"
#include "resource.h"

extern HINSTANCE hInst;

int CSwitchCtrl::instanceCount = 0;
HBITMAP CSwitchCtrl::bitmap = NULL;
HDC CSwitchCtrl::bitmapDC = NULL;

CSwitchCtrl::CSwitchCtrl()
{
	if (instanceCount == 0) {
		bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SWITCH));
		bitmapDC = CreateCompatibleDC(NULL);
		SelectObject(bitmapDC, bitmap);
	}
	instanceCount++;
	SetPosition(0, 0);
	boundFlag = NULL;
}

CSwitchCtrl::~CSwitchCtrl()
{
	instanceCount--;
	if (instanceCount == 0) {
		DeleteDC(bitmapDC);
		DeleteObject(bitmap);
	}
}

void CSwitchCtrl::SetPosition(int x, int y)
{
	bounds.left = x;
	bounds.top = y;
	bounds.right = x + 31;
	bounds.bottom = y + 15;
}

void CSwitchCtrl::BindTo(bool &flag)
{
	boundFlag = &flag;
}

void CSwitchCtrl::Unbind()
{
	boundFlag = NULL;
}

void CSwitchCtrl::OnDraw(HDC hDC, const LPRECT clientRect)
{
	int y;
	if (boundFlag) {
		y = *boundFlag ? 16 : 0;
	} else {
		y = 0;
	}
	BitBlt(hDC, bounds.left, bounds.top, 32, 16, bitmapDC, 0, y, SRCCOPY);
}

void CSwitchCtrl::OnMouseDown(int x, int y)
{
	if (boundFlag) {
		*boundFlag = !*boundFlag;
	}
}

bool CSwitchCtrl::OccupiesPoint(int x, int y)
{
	return PtInRect(&bounds, { x, y }) > 0;
}