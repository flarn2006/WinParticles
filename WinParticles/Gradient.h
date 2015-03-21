#pragma once
#include <Windows.h>
#include <vector>

class CGradient
{
private:
	static const int PRECALC_MIN_STEPS = 10;  // minimum number of steps to trigger precalculation
	static const int PRECALC_STEPS = 1024;    // number of individual gradient steps to be precalculated

	struct Step
	{
		COLORREF color;
		double position;
	};

	std::vector<Step> steps;
	COLORREF precalc[PRECALC_STEPS];
	bool usePrecalc;

public:
	CGradient();
	CGradient(int numSteps);

	COLORREF GetStepColor(int index);
	double GetStepPosition(int index);
	unsigned int GetStepCount();
	void SetStepColor(int index, COLORREF color);
	void SetStepPosition(int index, double position);
	void SetStep(int index, double position, COLORREF color);
	unsigned int AddStep(double position, COLORREF color);
	void DeleteStep(int index);
	void DeleteAllSteps();
	void PrecalculateColors();
	void SpaceEvenly();

	virtual COLORREF ColorAtPoint(double position);
};

