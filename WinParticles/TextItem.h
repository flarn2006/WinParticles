#pragma once
#include "Common.h"

class CTextItem
{
public:
	virtual void GetText(tstring &str) const = 0;
	virtual bool IsSelectable() const;
};

class CStaticTextItem : public CTextItem
{
private:
	tstring text;

public:
	CStaticTextItem(const tstring &text);
	virtual void GetText(tstring &str) const;
};