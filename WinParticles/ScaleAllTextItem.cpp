#include <cmath>
#include "stdafx.h"
#include "ScaleAllTextItem.h"
#include "RootDisplay.h"
#include "NumericInputBox.h"

extern CRootDisplay *display;

CScaleAllTextItem::CScaleAllTextItem()
{
}

void CScaleAllTextItem::ScaleAllBy(double factor)
{
	for (auto i = affectedParams.begin(); i != affectedParams.end(); i++) {
		(*i)->SetValue(factor * (*i)->GetValue());
	}
}

void CScaleAllTextItem::GetItemText(tstring &text) const
{
	text = TEXT("(Scale entire system)");
}

void CScaleAllTextItem::OnMouseWheel(short wheelDelta)
{
	double multiplier = std::pow(2.0, (double)wheelDelta / WHEEL_DELTA / 16);
	ScaleAllBy(multiplier);
}

void CScaleAllTextItem::AddParam(CAnimTarget<double> *param)
{
	affectedParams.push_back(param);
}

void CScaleAllTextItem::OnEnterKey()
{
	display->GetNumInputBox()->PromptForValue((CNumericInputBox::Callback)[this](double value) {
		ScaleAllBy(value);
	}, TEXT("Scale by factor:"));
}