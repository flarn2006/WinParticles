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
		bool operator<(const Step &other); //for sorting
	};

	std::vector<Step> steps;
	bool needsSorting;

	void SortSteps();

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
	void SpaceEvenly();

	virtual COLORREF ColorAtPoint(double position);
};

