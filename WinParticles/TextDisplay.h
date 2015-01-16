#pragma once
#include <vector>
#include "Common.h"
#include "TextItem.h"
#include "DisplayItem.h"
#include "SelectableTextItem.h"

class CTextDisplay : public CDisplayItem
{
public:
	typedef std::vector<std::vector<CTextItem*>::size_type>::size_type SelIndex;

private:
	std::vector<CTextItem*> items;
	std::vector<std::vector<CTextItem*>::size_type> selectableIndices;
	SelIndex selIndex;
	bool noSelection;

	void SetSelectedIndex(SelIndex index);

protected:
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);

public:
	CTextDisplay();
	~CTextDisplay();
	void AddItem(CTextItem *item);
	CStaticTextItem *AddText(const tstring &text);
	void LeftClick();
	void RightClick();
	/*new*/ void MouseWheel(short wheelDelta);
	CSelectableTextItem *GetSelectedItem();
	bool SetSelectedItem(CSelectableTextItem *item);
};

