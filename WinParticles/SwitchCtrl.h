#pragma once
#include <Windows.h>
#include <functional>
#include "DisplayItem.h"

class CSwitchCtrl : public CDisplayItem
{
public:
	typedef std::function<void(bool)> Callback;

private:
	static int instanceCount;
	static HBITMAP bitmap;
	static HDC bitmapDC;
	const Callback *callback;
	bool state;
	RECT bounds;

public:
	CSwitchCtrl();
	~CSwitchCtrl();

	void SetPosition(int x, int y);
	void SetCallback(const Callback &callback);
	void SetState(bool state);
	bool GetState();

	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual bool OccupiesPoint(int x, int y);
};

