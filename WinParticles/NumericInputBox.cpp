#include "stdafx.h"
#include "NumericInputBox.h"
#include <sstream>

extern HFONT font;

CNumericInputBox::CNumericInputBox()
{
	SetEnabled(false);
	SetPosition(0, 0);
	blueBrush = CreateSolidBrush(0xFF0000);
}

CNumericInputBox::~CNumericInputBox()
{
	DeleteObject(blueBrush);
}

void CNumericInputBox::PromptForValue(CParamAgent *paramAgent)
{
	input.clear();
	agent = paramAgent;
	paramNum = agent->GetSelParam();
	UpdateBounds();
	SetEnabled(true);
}

void CNumericInputBox::GetSize(const LPSIZE size)
{
	size->cx = max(100, 8 * ((LONG)input.length() + 2));
	size->cy = 16;
}

void CNumericInputBox::UpdateBounds()
{
	SIZE size; GetSize(&size);
	bounds.left = left;
	bounds.top = top;
	bounds.right = left + size.cx;
	bounds.bottom = top + size.cy;
	
	LONG dx = size.cx / 2;
	LONG dy = size.cy / 2;
	bounds.left -= dx;
	bounds.top -= dy;
	bounds.right -= dx;
	bounds.bottom -= dy;
}

void CNumericInputBox::AcceptValue()
{
	if (input.empty()) {
		CancelPrompt();
	} else {
		double value;
		tistringstream parse(input);
		parse >> value;
		if (!parse.fail()) {
			agent->SetValue(paramNum, value);
			SetEnabled(false);
		}
	}
}

void CNumericInputBox::CancelPrompt()
{
	SetEnabled(false);
}

void CNumericInputBox::SetPosition(LONG left, LONG top)
{
	this->left = left;
	this->top = top;
	UpdateBounds();
}

void CNumericInputBox::OnDraw(HDC hDC, LPRECT lpClientRect)
{
	tostringstream out;
	out << input << '_';
	
	FillRect(hDC, &bounds, blueBrush);

	SetTextColor(hDC, 0x000000);
	TextOut(hDC, bounds.left + 1, bounds.top - 15, TEXT("New value:"), 10);

	SetTextColor(hDC, 0xFFFFFF);
	TextOut(hDC, bounds.left, bounds.top - 16, TEXT("New value:"), 10);

	if (font != NULL) SelectObject(hDC, font);
	TextOut(hDC, 4 + bounds.left, bounds.top, out.str().c_str(), (int)out.str().length());
}

bool CNumericInputBox::OnKeyDown(UINT uCode)
{
	bool result = false;
	tchar_t ch[2];
	ch[0] = (tchar_t)LOWORD(MapVirtualKey(uCode, MAPVK_VK_TO_CHAR));
	ch[1] = 0;
	if (uCode == VK_BACK) {
		if (!input.empty()) input.pop_back();
		result = true;
	} else if (uCode == VK_RETURN) {
		AcceptValue();
		result = true;
	} else if (uCode == VK_ESCAPE) {
		SetEnabled(false);
		result = true;
	} else if (ch[0] != 0) {
		input.append(ch);
		result = true;
	}

	if (result && GetEnabled()) UpdateBounds();
	return result;
}