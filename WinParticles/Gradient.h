#pragma once
#include <Windows.h>
#include <vector>

class CGradient
{
private:
	struct Step
	{
		COLORREF color;
		double position;
	};

	std::vector<Step> *steps;

public:
	CGradient();
	CGradient(int numSteps);
	~CGradient();

	COLORREF GetStepColor(int index);
	double GetStepPosition(int index);
	unsigned int GetStepCount();
	void SetStepColor(int index, COLORREF color);
	void SetStepPosition(int index, double position);
	void SetStep(int index, double position, COLORREF color);
	unsigned int AddStep(double position, COLORREF color);
	void DeleteStep(int index);
	void DeleteAllSteps();

	virtual COLORREF ColorAtPoint(double position);
};

