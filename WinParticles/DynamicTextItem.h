#pragma once
#include <functional>
#include "Common.h"
#include "TextItem.h"

class CDynamicTextItem : public CTextItem
{
public:
	typedef std::function<void(tostringstream &text)> Callback;

private:
	Callback callback;

public:
	CDynamicTextItem();
	CDynamicTextItem(const Callback &callback, int minVerbosity = 2);
	const Callback &GetCallback();
	void SetCallback(const Callback &callback);
	virtual void GetText(tstring &str) const;
};

