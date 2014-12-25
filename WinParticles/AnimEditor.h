#pragma once
#include "CompoundDispItem.h"
#include "AnimatedParam.h"

class CAnimEditor : public CCompoundDispItem
{
private:
	CAnimatedParam *animParams;
	int selectedID;
	HBITMAP switchBmp;
	HDC switchBmpDC;
	POINT topLeft;
	RECT bounds;
	HPEN cyanPen;

	void UpdateBounds();

public:
	CAnimEditor(CAnimation<float> *animParams);
	~CAnimEditor();

	void SetPosition(int left, int top);
	
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual bool OccupiesPoint(int x, int y);
};

