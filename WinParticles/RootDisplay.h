#pragma once
#include "CompoundDispItem.h"
#include "NumericInputBox.h"
#include "BitmapEditor.h"
#include "GradientEditor.h"
#include "Gradient.h"
#include "AnimEditor.h"

class CRootDisplay : public CCompoundDispItem
{
private:
	CNumericInputBox *numInputBox;
	CBitmapEditor *bmpEditor;
	CGradientEditor *gradientEditor;
	CAnimEditor *animEditor;

public:
	CRootDisplay(); // You MUST also call the Init... functions
	~CRootDisplay();
	
	void InitBitmapEditor(CParticleBitmap *bitmap);
	void InitGradientEditor(CGradient *gradient);
	void InitAnimEditor(CAnimation<double> *animations);

	void UpdateSize(const LPRECT clientRect);

	CNumericInputBox *GetNumInputBox();
	CBitmapEditor *GetBitmapEditor();
	CGradientEditor *GetGradientEditor();
	CAnimEditor *GetAnimEditor();
};

