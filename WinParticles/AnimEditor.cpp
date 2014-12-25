#include "stdafx.h"
#include "AnimEditor.h"
#include "resource.h"

extern HINSTANCE hInst;

CAnimEditor::CAnimEditor(CAnimation<double> *animations)
{
	this->animations = animations;
	switchBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SWITCH));
	switchBmpDC = CreateCompatibleDC(NULL);
	SelectObject(switchBmpDC, switchBmp);
	SetPosition(0, 0);
	cyanPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
	enabledSwitch.SetCallback([this](bool newValue) { this->animations[selectedID].SetEnabled(newValue); });
	AddSubItem(&enabledSwitch);
}

CAnimEditor::~CAnimEditor()
{
	DeleteDC(switchBmpDC);
	DeleteObject(switchBmp);
	DeleteObject(cyanPen);
}

void CAnimEditor::UpdateBounds()
{
	bounds.left = topLeft.x;
	bounds.top = topLeft.y;
	bounds.right = topLeft.x + 256;
	bounds.bottom = topLeft.y + 128;

	enabledSwitch.SetPosition(bounds.left + 8, bounds.top + 8);
}

void CAnimEditor::SetPosition(int left, int top)
{
	topLeft.x = left;
	topLeft.y = top;
	UpdateBounds();
}

void CAnimEditor::SetSelectedID(int selectedID)
{
	this->selectedID = selectedID;
}

void CAnimEditor::OnDraw(HDC hDC, const LPRECT clientRect)
{
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	SelectObject(hDC, cyanPen);
	Rectangle(hDC, bounds.left, bounds.top, bounds.right, bounds.bottom);
	CCompoundDispItem::OnDraw(hDC, clientRect);
}

void CAnimEditor::OnMouseDown(int x, int y)
{
	CCompoundDispItem::OnMouseDown(x, y);
}

bool CAnimEditor::OccupiesPoint(int x, int y)
{
	return PtInRect(&bounds, { x, y }) > 0;
}