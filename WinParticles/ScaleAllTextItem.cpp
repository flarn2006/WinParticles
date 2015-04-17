#include <cmath>
#include "stdafx.h"
#include "ScaleAllTextItem.h"

CScaleAllTextItem::CScaleAllTextItem()
{
}

void CScaleAllTextItem::GetItemText(tstring &text) const
{
	text = TEXT("(Scale entire system)");
}

void CScaleAllTextItem::OnMouseWheel(short wheelDelta)
{
	double multiplier = std::pow(2.0, (double)wheelDelta / WHEEL_DELTA / 16);
	for (auto i = affectedParams.begin(); i != affectedParams.end(); i++) {
		(*i)->SetValue(multiplier * (*i)->GetValue());
	}
}

void CScaleAllTextItem::AddParam(CAnimTarget<double> *param)
{
	affectedParams.push_back(param);
}