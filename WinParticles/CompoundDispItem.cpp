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

bool CCompoundDispItem::GetStopHandlingFlag()
{
	return stopHandling;
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
	for (int i = subItems.size() - 1; i >= 0; i--) {
		subItems[i]->MouseDown(x, y);
		if (stopHandling) break;
	}
}

void CCompoundDispItem::OnMouseMove(int x, int y)
{
	stopHandling = false;
	for (int i = subItems.size() - 1; i >= 0; i--) {
		subItems[i]->MouseMove(x, y);
		if (stopHandling) break;
	}
}

void CCompoundDispItem::OnMouseUp(int x, int y)
{
	stopHandling = false;
	for (int i = subItems.size() - 1; i >= 0; i--) {
		subItems[i]->MouseUp(x, y);
		if (stopHandling) break;
	}
}

void CCompoundDispItem::OnRightClick(int x, int y)
{
	stopHandling = false;
	for (int i = subItems.size() - 1; i >= 0; i--) {
		subItems[i]->RightClick(x, y);
		if (stopHandling) break;
	}
}

bool CCompoundDispItem::OnMouseWheel(short wheelDelta)
{
	stopHandling = false;
	bool handledByAny = false;
	for (int i = subItems.size() - 1; i >= 0; i--) {
		handledByAny = handledByAny || subItems[i]->MouseWheel(wheelDelta);
		if (stopHandling) break;
	}
	return handledByAny;
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