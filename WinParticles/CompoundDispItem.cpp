#include "stdafx.h"
#include "CompoundDispItem.h"


CCompoundDispItem::CCompoundDispItem()
{
}

CCompoundDispItem::~CCompoundDispItem()
{
}

void CCompoundDispItem::AddSubItem(CDisplayItem *subItem)
{
	subItems.push_back(subItem);
}

void CCompoundDispItem::OnDraw(HDC hDC, const LPRECT clientRect)
{
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		(*i)->Draw(hDC, clientRect);
	}
}

void CCompoundDispItem::OnMouseDown(int x, int y)
{
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		(*i)->MouseDown(x, y);
	}
}

bool CCompoundDispItem::OnKeyDown(UINT uCode)
{
	bool handledByAny = false;
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		handledByAny = handledByAny || (*i)->KeyDown(uCode);
	}
	return handledByAny;
}

bool CCompoundDispItem::OccupiesPoint(int x, int y)
{
	bool result = false;
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		result = (*i)->OccupyingPoint(x, y);
		if (result) break;
	}
	return result;
}