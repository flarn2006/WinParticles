#pragma once
#include "SelectableTextItem.h"
#include "AnimTarget.h"
#include "RootDisplay.h"

extern CRootDisplay *display;
extern double deltaMult;

template <typename _T>
class CAdjustableTextItem : public CSelectableTextItem, public CAnimTarget<double>
{
private:
	tstring prefixText, suffixText;
	double defaultDeltaMult;

protected:
	virtual void GetItemText(tstring &text) const
	{
		text = prefixText + to_tstring(GetValue()) + suffixText;
	}

	virtual void OnSelected()
	{
		if (defaultDeltaMult != 0) {
			deltaMult = defaultDeltaMult;
		}
	}

	virtual void OnMouseWheel(short wheelDelta)
	{
		double delta = (double)wheelDelta / WHEEL_DELTA;
		SetValue(GetValue() + delta * deltaMult);
	}

	virtual void OnEnterKey()
	{
		display->GetNumInputBox()->PromptForValue((CNumericInputBox::Callback)[this](double value) {
			SetValue(value);
		});
	}

public:
	CAdjustableTextItem()
	{
		prefixText = suffixText = TEXT("");
		defaultDeltaMult = 0;
	}

	virtual double GetValue() const = 0;

	virtual void SetValue(double value) = 0;

	void SetPrefixSuffixText(const tstring &prefix, const tstring &suffix)
	{
		prefixText = prefix;
		suffixText = suffix;
	}

	void SetDefaultDeltaMult(double deltaMult)
	{
		defaultDeltaMult = deltaMult;
	}
};

