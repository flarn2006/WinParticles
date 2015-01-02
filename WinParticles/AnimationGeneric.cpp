#include "stdafx.h"
#include "Animation.h"

bool CAnimationGeneric::GetEnabled()
{
	return enabled;
}

void CAnimationGeneric::SetEnabled(bool enabled)
{
	this->enabled = enabled;
}

double CAnimationGeneric::GetFrequency()
{
	return freq;
}

void CAnimationGeneric::SetFrequency(double frequency)
{
	freq = frequency;
}