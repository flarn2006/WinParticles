#include "stdafx.h"
#include "TextDisplay.h"
#include "Common.h"

extern int verbosity;

CTextDisplay::CTextDisplay()
{
	noSelection = true;
}

CTextDisplay::~CTextDisplay()
{
	for (std::vector<CTextItem*>::iterator i = items.begin(); i != items.end(); i++) {
		delete *i;
	}
}

void CTextDisplay::OnDraw(HDC hDC, const LPRECT clientRect)
{
	tostringstream out;
	
	for (std::vector<CTextItem*>::iterator i = items.begin(); i != items.end(); i++) {
		if (verbosity >= (*i)->GetMinVerbosity()) {
			tstring text;
			(*i)->GetText(text);
			out << text << std::endl;
		}
	}

	RECT textRect;
	textRect.left = 5;
	textRect.top = 5;
	textRect.right = clientRect->right;
	textRect.bottom = clientRect->bottom;
	
	SetTextColor(hDC, 0);
	DrawText(hDC, out.str().c_str(), out.str().length(), &textRect, 0);
	textRect.left--; textRect.top--;
	SetTextColor(hDC, 0xFFFF00);
	DrawText(hDC, out.str().c_str(), out.str().length(), &textRect, 0);
}

void CTextDisplay::AddItem(CTextItem *item)
{
	if (item->IsSelectable()) {
		selectableIndices.push_back(items.size());
		if (noSelection) {
			selIndex = items.size();
			noSelection = false;
		}
	}

	items.push_back(item);
}

CStaticTextItem *CTextDisplay::AddText(const tstring &text)
{
	CStaticTextItem *item = new CStaticTextItem(text);
	items.push_back(item);
	return item;
}

void CTextDisplay::LeftClick()
{
	if (!noSelection) {
		if (selIndex == 0)
			SetSelectedIndex(selectableIndices.size() - 1);
		else
			SetSelectedIndex(selIndex - 1);
	}
}

void CTextDisplay::RightClick()
{
	if (!noSelection) {
		if (selIndex >= selectableIndices.size())
			SetSelectedIndex(0);
		else
			SetSelectedIndex(selIndex + 1);
	}
}

void CTextDisplay::MouseWheel(short wheelDelta)
{
	CSelectableTextItem *selItem = GetSelectedItem();
	if (selItem) {
		selItem->MouseWheel(wheelDelta);
	}
}

CSelectableTextItem *CTextDisplay::GetSelectedItem()
{
	if (noSelection)
		return NULL;
	else
		return dynamic_cast<CSelectableTextItem*>(items[selectableIndices[selIndex]]);
}

void CTextDisplay::SetSelectedIndex(SelIndex index)
{
	GetSelectedItem()->SetSelected(false);
	selIndex = index;
	GetSelectedItem()->SetSelected(true);
}

bool CTextDisplay::SetSelectedItem(CSelectableTextItem *item)
{
	for (SelIndex i = 0; i < selectableIndices.size(); i++) {
		if (items[i] == item) {
			SetSelectedIndex(i);
			return true;
		}
	}
	return false;
}