#pragma once
#include "DisplayItem.h"
#include "Common.h"

class CHelpText : public CDisplayItem
{
private:
	RECT bounds;
	tstring text;

protected:
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);

public:
	CHelpText();
	void SetBottomRight(int bottom, int right);
	void SetText(const tstring &text = TEXT(""));
	const tstring &GetText();
};

