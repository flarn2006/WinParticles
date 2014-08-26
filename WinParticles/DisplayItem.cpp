#include "stdafx.h"
#include "DisplayItem.h"

CDisplayItem::CDisplayItem()
{
	enabled = true;
}


CDisplayItem::~CDisplayItem()
{
}

void CDisplayItem::OnDraw(HDC hDC, LPRECT clientRect)
{
}

void CDisplayItem::OnMouseDown(int x, int y)
{
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

void CDisplayItem::Draw(HDC hDC, LPRECT clientRect)
{
	if (enabled) OnDraw(hDC, clientRect);
}

void CDisplayItem::MouseDown(int x, int y)
{
	if (OccupyingPoint(x, y)) OnMouseDown(x, y);
}

bool CDisplayItem::OccupyingPoint(int x, int y)
{
	if (enabled)
		return OccupiesPoint(x, y);
	else
		return false;
}