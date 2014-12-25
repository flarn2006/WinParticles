#pragma once
#include "AnimTarget.h"
#include "AnimFunctions.h"
#include "Common.h"
#include <cmath>

#ifndef NULL
#define NULL 0
#endif

class CAnimationGeneric
{
public:
	virtual void Run(double time) = 0;
};

template <typename TValue>
class CAnimation : public CAnimationGeneric
{
public:
	typedef TValue(*AnimFunction)(TValue time);

private:
	CAnimTarget<TValue> *target;
	AnimFunction function;
	double currentTime, freq;
	TValue min, max;
	bool enabled;

public:
	CAnimation()
	{
		SetTarget(NULL);
		SetFunction(AnimFunctions::Saw);
		SetRange(0.0, 1.0);
		SetFrequency(1.0);
		SetEnabled(false);
	}

	~CAnimation()
	{
	}

	CAnimTarget<TValue> *GetTarget()
	{
		return target;
	}

	AnimFunction GetFunction()
	{
		return function;
	}

	void GetRange(double *min, double *max)
	{
		if (min) *min = this->min;
		if (max) *max = this->max;
	}

	double GetFrequency()
	{
		return freq;
	}

	bool GetEnabled()
	{
		return enabled;
	}
	
	void SetTarget(CAnimTarget<TValue> *target)
	{
		this->target = target;
	}

	void SetFunction(AnimFunction function)
	{
		this->function = function;
	}

	void SetRange(TValue min, TValue max)
	{
		this->min = min;
		this->max = max;
	}

	void SetFrequency(double frequency)
	{
		freq = frequency;
	}

	void SetEnabled(bool enabled)
	{
		this->enabled = enabled;
	}

	void Run(double time)
	{
		if (enabled) {
			time *= freq;
			currentTime = std::fmod(currentTime + time, 1.0);
			if (target) **target = Interpolate(function(currentTime), 0.0, 1.0, min, max);
		}
	}
};