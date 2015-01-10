#include "stdafx.h"
#include "TextItem.h"

bool CTextItem::IsSelectable() const
{
	return false;
}

CStaticTextItem::CStaticTextItem(const tstring &text)
{
	this->text = text;
}

void CStaticTextItem::GetText(tstring &text) const
{
	text = this->text;
}