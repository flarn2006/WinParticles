#include "stdafx.h"
#include "AnimEditor.h"
#include "resource.h"

extern HINSTANCE hInst;

CAnimEditor::CAnimEditor(CAnimation<double> *animations)
{
	this->animations = animations;
	switchBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SWITCH));
	switchBmpDC = CreateCompatibleDC(NULL);
	SelectObject(switchBmpDC, switchBmp);
	SetPosition(0, 0);
	cyanPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 255));
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
	SelectObject(hDC, cyanPen);
	Rectangle(hDC, bounds.left, bounds.top, bounds.right, bounds.bottom);

	SetTextColor(hDC, RGB(0, 255, 255));
	TextOut(hDC, bounds.left + 48, bounds.top + 9, enabledSwitch.GetState() ? TEXT("ENABLED ") : TEXT("DISABLED"), 8);

	LPCTSTR funcName;
	CAnimation<double>::AnimFunction func = animations[selectedID].GetFunction();
	if (func == AnimFunctions::Saw) funcName = TEXT("Saw");
	else if (func == AnimFunctions::Sine) funcName = TEXT("Sine");
	else if (func == AnimFunctions::Square) funcName = TEXT("Square");
	else if (func == AnimFunctions::Triangle) funcName = TEXT("Triangle");
	else if (func == AnimFunctions::Random) funcName = TEXT("Random");

	tostringstream out;
	double min, max;
	animations[selectedID].GetRange(&min, &max);
	out << TEXT("Min: ") << min << std::endl;
	out << TEXT("Max: ") << max << std::endl;
	out << TEXT("Func: ") << funcName << std::endl;
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
		SetDCBrushColor(hDC, RGB(0, 255, 255));
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
		animations[selectedID].SetRange(**animations[selectedID].GetTarget(), max);
		break;
	case 1:
		animations[selectedID].SetRange(min, **animations[selectedID].GetTarget());
		break;
	case 2:
		if (func == AnimFunctions::Saw) func = AnimFunctions::Sine;
		else if (func == AnimFunctions::Sine) func = AnimFunctions::Square;
		else if (func == AnimFunctions::Square) func = AnimFunctions::Triangle;
		else if (func == AnimFunctions::Triangle) func = AnimFunctions::Random;
		else if (func == AnimFunctions::Random) func = AnimFunctions::Saw;
		animations[selectedID].SetFunction(func);
		break;
	default:
		CCompoundDispItem::OnMouseDown(x, y);
	}
}

void CAnimEditor::OnMouseMove(int x, int y)
{
	RECT line0, line1, line2;
	GetLineRect(&line0, 0);
	GetLineRect(&line1, 1);
	GetLineRect(&line2, 2);

	if (PtInRect(&line0, { x, y })) highlightLine = 0;
	else if (PtInRect(&line1, { x, y })) highlightLine = 1;
	else if (PtInRect(&line2, { x, y })) highlightLine = 2;
	else highlightLine = -1;

	CCompoundDispItem::OnMouseMove(x, y);
}

bool CAnimEditor::OccupiesPoint(int x, int y)
{
	return PtInRect(&bounds, { x, y }) > 0;
}