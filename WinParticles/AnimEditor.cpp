#include "stdafx.h"
#include "AnimEditor.h"
#include "WinEvents.h"
#include "RootDisplay.h"
#include "NumericInputBox.h"
#include "resource.h"

extern HINSTANCE hInst;
extern CWinEvents *winEvents;
extern CRootDisplay *display;

CAnimation<double>::AnimFunction CAnimEditor::functionList[] = { AnimFunctions::Saw, AnimFunctions::Sine, AnimFunctions::Square, AnimFunctions::Triangle, AnimFunctions::Random };
LPCTSTR CAnimEditor::functionNames[] = { TEXT("Saw"), TEXT("Sine"), TEXT("Square"), TEXT("Triangle"), TEXT("Random") };

CAnimEditor::CAnimEditor(CAnimation<double> *animations)
{
	this->animations = animations;
	SetPosition(0, 0);
	cyanPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 255));
	greenPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	resetBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RESET_ICON));
	resetBmpDC = CreateCompatibleDC(NULL);
	SelectObject(resetBmpDC, resetBmp);
	enabledSwitch.SetCallback([this](bool newValue) {
		this->animations[selectedID].SetEnabled(newValue);
	});
	freqKnob.SetCallback([this](int delta) {
		double freq = this->animations[selectedID].GetFrequency();
		freq *= std::pow(ROOT8_OF_2, (double)delta);
		this->animations[selectedID].SetFrequency(freq);
	});
	AddSubItem(&enabledSwitch);
	AddSubItem(&freqKnob);
	SetSelectedID(0);
}

CAnimEditor::~CAnimEditor()
{
	DeleteDC(resetBmpDC);
	DeleteObject(resetBmp);
	DeleteObject(cyanPen);
}

void CAnimEditor::UpdateBounds()
{
	bounds.left = topLeft.x;
	bounds.top = topLeft.y;
	bounds.right = topLeft.x + 127;
	bounds.bottom = topLeft.y + 122;

	enabledSwitch.SetPosition(bounds.left + 8, bounds.top + 8);
	freqKnob.SetPosition(bounds.left + 20, bounds.top + 100);

	resetBtnBounds.left = bounds.right - 21;
	resetBtnBounds.top = bounds.bottom - 21;
	resetBtnBounds.right = resetBtnBounds.left + 9;
	resetBtnBounds.bottom = resetBtnBounds.top + 10;
	InflateRect(&resetBtnBounds, 4, 3);
}

void CAnimEditor::SetPosition(int left, int top)
{
	topLeft.x = left;
	topLeft.y = top;
	UpdateBounds();
}

void CAnimEditor::Update()
{
	enabledSwitch.SetState(animations[selectedID].GetEnabled());

	selFuncID = 0;
	for (int i = 0; i < 5; i++) {
		if (animations[selectedID].GetFunction() == functionList[i]) {
			selFuncID = i;
			break;
		}
	}
}

void CAnimEditor::SetSelectedID(int selectedID)
{
	this->selectedID = selectedID;
	Update();
}

void CAnimEditor::GetLineRect(LPRECT rect, int lineNum)
{
	rect->left = bounds.left + 3;
	rect->top = bounds.top + 32 + lineNum * 15;
	rect->right = rect->left + 120;
	rect->bottom = rect->top + 15;
}

void CAnimEditor::OnDraw(HDC hDC, const LPRECT clientRect)
{
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	SelectObject(hDC, animations[selectedID].GetEnabled() ? greenPen : cyanPen);
	Rectangle(hDC, bounds.left, bounds.top, bounds.right, bounds.bottom);

	COLORREF color = animations[selectedID].GetEnabled() ? RGB(0, 255, 0) : RGB(0, 255, 255);
	SetTextColor(hDC, color);
	TextOut(hDC, bounds.left + 48, bounds.top + 9, enabledSwitch.GetState() ? TEXT("ENABLED ") : TEXT("DISABLED"), 8);

	TextOut(hDC, bounds.left + 7, bounds.top + 99, TEXT("-"), 1);
	TextOut(hDC, bounds.left + 83, bounds.top + 99, TEXT("+"), 1);

	tostringstream out;
	double min, max;
	animations[selectedID].GetRange(&min, &max);
	out << TEXT("Min: ") << min << std::endl;
	out << TEXT("Max: ") << max << std::endl;
	out << TEXT("Func: ") << functionNames[selFuncID] << std::endl;
	out << TEXT("Freq: ") << animations[selectedID].GetFrequency() << std::endl;

	RECT textRect;
	textRect.left = bounds.left + 7;
	textRect.top = bounds.top + 32;
	textRect.bottom = bounds.bottom;
	textRect.right = bounds.right;
	DrawText(hDC, out.str().c_str(), out.str().length(), &textRect, 0);

	BitBlt(hDC, resetBtnBounds.left + 4, resetBtnBounds.top + 3, 10, 11, resetBmpDC, 0, 0, SRCCOPY);
	if (resetBtnHighlighted) {
		SelectObject(hDC, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hDC, color);
		PatBlt(hDC, resetBtnBounds.left, resetBtnBounds.top, resetBtnBounds.right - resetBtnBounds.left + 1, resetBtnBounds.bottom - resetBtnBounds.top + 1, PATINVERT);
	}

	if (highlightLine >= 0) {
		RECT line;
		GetLineRect(&line, highlightLine);
		SelectObject(hDC, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hDC, color);
		PatBlt(hDC, line.left, line.top, line.right - line.left, line.bottom - line.top, PATINVERT);
	}

	CCompoundDispItem::OnDraw(hDC, clientRect);
}

