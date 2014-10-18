#pragma once
#include "Gradient.h"

class CChaoticGradient : public CGradient
{
public:
	CChaoticGradient();
	CChaoticGradient(int numSteps);
	~CChaoticGradient();
	virtual COLORREF ColorAtPoint(double position);
};

