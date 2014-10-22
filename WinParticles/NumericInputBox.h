#pragma once
#include <string>
#include "DisplayItem.h"
#include "ParamAgent.h"

class CNumericInputBox : public CDisplayItem
{
private:
	CParamAgent *agent;
	int paramNum;
	std::wstring input;
	void GetSize(LPSIZE lpSize);
	void UpdateBounds();
	LONG left, top;
	RECT bounds;
	HBRUSH blueBrush;
	HFONT font;

protected:
	virtual void OnDraw(HDC hDC, const LPRECT lpClientRect);
	virtual bool OnKeyDown(UINT uCode);

public:
	CNumericInputBox();
	~CNumericInputBox();
	
	void PromptForValue(CParamAgent *paramAgent);
	void AcceptValue();
	void CancelPrompt();
	void SetPosition(LONG left, LONG top);
};

