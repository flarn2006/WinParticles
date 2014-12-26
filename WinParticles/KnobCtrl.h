#pragma once
#include <Windows.h>
#include <functional>
#include "DisplayItem.h"

class CKnobCtrl : public CDisplayItem
{
public:
	typedef std::function<void(int)> Callback;

private:
	static int instanceCount;
	static HBITMAP bitmap;
	static HDC bitmapDC;
	Callback callback;
	RECT bounds;
	int imageIndex = 0;
	bool mouseDown = false;
	int lastX;

public:
	CKnobCtrl();
	~CKnobCtrl();

	void SetPosition(int x, int y);
	void SetCallback(const Callback &callback);
	
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseUp(int x, int y);
	virtual bool OccupiesPoint(int x, int y);
};

