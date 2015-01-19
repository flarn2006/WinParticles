#pragma once
#include "TextItem.h"

class CSelectableTextItem : public CTextItem
{
private:
	bool isSelected;

protected:
	virtual void GetItemText(tstring &text) const = 0;
	virtual void OnSelected();
	virtual void OnMouseWheel(short wheelDelta);
	virtual void OnReactToNumInput(double value);

public:
	CSelectableTextItem();

	void SetSelected(bool state);
	void MouseWheel(short wheelDelta);
	void ReactToNumInput(double value);

	virtual void GetText(tstring &text) const;
	bool IsSelectable() const;
};

