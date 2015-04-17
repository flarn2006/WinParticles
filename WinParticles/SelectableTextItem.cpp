#include "stdafx.h"
#include "SelectableTextItem.h"
#include "RootDisplay.h"

extern CRootDisplay *display;

CSelectableTextItem::CSelectableTextItem()
{
	isSelected = false;
}

void CSelectableTextItem::OnSelected()
{
	SetAnimEditorID(-1);
}

void CSelectableTextItem::OnMouseWheel(short wheelDelta)
{
}

void CSelectableTextItem::OnEnterKey()
{
}

void CSelectableTextItem::SetAnimEditorID(int id)
{
	display->GetAnimEditor()->SetSelectedID(id);
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

void CSelectableTextItem::EnterKey()
{
	// Same with this.
	OnEnterKey();
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