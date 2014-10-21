#include "stdafx.h"
#include "GradientEditor.h"
#include "Resource.h"
#include "Common.h"

extern HINSTANCE hInst;
extern CHOOSECOLOR colorDlg;

CGradientEditor::CGradientEditor(CGradient *gradient)
{
	clientRect = NULL;
	tint = 0xFFFFFF;
	SetGradient(gradient);
	shBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_STEPHANDLE));
	shBmpMask = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_STEPHANDLE_MASK));
	shBmpColorMask = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_STEPHANDLE_COLOR_MASK));
	shBitmapDC = CreateCompatibleDC(NULL);
	SelectObject(shBitmapDC, shBitmap);
}

CGradientEditor::~CGradientEditor()
{
	DeleteDC(shBitmapDC);
	DeleteObject(shBitmap);
	DeleteObject(shBmpMask);
	DeleteObject(shBmpColorMask);

	for (std::vector<CStepHandle*>::iterator i = stepHandles.begin(); i != stepHandles.end(); i++) {
		delete *i;
	}
}

void CGradientEditor::SetTint(COLORREF tint)
{
	this->tint = tint;
}

void CGradientEditor::OnDraw(HDC hDC, const LPRECT clientRect)
{
	this->clientRect = clientRect;

	for (int x = clientRect->left; x < clientRect->right; x++) {
		double point = Interpolate(x, clientRect->left, clientRect->right, 0.0, 1.0);
		SelectObject(hDC, GetStockObject(DC_PEN));
		SetDCPenColor(hDC, MultiplyColors(tint, gradient->ColorAtPoint(point)));
		MoveToEx(hDC, x, clientRect->bottom - 24, NULL);
		LineTo(hDC, x, clientRect->bottom);
	}

	for (std::vector<CStepHandle*>::iterator i = stepHandles.begin(); i != stepHandles.end(); i++) {
		(*i)->SetPositioningInfo(clientRect->left, clientRect->right, clientRect->bottom - 24);
	}

	CCompoundDispItem::OnDraw(hDC, clientRect);
}

CGradientEditor::CStepHandle *CGradientEditor::AddGradientStep(int x)
{
	double pos = Interpolate(x, clientRect->left, clientRect->right, 0.0, 1.0);
	unsigned int index = gradient->AddStep(pos, gradient->ColorAtPoint(pos));
	gradient->PrecalculateColors();
	SetGradient(gradient);
	return stepHandles[index];
}

void CGradientEditor::OnMouseDown(int x, int y)
{
	CCompoundDispItem::OnMouseDown(x, y);
	if (!GetStopHandlingFlag()) {
		AddGradientStep(x)->StartDragging(x);
	}
}

void CGradientEditor::OnRightClick(int x, int y)
{
	CCompoundDispItem::OnRightClick(x, y);
	if (!GetStopHandlingFlag()) {
		AddGradientStep(x);
	}
}

bool CGradientEditor::OccupiesPoint(int x, int y)
{
	if (CCompoundDispItem::OccupiesPoint(x, y)) {
		return true;
	} else if (clientRect) {
		return (y >= clientRect->bottom - 24);
	} else {
		return false;
	}
}

void CGradientEditor::SetGradient(CGradient *gradient)
{
	this->gradient = gradient;
	
	if ((unsigned)gradient->GetStepCount() > stepHandles.size()) {
		for (unsigned int i = stepHandles.size(); i < gradient->GetStepCount(); i++) {
			CStepHandle *handle = new CStepHandle(this);
			stepHandles.push_back(handle);
			AddSubItem(handle);
		}
	}

	for (unsigned int i = 0; i < stepHandles.size(); i++) {
		stepHandles[i]->SetEnabled(i < (unsigned)gradient->GetStepCount());
		stepHandles[i]->SetGradientInfo(gradient, i);
	}
}

