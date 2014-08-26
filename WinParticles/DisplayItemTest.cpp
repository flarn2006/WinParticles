#include "stdafx.h"
#include "DisplayItemTest.h"


CDisplayItemTest::CDisplayItemTest()
{
	xPos = 32;
	yPos = 32;
}

CDisplayItemTest::~CDisplayItemTest()
{
}

void CDisplayItemTest::OnDraw(HDC hDC, LPRECT clientRect)
{
	xOrigin = clientRect->right - 116;
	yOrigin = 16;

	SelectObject(hDC, GetStockObject(WHITE_PEN));
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	Rectangle(hDC, xOrigin, yOrigin, xOrigin + 100, 96);
	SetPixel(hDC, xOrigin + xPos, yOrigin + yPos, 0xFFFFFF);
}

void CDisplayItemTest::OnMouseDown(int x, int y)
{
	xPos = x - xOrigin;
	yPos = y - yOrigin;
}

bool CDisplayItemTest::OccupiesPoint(int x, int y)
{
	return (xOrigin <= x && x < xOrigin + 100 && yOrigin <= y && y < yOrigin + 80);
}