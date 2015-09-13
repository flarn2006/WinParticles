#include "stdafx.h"
#include "TextDisplay.h"
#include "Common.h"

extern HFONT font;
extern int verbosity;

CTextDisplay::CTextDisplay()
{
	noSelection = true;
	myFont = font;
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
		if ((*i)->IsVisible()) {
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
	
	SelectObject(hDC, myFont);
	SetTextColor(hDC, 0);
	DrawText(hDC, out.str().c_str(), out.str().length(), &textRect, 0);
	textRect.left--; textRect.top--;
	SetTextColor(hDC, 0xFFFF00);
	DrawText(hDC, out.str().c_str(), out.str().length(), &textRect, 0);
}

void CTextDisplay::AddItem(CTextItem *item)
{
	items.push_back(item);

	if (item->IsSelectable()) {
		selectableIndices.push_back(items.size() - 1);
		if (noSelection) {
			SetSelectedItem(dynamic_cast<CSelectableTextItem*>(item));
			noSelection = false;
		}
	}
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
		if (selIndex >= selectableIndices.size() - 1)
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

void CTextDisplay::EnterKey()
{
	GetSelectedItem()->EnterKey();
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
	if (!noSelection) GetSelectedItem()->SetSelected(false);
	selIndex = index;
	noSelection = false;
	GetSelectedItem()->SetSelected(true);
}

bool CTextDisplay::SetSelectedItem(CSelectableTextItem *item)
{
	for (SelIndex i = 0; i < selectableIndices.size(); i++) {
		if (items[selectableIndices[i]] == item) {
			SetSelectedIndex(i);
			return true;
		}
	}
	return false;
}

HFONT CTextDisplay::GetFont() const
{
	return myFont;
}

void CTextDisplay::SetFont(HFONT newFont)
{
	myFont = newFont;
}
