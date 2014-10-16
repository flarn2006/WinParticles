#include "stdafx.h"
#include "GradientEditor.h"
#include "Resource.h"
#include "Common.h"

extern HINSTANCE hInst;

CGradientEditor::CGradientEditor(CGradient *gradient)
{
	tint = 0xFFFFFF;
	colorDlg = NULL;
	SetGradient(gradient);
}

CGradientEditor::~CGradientEditor()
{
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

void CGradientEditor::SetGradient(CGradient *gradient)
{
	this->gradient = gradient;
	
	if ((unsigned)gradient->GetStepCount() > stepHandles.size()) {
		for (int i = stepHandles.size(); i < gradient->GetStepCount(); i++) {
			CStepHandle *handle = new CStepHandle(this);
			handle->SetColorDialog(colorDlg);
			stepHandles.push_back(handle);
			AddSubItem(handle);
		}
	}

	for (unsigned int i = 0; i < stepHandles.size(); i++) {
		stepHandles[i]->SetEnabled(i < (unsigned)gradient->GetStepCount());
		stepHandles[i]->SetGradientInfo(gradient, i);
	}
}

void CGradientEditor::SetColorDialog(LPCHOOSECOLOR colorDlg)
{
	this->colorDlg = colorDlg;
	for (std::vector<CStepHandle*>::iterator i = stepHandles.begin(); i != stepHandles.end(); i++) {
		(*i)->SetColorDialog(colorDlg);
	}
}

// CGradientEditor::CStepHandle member functions

CGradientEditor::CStepHandle::CStepHandle(CGradientEditor *parent)
{
	this->parent = parent;
	bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_STEPHANDLE));
	bmpMask = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_STEPHANDLE_MASK));
	bitmapDC = CreateCompatibleDC(NULL);
	SelectObject(bitmapDC, bitmap);
	dragging = false;
}

CGradientEditor::CStepHandle::~CStepHandle()
{
	DeleteDC(bitmapDC);
	DeleteObject(bitmap);
	DeleteObject(bmpMask);
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
	MaskBlt(hDC, bounds.left, bounds.top, WIDTH, HEIGHT, bitmapDC, xSrc, 0, bmpMask, 0, 0, MAKEROP4(SRCCOPY, 0xAA0029));
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hDC, gradient->GetStepColor(stepIndex));
	PatBlt(hDC, bounds.left + 3, bounds.top + 3, 6, 5, PATCOPY);
}

void CGradientEditor::CStepHandle::OnMouseDown(int x, int y)
{
	dragging = true;
	lastDragX = x;
	parent->StopHandlingSubItemEvents();
}

void CGradientEditor::CStepHandle::OnMouseMove(int x, int y)
{
	if (dragging) {
		int deltaX = x - lastDragX;
		double deltaPos = Interpolate(deltaX, posXMin, posXMax, 0.0, 1.0);
		gradient->SetStepPosition(stepIndex, gradient->GetStepPosition(stepIndex) + deltaPos);
		lastDragX = x;
	}
}

void CGradientEditor::CStepHandle::OnMouseUp(int x, int y)
{
	dragging = false;
}

void CGradientEditor::CStepHandle::OnRightClick(int x, int y)
{
	if (colorDlg) {
		colorDlg->rgbResult = gradient->GetStepColor(stepIndex);
		if (ChooseColor(colorDlg)) {
			gradient->SetStepColor(stepIndex, colorDlg->rgbResult);
		}
		parent->StopHandlingSubItemEvents();
	}
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
}

void CGradientEditor::CStepHandle::SetColorDialog(LPCHOOSECOLOR colorDlg)
{
	this->colorDlg = colorDlg;
}