#pragma once
#include "DisplayItem.h"
#include <vector>

class CCompoundDispItem : public CDisplayItem
{
private:
	std::vector<CDisplayItem*> subItems;
	CDisplayItem *lastMouseMove;

protected:
	void AddSubItem(CDisplayItem *subItem);
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseUp(int x, int y);
	virtual bool OnKeyDown(UINT uCode);
	virtual bool OccupiesPoint(int x, int y);

public:
	CCompoundDispItem();
	~CCompoundDispItem();
};

