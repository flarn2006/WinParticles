#include "stdafx.h"
#include "ChaoticGradient.h"
#include "Common.h"

CChaoticGradient::CChaoticGradient()
{
}

CChaoticGradient::CChaoticGradient(int numSteps)
: CGradient(numSteps)
{
}

CChaoticGradient::~CChaoticGradient()
{
}

COLORREF CChaoticGradient::ColorAtPoint(double position)
{
	return CGradient::ColorAtPoint(RandInRange(0.0, 1.0));
}