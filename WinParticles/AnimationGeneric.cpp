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