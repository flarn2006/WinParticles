#pragma once
#include <functional>
#include "AdjustableTextItem.h"

template <typename T>
class CCustomAdjTextItem : public CAdjustableTextItem<T>
{
public:
	typedef std::function<T()> Callback_Get;
	typedef std::function<void(T)> Callback_Set;

private:
	Callback_Get callbackGet;
	Callback_Set callbackSet;

protected:
	virtual void OnSelected()
	{
		CAdjustableTextItem<T>::OnSelected();
		SetAnimEditorID(-1);
	}

public:
	void SetCallbackForGet(const Callback_Get &callback)
	{
		callbackGet = callback;
	}

	void SetCallbackForSet(const Callback_Set &callback)
	{
		callbackSet = callback;
	}

	virtual T GetValue() const
	{
		return callbackGet();
	}

	virtual void SetValue(T value)
	{
		callbackSet(value);
	}
};