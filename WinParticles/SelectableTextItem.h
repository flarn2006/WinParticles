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

public:
	CSelectableTextItem();

	void SetSelected(bool state);
	void MouseWheel(short wheelDelta);

	virtual void GetText(tstring &text) const;
	bool IsSelectable() const;
};

