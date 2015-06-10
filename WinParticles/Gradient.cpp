#include "stdafx.h"
#include "Gradient.h"
#include <algorithm>
#include <exception>
#include <climits>

CGradient::CGradient()
{
}

CGradient::CGradient(int numSteps)
{
	for (int i = 0; i < numSteps; i++) {
		AddStep(0.0, 0);
	}
	needsSorting = false;
}

void CGradient::SortSteps()
{
	if (needsSorting) {
		std::sort(steps.begin(), steps.end());
		needsSorting = false;
	}
}

bool CGradient::FindStep(int id, unsigned int *indexOut)
{
	for (unsigned int i = 0; i < steps.size(); i++) {
		if (steps[i].id == id) {
			if (indexOut) *indexOut = i;
			return true;
		}
	}
	return false;
}

CGradient::Step &CGradient::StepWithID(int id)
{
	unsigned int index;
	if (FindStep(id, &index)) {
		return steps[index];
	} else {
		throw std::exception("Step ID not found");
	}
}

int CGradient::GetStepID(unsigned int index)
{
	return steps[index].id;
}

COLORREF CGradient::GetStepColor(int id)
{
	return StepWithID(id).color;
}

double CGradient::GetStepPosition(int id)
{
	return StepWithID(id).position;
}

unsigned int CGradient::GetStepCount()
{
	return steps.size();
}

void CGradient::SetStepColor(int id, COLORREF color)
{
	StepWithID(id).color = color;
}

void CGradient::SetStepPosition(int id, double position)
{
	if (StepWithID(id).position != position) {
		needsSorting = true;
		StepWithID(id).position = position;
	}
}

void CGradient::SetStep(int id, double position, COLORREF color)
{
	SetStepColor(id, color);
	SetStepPosition(id, position);
}

int CGradient::AddStep(double position, COLORREF color)
{
	needsSorting = true;
	steps.push_back({ nextStepID++, color, position });
	return steps.size() - 1;
}

bool CGradient::DeleteStep(int id)
{
	unsigned int index;
	if (FindStep(id, &index)) {
		DeleteVectorItem(steps, index);
		return true;
	} else {
		return false;
	}
}

void CGradient::DeleteAllSteps()
{
	steps.clear();
	nextStepID = 0;
}

COLORREF CGradient::ColorAtPoint(double position)
{
	SortSteps();

	unsigned int index1 = UINT_MAX;
	unsigned int count = GetStepCount();
	for (unsigned int i = 0; i < count; i++) {
		if (steps[i].position <= position) {
			index1 = i;
		} else {
			break;
		}
	}

	if (index1 == UINT_MAX) {
		// No steps before this point
		return steps[0].color;
	} else if (index1 + 1 >= count) {
		// No steps after this point
		return steps[index1].color;
	} else if (steps[index1].position == position) {
		// Step is exactly on point
		return steps[index1].color;
	} else {
		// Steps on both sides of point
		Step &step1 = steps[index1];
		Step &step2 = steps[index1 + 1];
		return InterpolateColor(step1.color, step2.color, Interpolate(position, step1.position, step2.position, 0.0, 1.0));
	}
}

void CGradient::SpaceEvenly()
{
	SortSteps();
	unsigned int count = GetStepCount();
	if (count > 1) {
		for (unsigned int i = 0; i < count; i++) {
			double newPos = (double)i / (count - 1);
			SetStepPosition(i, newPos);
		}
	} else if (count == 1) {
		SetStepPosition(0, 0.5);
	}
}

bool CGradient::Step::operator<(const CGradient::Step &other)
{
	return position < other.position;
}