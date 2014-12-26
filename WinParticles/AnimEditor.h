#pragma once
#include "CompoundDispItem.h"
#include "Animation.h"
#include "SwitchCtrl.h"
#include "KnobCtrl.h"

class CAnimEditor : public CCompoundDispItem
{
private:
	static CAnimation<double>::AnimFunction functionList[5];
	static LPCTSTR functionNames[5];
	CAnimation<double> *animations;
	int selectedID;
	HBITMAP switchBmp;
	HDC switchBmpDC;
	POINT topLeft;
	RECT bounds;
	HPEN cyanPen;
	HPEN greenPen;
	CSwitchCtrl enabledSwitch;
	CKnobCtrl freqKnob;
	int highlightLine = -1;
	int selFuncID = 0;

	void UpdateBounds();
	void GetLineRect(LPRECT rect, int lineNum);

public:
	CAnimEditor(CAnimation<double> *animations);
	~CAnimEditor();

	void SetPosition(int left, int top);
	void SetSelectedID(int selectedID);
	
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnRightClick(int x, int y);
	virtual bool OccupiesPoint(int x, int y);
};

