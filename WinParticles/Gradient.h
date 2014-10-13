#pragma once
#include <Windows.h>

class CGradient
{
private:
	struct Step
	{
		COLORREF color;
		double position;
	};

	Step *steps;
	int numSteps;

public:
	CGradient(int numSteps);
	~CGradient();

	COLORREF GetStepColor(int index);
	double GetStepPosition(int index);
	int GetStepCount();
	void SetStepColor(int index, COLORREF color);
	void SetStepPosition(int index, double position);
	void SetStep(int index, double position, COLORREF color);

	COLORREF ColorAtPoint(double position);
};

