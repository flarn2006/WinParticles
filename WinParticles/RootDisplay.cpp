#include "stdafx.h"
#include "RootDisplay.h"

CRootDisplay::CRootDisplay()
{
	numInputBox = new CNumericInputBox();
	helpText = new CHelpText();
	textDisplay = new CTextDisplay();
	AddSubItem(numInputBox);
	AddSubItem(helpText);
	AddSubItem(textDisplay);
}

CRootDisplay::~CRootDisplay()
{
	delete numInputBox;
	delete bmpEditor;
	delete gradientEditor;
	delete animEditor;
	delete helpText;
	delete textDisplay;
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

void CRootDisplay::InitAnimEditor(CAnimation<double> *animations)
{
	animEditor = new CAnimEditor(animations);
	AddSubItem(animEditor);
}

void CRootDisplay::UpdateSize(const LPRECT clientRect)
{
	numInputBox->SetPosition((clientRect->left + clientRect->right) / 2, (clientRect->top + clientRect->bottom) / 2);
	bmpEditor->SetTopRightPos(clientRect->right - 16, clientRect->top + 16);
	animEditor->SetPosition(clientRect->left + 16, clientRect->bottom - (gradientEditor->GetEnabled() ? 176 : 144));
	helpText->SetBottomRight(clientRect->bottom - (gradientEditor->GetEnabled() ? 40 : 8), clientRect->right - 8);
}

void CRootDisplay::SetHelpText(const tstring &text)
{
	helpText->SetText(text);
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

CAnimEditor *CRootDisplay::GetAnimEditor()
{
	return animEditor;
}

CHelpText *CRootDisplay::GetHelpTextObj()
{
	return helpText;
}

CTextDisplay *CRootDisplay::GetTextDisplay()
{
	return textDisplay;
}