void CAnimEditor::OnMouseDown(int x, int y)
{
	if (resetBtnHighlighted) {
		for (int i = 0; i < CParamAgent::ParamID::PARAM_COUNT; i++) {
			animations[i].Reset();
		}
	}
	double min, max;
	animations[selectedID].GetRange(&min, &max);
	CAnimation<double>::AnimFunction func = animations[selectedID].GetFunction();

	CNumericInputBox::Callback callback;
	tstring prompt;
	bool openInputBox = false;
	switch (highlightLine) {
	case 0:
		openInputBox = true;
		prompt = TEXT("Min value:");
		callback = [this, max](double value) {
			animations[selectedID].SetRange(value, max);
		};
		break;
	case 1:
		openInputBox = true;
		prompt = TEXT("Max value:");
		callback = [this, min](double value) {
			animations[selectedID].SetRange(min, value);
		};
		break;
	case 2:
		selFuncID = (selFuncID + 1) % 5;
		animations[selectedID].SetFunction(functionList[selFuncID]);
		break;
	case 3:
		openInputBox = true;
		prompt = TEXT("Frequency:");
		callback = [this](double value) {
			animations[selectedID].SetFrequency(value);
		};
		break;
	default:
		CCompoundDispItem::OnMouseDown(x, y);
	}

	if (openInputBox) {
		display->GetNumInputBox()->PromptForValue(callback, prompt);
	}
}

void CAnimEditor::OnMouseMove(int x, int y)
{
	highlightLine = -1;
	for (int i = 0; i < 4; i++) {
		RECT line;
		GetLineRect(&line, i);
		if (PtInRect(&line, { x, y })) {
			highlightLine = i;
			break;
		}
	}

	resetBtnHighlighted = PtInRect(&resetBtnBounds, { x, y }) > 0;

	if (resetBtnHighlighted) {
		display->SetHelpText(TEXT("Resets (and synchronizes) all animations."));
	} else {
		switch (highlightLine) {
		case 0: case 1: display->SetHelpText(TEXT("Left click = type in new value. Right click = set to current value. Or use mouse wheel.")); break;
		case 2: display->SetHelpText(TEXT("Left click to change the function used for animation. Right click to go back.")); break;
		case 3: display->SetHelpText(TEXT("Left click = type in new value. Right click = reset to 1 Hz. Or use mouse wheel.")); break;
		}
	}

	CCompoundDispItem::OnMouseMove(x, y);
}

void CAnimEditor::OnRightClick(int x, int y)
{
	// Fun (but irrelevant) fact: this is the first actual need I had for the Window->Split function.
	// Until now I never saw the need if you couldn't split with two separate documents, but I needed
	// to be able to see code from CAnimEditor::OnMouseDown while editing this, but didn't want to
	// keep scrolling. Then I got the idea to use that long-neglected feature! Anyway, doubt you care.
	// Now back to our regularly-scheduled source code. :)

	double min, max;
	animations[selectedID].GetRange(&min, &max);
	
	switch (highlightLine) {
	case 0:
		animations[selectedID].SetRange(animations[selectedID].GetTarget()->GetValue(), max);
		break;
	case 1:
		animations[selectedID].SetRange(min, animations[selectedID].GetTarget()->GetValue());
		break;
	case 2:
		selFuncID--;
		if (selFuncID < 0) selFuncID = 4;
		animations[selectedID].SetFunction(functionList[selFuncID]);
		break;
	case 3:
		animations[selectedID].SetFrequency(1.0);
		break;
	default:
		CCompoundDispItem::OnRightClick(x, y);
	}
}

void CAnimEditor::OnMouseLeave()
{
	highlightLine = -1;
}

bool CAnimEditor::OnMouseWheel(short wheelDelta)
{
	double delta = (double)wheelDelta / WHEEL_DELTA;
	double min, max;
	switch (highlightLine) {
	case -1:
		return false;
	case 0:
		animations[selectedID].GetRange(&min, &max);
		min += delta * winEvents->GetDeltaMult();
		animations[selectedID].SetRange(min, max);
		return true;
	case 1:
		animations[selectedID].GetRange(&min, &max);
		max += delta * winEvents->GetDeltaMult();
		animations[selectedID].SetRange(min, max);
		return true;
	case 3:
		animations[selectedID].SetFrequency(animations[selectedID].GetFrequency() + delta * winEvents->GetDeltaMult());
		return true;
	default:
		// If any line is highlighted, the mouse wheel shouldn't control the standard parameters.
		return true;
	}
}

bool CAnimEditor::OccupiesPoint(int x, int y)
{
	return PtInRect(&bounds, { x, y }) > 0;
}