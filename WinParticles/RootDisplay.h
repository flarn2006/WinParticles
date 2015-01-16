#pragma once
#include "CompoundDispItem.h"
#include "NumericInputBox.h"
#include "BitmapEditor.h"
#include "GradientEditor.h"
#include "Gradient.h"
#include "AnimEditor.h"
#include "HelpText.h"
#include "TextDisplay.h"

class CRootDisplay : public CCompoundDispItem
{
private:
	CNumericInputBox *numInputBox;
	CBitmapEditor *bmpEditor;
	CGradientEditor *gradientEditor;
	CAnimEditor *animEditor;
	CHelpText *helpText;
	CTextDisplay *textDisplay;

public:
	CRootDisplay(); // You MUST also call the Init... functions
	~CRootDisplay();
	
	void InitBitmapEditor(CParticleBitmap *bitmap);
	void InitGradientEditor(CGradient *gradient);
	void InitAnimEditor(CAnimation<double> *animations);

	void UpdateSize(const LPRECT clientRect);
	void SetHelpText(const tstring &text = TEXT(""));

	CNumericInputBox *GetNumInputBox();
	CBitmapEditor *GetBitmapEditor();
	CGradientEditor *GetGradientEditor();
	CAnimEditor *GetAnimEditor();
	CHelpText *GetHelpTextObj();
	CTextDisplay *GetTextDisplay();
};

