#pragma once
#include <vector>
#include "SelectableTextItem.h"
#include "AnimTarget.h"

class CScaleAllTextItem : public CSelectableTextItem
{
private:
	std::vector<CAnimTarget<double>*> affectedParams;
	void ScaleAllBy(double factor);

protected:
	virtual void GetItemText(tstring &text) const;
	virtual void OnMouseWheel(short wheelDelta);
	virtual void OnEnterKey();

public:
	CScaleAllTextItem();
	void AddParam(CAnimTarget<double> *param);
};

