#include "stdafx.h"
#include "SelectableTextItem.h"

CSelectableTextItem::CSelectableTextItem()
{
	isSelected = false;
}

void CSelectableTextItem::OnSelected()
{
}

void CSelectableTextItem::OnMouseWheel(short wheelDelta)
{
}

void CSelectableTextItem::SetSelected(bool state)
{
	isSelected = state;
	if (isSelected) OnSelected();
}

void CSelectableTextItem::MouseWheel(short wheelDelta)
{
	// This is separate just in case I ever add some kind of additional code here.
	OnMouseWheel(wheelDelta);
}

void CSelectableTextItem::GetText(tstring &text) const
{
	tstring itemText;
	GetItemText(itemText);
	text = (isSelected ? TEXT("> ") : TEXT("  ")) + itemText;
}

bool CSelectableTextItem::IsSelectable() const
{
	return true;
}