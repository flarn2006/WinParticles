#pragma once
#include "DisplayItem.h"
class CDisplayItemTest : public CDisplayItem
{
private:
	int xPos, yPos;
	int xOrigin, yOrigin;

public:
	CDisplayItemTest();
	~CDisplayItemTest();

	void OnDraw(HDC hDC, LPRECT lpClientRect);
	void OnMouseDown(int x, int y);
	bool OccupiesPoint(int x, int y);
};

