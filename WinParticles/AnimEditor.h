#pragma once
#include "CompoundDispItem.h"
#include "Animation.h"

class CAnimEditor : public CCompoundDispItem
{
private:
	CAnimation<double> *animations;
	int selectedID;
	HBITMAP switchBmp;
	HDC switchBmpDC;
	POINT topLeft;
	RECT bounds;
	HPEN cyanPen;

	void UpdateBounds();

public:
	CAnimEditor(CAnimation<double> *animations);
	~CAnimEditor();

	void SetPosition(int left, int top);
	
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual bool OccupiesPoint(int x, int y);
};

