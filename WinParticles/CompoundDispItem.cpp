#include "stdafx.h"
#include "CompoundDispItem.h"


CCompoundDispItem::CCompoundDispItem()
{
	lastMouseMove = NULL;
}

CCompoundDispItem::~CCompoundDispItem()
{
}

void CCompoundDispItem::AddSubItem(CDisplayItem *subItem)
{
	subItems.push_back(subItem);
}

void CCompoundDispItem::StopHandlingSubItemEvents()
{
	stopHandling = true;
}

void CCompoundDispItem::OnDraw(HDC hDC, const LPRECT clientRect)
{
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		(*i)->Draw(hDC, clientRect);
	}
}

void CCompoundDispItem::OnMouseDown(int x, int y)
{
	stopHandling = false;
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		(*i)->MouseDown(x, y);
		if (stopHandling) break;
	}
}

void CCompoundDispItem::OnMouseMove(int x, int y)
{
	stopHandling = false;
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		(*i)->MouseMove(x, y);
		if (stopHandling) break;
	}
}

void CCompoundDispItem::OnMouseUp(int x, int y)
{
	stopHandling = false;
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		(*i)->MouseUp(x, y);
		if (stopHandling) break;
	}
}

void CCompoundDispItem::OnRightClick(int x, int y)
{
	stopHandling = false;
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		(*i)->RightClick(x, y);
		if (stopHandling) break;
	}
}

bool CCompoundDispItem::OnKeyDown(UINT uCode)
{
	stopHandling = false;
	bool handledByAny = false;
	for (std::vector<CDisplayItem*>::iterator i = subItems.begin(); i != subItems.end(); i++) {
		handledByAny = handledByAny || (*i)->KeyDown(uCode);
		if (stopHandling) break;
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