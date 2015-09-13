#include <cmath>
#include "stdafx.h"
#include "ScaleAllTextItem.h"
#include "RootDisplay.h"
#include "NumericInputBox.h"

extern CRootDisplay *display;

CScaleAllTextItem::CScaleAllTextItem(const tstring &itemText)
{
	text = itemText;
}

void CScaleAllTextItem::ScaleAllBy(double factor)
{
	std::size_t count = affectedParams.size();
	for (std::size_t i = 0; i < count; i++) {
		double newValue;
		if (inverted[i]) {
			newValue = affectedParams[i]->GetValue() / factor;
		} else {
			newValue = affectedParams[i]->GetValue() * factor;
		}
		affectedParams[i]->SetValue(newValue);
	}
}

void CScaleAllTextItem::GetItemText(tstring &text) const
{
	text = this->text;
}

void CScaleAllTextItem::OnMouseWheel(short wheelDelta)
{
	double multiplier = std::pow(2.0, (double)wheelDelta / WHEEL_DELTA / 16);
	ScaleAllBy(multiplier);
}

void CScaleAllTextItem::AddParam(CAnimTarget<double> *param, bool inverted)
{
	affectedParams.push_back(param);
	this->inverted.push_back(inverted);
}

void CScaleAllTextItem::OnEnterKey()
{
	display->GetNumInputBox()->PromptForValue((CNumericInputBox::Callback)[this](double value) {
		ScaleAllBy(value);
	}, TEXT("Scale by factor:"));
}