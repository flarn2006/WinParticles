#include "stdafx.h"
#include "ChaoticGradient.h"
#include "Common.h"

CChaoticGradient::CChaoticGradient()
{
}

CChaoticGradient::CChaoticGradient(const CGradient &other)
: CGradient(other) { }

CChaoticGradient::CChaoticGradient(int numSteps)
: CGradient(numSteps)
{
}

CChaoticGradient::~CChaoticGradient()
{
}

bool CChaoticGradient::IsChaotic(CGradient *gradient)
{
	return (dynamic_cast<CChaoticGradient*>(gradient) != NULL);
}

COLORREF CChaoticGradient::ColorAtPoint(double position)
{
	return CGradient::ColorAtPoint(RandInRange(0.0, 1.0));
}