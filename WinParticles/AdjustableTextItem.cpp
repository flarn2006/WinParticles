#include "stdafx.h"
#include "AdjustableTextItem.h"

extern double deltaMult;

CAdjustableTextItem::CAdjustableTextItem()
{
	prefixText = suffixText = TEXT("");
	defaultDeltaMult = 0;
}

void CAdjustableTextItem::GetItemText(tstring &text) const
{
	text = prefixText + to_tstring(GetValue()) + suffixText;
}

void CAdjustableTextItem::OnSelected()
{
	if (defaultDeltaMult != 0) {
		deltaMult = defaultDeltaMult;
	}
}

void CAdjustableTextItem::OnMouseWheel(short wheelDelta)
{
	double delta = (double)wheelDelta / WHEEL_DELTA;
	SetValue(GetValue() + delta * deltaMult);
}

void CAdjustableTextItem::SetPrefixSuffixText(const tstring &prefix, const tstring &suffix)
{
	prefixText = prefix;
	suffixText = suffix;
}

void CAdjustableTextItem::SetDefaultDeltaMult(double deltaMult)
{
	defaultDeltaMult = deltaMult;
}