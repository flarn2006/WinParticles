#include "stdafx.h"
#include "GradientEditor.h"
#include "Resource.h"
#include "ParticleBitmap.h"
#include "RootDisplay.h"
#include "ParticleSys.h"

extern HINSTANCE hInst;
extern CHOOSECOLOR colorDlg;
extern CParticleBitmap bitmap;  // to get number of cells (sub-images)
extern CRootDisplay *display;
extern CParticleSys *psys;

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

	const BYTE data[] = {
		0xEF, 0xFF, //11101111 11111111
		0xC7, 0xFF, //11000111 11111111
		0x83, 0xFF, //10000011 11111111
		0x01, 0xFF, //00000001 11111111
	};
	cellMarkerBmp = CreateBitmap(7, 4, 1, 1, data);
	cellMarkerBmpDC = CreateCompatibleDC(NULL);
	SelectObject(cellMarkerBmpDC, cellMarkerBmp);
}

CGradientEditor::~CGradientEditor()
{
	DeleteDC(shBitmapDC);
	DeleteObject(shBitmap);
	DeleteObject(shBmpMask);
	DeleteObject(shBmpColorMask);
	DeleteDC(cellMarkerBmpDC);
	DeleteObject(cellMarkerBmp);

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

	SetTextColor(hDC, 0xFFFFFF);
	int count = bitmap.GetCellCount();
	for (int i = 1; i < count; i++) {
		int x = i * (clientRect->right - clientRect->left) / count;
		BitBlt(hDC, x - 3, clientRect->bottom - 4, 7, 4, cellMarkerBmpDC, 0, 0, SRCINVERT);
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

void CGradientEditor::UpdatePsysGradient()
{
	psys->SetGradient(*gradient);
}

void CGradientEditor::OnMouseMove(int x, int y)
{
	COLORREF colorHere = gradient->ColorAtPoint(Interpolate(x, clientRect->left, clientRect->right, 0.0, 1.0));
	tstring colorStr; ColorToHTML(colorHere, colorStr);
	display->SetHelpText(TEXT("Click (and optionally drag) to add a new step to the gradient. [color here: ") + colorStr + TEXT("]"));
	CCompoundDispItem::OnMouseMove(x, y);
}

void CGradientEditor::OnMouseDown(int x, int y)
{
	CCompoundDispItem::OnMouseDown(x, y);
	if (!GetStopHandlingFlag()) {
		AddGradientStep(x)->StartDragging(x);
		UpdatePsysGradient();
	}
}

void CGradientEditor::OnRightClick(int x, int y)
{
	CCompoundDispItem::OnRightClick(x, y);
	if (!GetStopHandlingFlag()) {
		AddGradientStep(x);
		UpdatePsysGradient();
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
	tstring colorStr; ColorToHTML(gradient->GetStepColor(stepIndex), colorStr);
	display->SetHelpText(TEXT("Right click to change color, or right click while dragging to delete. [color = " + colorStr + TEXT("]")));
	if (dragging && posInfoSet) {
		int deltaX = x - lastDragX;
		double deltaPos = Interpolate(deltaX, posXMin, posXMax, 0.0, 1.0);
		gradient->SetStepPosition(stepIndex, gradient->GetStepPosition(stepIndex) + deltaPos);
		lastDragX = x;
		parent->UpdatePsysGradient();
	}
}

void CGradientEditor::CStepHandle::OnMouseUp(int x, int y)
{
	dragging = false;
	gradient->PrecalculateColors();
	parent->UpdatePsysGradient();
}

void CGradientEditor::CStepHandle::OnRightClick(int x, int y)
{
	if (dragging) {
		if (gradient->GetStepCount() > 1) {
			dragging = false;
			gradient->DeleteStep(stepIndex);
			parent->SetGradient(gradient);
			gradient->PrecalculateColors();
			parent->UpdatePsysGradient();
		}
	} else {
		colorDlg.rgbResult = gradient->GetStepColor(stepIndex);
		if (ChooseColor(&colorDlg)) {
			gradient->SetStepColor(stepIndex, colorDlg.rgbResult);
			gradient->PrecalculateColors();
			parent->UpdatePsysGradient();
		}
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