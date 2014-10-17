#include "stdafx.h"
#include "RootDisplay.h"


CRootDisplay::CRootDisplay()
{
	numInputBox = new CNumericInputBox();
	AddSubItem(numInputBox);
}

CRootDisplay::~CRootDisplay()
{
}

void CRootDisplay::InitBitmapEditor(HDC hBitmapDC, LONG width, LONG height)
{
	bmpEditor = new CBitmapEditor(hBitmapDC, width, height);
	AddSubItem(bmpEditor);
}

void CRootDisplay::InitGradientEditor(CGradient *gradient)
{
	gradientEditor = new CGradientEditor(gradient);
	AddSubItem(gradientEditor);
}

void CRootDisplay::UpdateSize(const LPRECT clientRect)
{
	numInputBox->SetPosition((clientRect->left + clientRect->right) / 2, (clientRect->top + clientRect->bottom) / 2);
	bmpEditor->SetTopRightPos(clientRect->right - 16, clientRect->top + 16);
}

CNumericInputBox *CRootDisplay::GetNumInputBox()
{
	return numInputBox;
}

CBitmapEditor *CRootDisplay::GetBitmapEditor()
{
	return bmpEditor;
}

CGradientEditor *CRootDisplay::GetGradientEditor()
{
	return gradientEditor;
}