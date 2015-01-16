#pragma once
#include "Common.h"

class CTextItem
{
private:
	int minVerbosity = 2;

public:
	virtual void GetText(tstring &str) const = 0;
	virtual bool IsSelectable() const;
	int GetMinVerbosity() const;
	void SetMinVerbosity(int verbosity);
};

class CStaticTextItem : public CTextItem
{
private:
	tstring text;

public:
	CStaticTextItem(const tstring &text);
	virtual void GetText(tstring &str) const;
};