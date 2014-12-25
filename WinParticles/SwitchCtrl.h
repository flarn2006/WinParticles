#pragma once
#include <Windows.h>
#include "DisplayItem.h"

class CSwitchCtrl : public CDisplayItem
{
public:
	typedef void(*Callback)(bool newValue);

private:
	static int instanceCount;
	static HBITMAP bitmap;
	static HDC bitmapDC;
	bool *boundFlag;
	RECT bounds;

public:
	CSwitchCtrl();
	~CSwitchCtrl();

	void SetPosition(int x, int y);
	void BindTo(bool &flag);
	void Unbind();

	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual bool OccupiesPoint(int x, int y);
};

