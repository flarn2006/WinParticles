#include "stdafx.h"
#include "TextItem.h"

bool CTextItem::IsSelectable() const
{
	return false;
}

int CTextItem::GetMinVerbosity() const
{
	return minVerbosity;
}

void CTextItem::SetMinVerbosity(int verbosity)
{
	minVerbosity = verbosity;
}

CStaticTextItem::CStaticTextItem(const tstring &text)
{
	this->text = text;
}

void CStaticTextItem::GetText(tstring &text) const
{
	text = this->text;
}