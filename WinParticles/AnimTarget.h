#pragma once

template <typename TValue>
class CAnimTarget
{
public:
	virtual TValue GetValue() = 0;
	virtual void SetValue(TValue newValue) = 0;
};