bool CGradientEditor::IsOKToSwitchGradients()
{
	for (std::vector<CStepHandle*>::iterator i = stepHandles.begin(); i != stepHandles.end(); i++) {
		if ((*i)->IsBeingDragged()) {
			return false;
		}
	}
	return true;
}

// CGradientEditor::CStepHandle member functions

CGradientEditor::CStepHandle::CStepHandle(CGradientEditor *parent)
{
	this->parent = parent;
	dragging = false;
	posInfoSet = false;
}

CGradientEditor::CStepHandle::~CStepHandle()
{
}

void CGradientEditor::CStepHandle::UpdateBounds()
{
	double stepPos = gradient->GetStepPosition(stepIndex);
	int ctrX = (int)Interpolate(stepPos, 0.0, 1.0, posXMin, posXMax);
	bounds.left = ctrX - 5;
	bounds.right = bounds.left + WIDTH - 1;
	bounds.bottom = posY;
	bounds.top = bounds.bottom - HEIGHT + 1;
}

void CGradientEditor::CStepHandle::OnDraw(HDC hDC, const LPRECT clientRect)
{
	int xSrc = dragging ? WIDTH : 0;
	UpdateBounds();
	MaskBlt(hDC, bounds.left, bounds.top, WIDTH, HEIGHT, parent->shBitmapDC, xSrc, 0, parent->shBmpMask, xSrc, 0, MAKEROP4(SRCCOPY, 0xAA0029));
	SetBrushOrgEx(hDC, -20, 0, NULL);
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hDC, gradient->GetStepColor(stepIndex));
	WorkingMaskBlt(hDC, bounds.left, bounds.top, WIDTH, HEIGHT, hDC, xSrc, 0, parent->shBmpColorMask, xSrc, 0, MAKEROP4(PATCOPY, 0xAA0029));
}

void CGradientEditor::CStepHandle::OnMouseDown(int x, int y)
{
	StartDragging(x);
	parent->StopHandlingSubItemEvents();
}

void CGradientEditor::CStepHandle::OnMouseMove(int x, int y)
{
	if (dragging && posInfoSet) {
		int deltaX = x - lastDragX;
		double deltaPos = Interpolate(deltaX, posXMin, posXMax, 0.0, 1.0);
		gradient->SetStepPosition(stepIndex, gradient->GetStepPosition(stepIndex) + deltaPos);
		lastDragX = x;
	}
}

void CGradientEditor::CStepHandle::OnMouseUp(int x, int y)
{
	dragging = false;
	gradient->PrecalculateColors();
}

void CGradientEditor::CStepHandle::OnRightClick(int x, int y)
{
	if (dragging) {
		if (gradient->GetStepCount() > 1) {
			dragging = false;
			gradient->DeleteStep(stepIndex);
			parent->SetGradient(gradient);
			gradient->PrecalculateColors();
		}
	} else {
		colorDlg.rgbResult = gradient->GetStepColor(stepIndex);
		if (ChooseColor(&colorDlg)) {
			gradient->SetStepColor(stepIndex, colorDlg.rgbResult);
		}
		gradient->PrecalculateColors();
	}
	parent->StopHandlingSubItemEvents();
}

bool CGradientEditor::CStepHandle::OccupiesPoint(int x, int y)
{
	return PtInRect(&bounds, { x, y }) > 0;
}

void CGradientEditor::CStepHandle::SetGradientInfo(CGradient *gradient, int stepIndex)
{
	this->gradient = gradient;
	this->stepIndex = stepIndex;
}

void CGradientEditor::CStepHandle::SetPositioningInfo(int xMin, int xMax, int y)
{
	posXMin = xMin;
	posXMax = xMax;
	posY = y;
	posInfoSet = true;
}

void CGradientEditor::CStepHandle::StartDragging(int x)
{
	dragging = true;
	lastDragX = x;
	SetMouseDownFlag(true);
}

bool CGradientEditor::CStepHandle::IsBeingDragged()
{
	return dragging;
}