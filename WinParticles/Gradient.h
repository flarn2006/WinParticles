#pragma once
#include <Windows.h>
#include <vector>

class CGradient
{
private:
	struct Step
	{
		int id;
		COLORREF color;
		double position;
		bool operator<(const Step &other); //for sorting
	};

	std::vector<Step> steps;
	bool needsSorting;
	int nextStepID = 0;

	void SortSteps();
	bool FindStep(int id, unsigned int *indexOut = NULL);
	Step &StepWithID(int id);

public:
	CGradient();
	CGradient(int numSteps);

	int GetStepID(unsigned int index);
	COLORREF GetStepColor(int id);
	double GetStepPosition(int id);
	unsigned int GetStepCount();
	void SetStepColor(int id, COLORREF color);
	void SetStepPosition(int id, double position);
	void SetStep(int id, double position, COLORREF color);
	int AddStep(double position, COLORREF color);
	bool DeleteStep(int id);
	void DeleteAllSteps();
	void SpaceEvenly();

	virtual COLORREF ColorAtPoint(double position);
};

