#pragma once
#include <string>
#include <functional>
#include "DisplayItem.h"
#include "ParamAgent.h"
#include "Common.h"

class CNumericInputBox : public CDisplayItem
{
public:
	typedef std::function<void(double)> Callback;

private:
	CParamAgent *agent;
	CParamAgent::ParamID paramNum;
	tstring prompt;
	tstring input;
	void GetSize(LPSIZE lpSize);
	void UpdateBounds();
	LONG left, top;
	RECT bounds;
	HBRUSH blueBrush;
	HFONT font;
	Callback callback;
	bool useCallback;

protected:
	virtual void OnDraw(HDC hDC, const LPRECT lpClientRect);
	virtual bool OnKeyDown(UINT uCode);

public:
	CNumericInputBox();
	~CNumericInputBox();
	
	void PromptForValue(CParamAgent *paramAgent, const tstring &prompt = TEXT("New value:"));
	void PromptForValue(Callback &callback, const tstring &prompt = TEXT("New value:"));
	void AcceptValue();
	void CancelPrompt();
	void SetPosition(LONG left, LONG top);
};

