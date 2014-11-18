#pragma once
#include "Gradient.h"

class CChaoticGradient : public CGradient
{
public:
	CChaoticGradient();
	CChaoticGradient(const CGradient &other);
	CChaoticGradient(int numSteps);
	~CChaoticGradient();

	static bool IsChaotic(CGradient *gradient);
	virtual COLORREF ColorAtPoint(double position);
};

