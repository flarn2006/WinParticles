#pragma once
#include <vector>
#include "SelectableTextItem.h"
#include "AnimTarget.h"

class CScaleAllTextItem : public CSelectableTextItem
{
private:
	std::vector<CAnimTarget<double>*> affectedParams;
	std::vector<bool> inverted;
	tstring text;
	void ScaleAllBy(double factor);

protected:
	virtual void GetItemText(tstring &text) const;
	virtual void OnMouseWheel(short wheelDelta);
	virtual void OnEnterKey();

public:
	CScaleAllTextItem(const tstring &itemText);
	void AddParam(CAnimTarget<double> *param, bool inverted = false);
};

