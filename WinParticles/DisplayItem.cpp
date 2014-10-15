#include "stdafx.h"
#include "DisplayItem.h"

CDisplayItem::CDisplayItem()
{
	enabled = true;
	mouseLastSeenInside = false;
	mouseIsDown = false;
}

CDisplayItem::~CDisplayItem()
{
}

void CDisplayItem::OnDraw(HDC hDC, const LPRECT clientRect)
{
}

void CDisplayItem::OnMouseDown(int x, int y)
{
}

void CDisplayItem::OnMouseMove(int x, int y)
{
}

void CDisplayItem::OnMouseUp(int x, int y)
{
}

void CDisplayItem::OnMouseLeave()
{
}

bool CDisplayItem::OnKeyDown(UINT uCode)
{
	return false;
}

bool CDisplayItem::OccupiesPoint(int x, int y)
{
	return false;
}

bool CDisplayItem::GetEnabled()
{
	return enabled;
}

void CDisplayItem::SetEnabled(bool enabled)
{
	this->enabled = enabled;
}

void CDisplayItem::Draw(HDC hDC, const LPRECT clientRect)
{
	if (enabled) OnDraw(hDC, clientRect);
}

void CDisplayItem::MouseDown(int x, int y)
{
	if (OccupyingPoint(x, y)) {
		OnMouseDown(x, y);
		mouseIsDown = true;
	}
}

void CDisplayItem::MouseMove(int x, int y)
{
	bool mouseInside = OccupyingPoint(x, y);
	if (mouseInside) {
		OnMouseMove(x, y);
	} else if (mouseLastSeenInside) {
		OnMouseLeave();
	}
	mouseLastSeenInside = mouseInside;
}

void CDisplayItem::MouseUp(int x, int y)
{
	if (OccupyingPoint(x, y) || mouseIsDown) OnMouseUp(x, y);
	mouseIsDown = false;
}

bool CDisplayItem::KeyDown(UINT uCode)
{
	if (enabled)
		return OnKeyDown(uCode);
	else
		return false;
}

bool CDisplayItem::OccupyingPoint(int x, int y)
{
	if (enabled)
		return OccupiesPoint(x, y);
	else
		return false;
}