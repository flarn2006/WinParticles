#pragma once
#include "CompoundDispItem.h"
#include "NumericInputBox.h"
#include "BitmapEditor.h"
#include "GradientEditor.h"
#include "Gradient.h"

class CRootDisplay : public CCompoundDispItem
{
private:
	CNumericInputBox *numInputBox;
	CBitmapEditor *bmpEditor;
	CGradientEditor *gradientEditor;

public:
	CRootDisplay(); // You MUST also call InitBitmapEditor and InitGradientEditor
	~CRootDisplay();
	
	void InitBitmapEditor(CParticleBitmap *bitmap);
	void InitGradientEditor(CGradient *gradient);

	void UpdateSize(const LPRECT clientRect);

	CNumericInputBox *GetNumInputBox();
	CBitmapEditor *GetBitmapEditor();
	CGradientEditor *GetGradientEditor();
};

