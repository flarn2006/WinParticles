#include "stdafx.h"
#include "Gradient.h"
#include <algorithm>

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

COLORREF CGradient::GetStepColor(int index)
{
	return steps[index].color;
}

double CGradient::GetStepPosition(int index)
{
	return steps[index].position;
}

unsigned int CGradient::GetStepCount()
{
	return steps.size();
}

void CGradient::SetStepColor(int index, COLORREF color)
{
	steps[index].color = color;
}

void CGradient::SetStepPosition(int index, double position)
{
	if (steps[index].position != position) {
		needsSorting = true;
		steps[index].position = position;
	}
}

void CGradient::SetStep(int index, double position, COLORREF color)
{
	SetStepColor(index, color);
	SetStepPosition(index, position);
}

unsigned int CGradient::AddStep(double position, COLORREF color)
{
	needsSorting = true;
	steps.push_back({ color, position });
	return steps.size() - 1;
}

void CGradient::DeleteStep(int index)
{
	DeleteVectorItem(steps, index);
}

void CGradient::DeleteAllSteps()
{
	steps.clear();
}

COLORREF CGradient::ColorAtPoint(double position)
{
	SortSteps();

	int index1 = -1;
	unsigned int count = GetStepCount();
	for (int i = 0; i < count; i++) {
		if (steps[i].position <= position) {
			index1 = i;
		} else {
			break;
		}
	}

	if (index1 == -1) {
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