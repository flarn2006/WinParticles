#include "stdafx.h"
#include "HelpText.h"

extern HFONT font;

CHelpText::CHelpText()
{
	SetText();
}

void CHelpText::OnDraw(HDC hDC, const LPRECT clientRect)
{
	UINT format = DT_SINGLELINE | DT_BOTTOM | DT_RIGHT;
	SelectObject(hDC, font);
	SetTextColor(hDC, 0);
	DrawText(hDC, text.c_str(), text.length(), &bounds, format); //drop shadow
	bounds.right--; bounds.bottom--;
	SetTextColor(hDC, 0xFFFF00);
	DrawText(hDC, text.c_str(), text.length(), &bounds, format); //main text
	bounds.right++; bounds.bottom++;
}

void CHelpText::SetBottomRight(int bottom, int right)
{
	bounds.bottom = bottom;
	bounds.top = bottom - 32;
	bounds.left = 0;
	bounds.right = right;
}

void CHelpText::SetText(const tstring &text)
{
	this->text = text;
}