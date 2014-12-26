#include "stdafx.h"
#include "AnimEditor.h"
#include "resource.h"

extern HINSTANCE hInst;

CAnimation<double>::AnimFunction CAnimEditor::functionList[] = { AnimFunctions::Saw, AnimFunctions::Sine, AnimFunctions::Square, AnimFunctions::Triangle, AnimFunctions::Random };
LPCTSTR CAnimEditor::functionNames[] = { TEXT("Saw"), TEXT("Sine"), TEXT("Square"), TEXT("Triangle"), TEXT("Random") };

CAnimEditor::CAnimEditor(CAnimation<double> *animations)
{
	this->animations = animations;
	switchBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SWITCH));
	switchBmpDC = CreateCompatibleDC(NULL);
	SelectObject(switchBmpDC, switchBmp);
	SetPosition(0, 0);
	cyanPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 255));
	greenPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	enabledSwitch.SetCallback([this](bool newValue) {
		this->animations[selectedID].SetEnabled(newValue);
	});
	freqKnob.SetCallback([this](int delta) {
		double freq = this->animations[selectedID].GetFrequency();
		freq += 0.1 * delta;
		if (std::abs(freq) < 0.000001) freq = 0.0;
		this->animations[selectedID].SetFrequency(freq);
	});
	AddSubItem(&enabledSwitch);
	AddSubItem(&freqKnob);
	SetSelectedID(0);
}

CAnimEditor::~CAnimEditor()
{
	DeleteDC(switchBmpDC);
	DeleteObject(switchBmp);
	DeleteObject(cyanPen);
}

void CAnimEditor::UpdateBounds()
{
	bounds.left = topLeft.x;
	bounds.top = topLeft.y;
	bounds.right = topLeft.x + 127;
	bounds.bottom = topLeft.y + 122;

	enabledSwitch.SetPosition(bounds.left + 8, bounds.top + 8);
	freqKnob.SetPosition(bounds.left + 8, bounds.top + 100);
}

void CAnimEditor::SetPosition(int left, int top)
{
	topLeft.x = left;
	topLeft.y = top;
	UpdateBounds();
}

void CAnimEditor::SetSelectedID(int selectedID)
{
	this->selectedID = selectedID;
	enabledSwitch.SetState(animations[selectedID].GetEnabled());

	selFuncID = 0;
	for (int i = 0; i < 5; i++) {
		if (animations[selectedID].GetFunction() == functionList[i]) {
			selFuncID = i;
			break;
		}
	}
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
	double min, max;
	animations[selectedID].GetRange(&min, &max);

	CAnimation<double>::AnimFunction func = animations[selectedID].GetFunction();
	switch (highlightLine) {
	case 0:
		animations[selectedID].SetRange(animations[selectedID].GetTarget()->GetValue(), max);
		break;
	case 1:
		animations[selectedID].SetRange(min, animations[selectedID].GetTarget()->GetValue());
		break;
	case 2:
		selFuncID = (selFuncID + 1) % 5;
		animations[selectedID].SetFunction(functionList[selFuncID]);
		break;
	case 3:
		animations[selectedID].SetFrequency(1.0);
		break;
	default:
		CCompoundDispItem::OnMouseDown(x, y);
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

	CCompoundDispItem::OnMouseMove(x, y);
}

void CAnimEditor::OnRightClick(int x, int y)
{
	if (highlightLine == 2) {
		selFuncID--;
		if (selFuncID < 0) selFuncID = 4;
		animations[selFuncID].SetFunction(functionList[selFuncID]);
	}
}

bool CAnimEditor::OccupiesPoint(int x, int y)
{
	return PtInRect(&bounds, { x, y }) > 0;
}