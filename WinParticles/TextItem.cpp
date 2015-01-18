#include "stdafx.h"
#include "TextItem.h"

extern int verbosity;

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

bool CTextItem::GetVisibility() const
{
	return visibility;
}

void CTextItem::SetVisibility(bool visibility)
{
	this->visibility = visibility;
}

bool CTextItem::IsVisible() const
{
	return GetVisibility() && (verbosity >= minVerbosity);
}

CStaticTextItem::CStaticTextItem(const tstring &text)
{
	this->text = text;
}

void CStaticTextItem::GetText(tstring &text) const
{
	text = this->text;
}