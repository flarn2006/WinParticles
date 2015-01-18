#include "stdafx.h"
#include "DynamicTextItem.h"

CDynamicTextItem::CDynamicTextItem()
{
	callback = [](tostringstream &ss){};
}

CDynamicTextItem::CDynamicTextItem(const Callback &callback, int minVerbosity)
{
	SetCallback(callback);
	SetMinVerbosity(minVerbosity);
}

const CDynamicTextItem::Callback &CDynamicTextItem::GetCallback()
{
	return callback;
}

void CDynamicTextItem::SetCallback(const CDynamicTextItem::Callback &callback)
{
	this->callback = callback;
}

void CDynamicTextItem::GetText(tstring &str) const
{
	tostringstream ss;
	callback(ss);
	str = ss.str();
}