#pragma once
#include <vector>
#include "SelectableTextItem.h"
#include "AnimTarget.h"

class CScaleAllTextItem : public CSelectableTextItem
{
private:
	std::vector<CAnimTarget<double>*> affectedParams;

protected:
	virtual void GetItemText(tstring &text) const;
	virtual void OnMouseWheel(short wheelDelta);

public:
	CScaleAllTextItem();
	void AddParam(CAnimTarget<double> *param);
};

