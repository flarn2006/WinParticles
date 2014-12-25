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
	SetState(false);
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
	bounds.right = x + 33;
	bounds.bottom = y + 17;
}

void CSwitchCtrl::SetCallback(const Callback &callback)
{
	this->callback = callback;
}

void CSwitchCtrl::SetState(bool state)
{
	this->state = state;
}

bool CSwitchCtrl::GetState()
{
	return state;
}

void CSwitchCtrl::OnDraw(HDC hDC, const LPRECT clientRect)
{
	BitBlt(hDC, bounds.left, bounds.top, 34, 18, bitmapDC, 0, state ? 18 : 0, SRCCOPY);
}

void CSwitchCtrl::OnMouseDown(int x, int y)
{
	state = !state;
	callback(state);
}

bool CSwitchCtrl::OccupiesPoint(int x, int y)
{
	return PtInRect(&bounds, { x, y }) > 0;
}