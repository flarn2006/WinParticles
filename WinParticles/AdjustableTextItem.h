#pragma once
#include "SelectableTextItem.h"

class CAdjustableTextItem : public CSelectableTextItem
{
private:
	tstring prefixText, suffixText;
	double defaultDeltaMult;

protected:
	virtual void GetItemText(tstring &text) const;
	virtual void OnSelected();
	virtual void OnMouseWheel(short wheelDelta);
	virtual void OnEnterKey();

public:
	CAdjustableTextItem();
	virtual double GetValue() const = 0;
	virtual void SetValue(double value) = 0;
	void SetPrefixSuffixText(const tstring &prefix, const tstring &suffix);
	void SetDefaultDeltaMult(double deltaMult);
};

