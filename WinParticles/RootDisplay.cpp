#include "stdafx.h"
#include "RootDisplay.h"


CRootDisplay::CRootDisplay()
{
	numInputBox = new CNumericInputBox();
	AddSubItem(numInputBox);
}

CRootDisplay::~CRootDisplay()
{
	delete numInputBox;
	delete bmpEditor;
	delete gradientEditor;
}

void CRootDisplay::InitBitmapEditor(CParticleBitmap *bitmap)
{
	bmpEditor = new CBitmapEditor(bitmap);
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