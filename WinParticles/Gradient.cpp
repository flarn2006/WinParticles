#include "stdafx.h"
#include "Gradient.h"
#include "Common.h"

CGradient::CGradient()
{
	steps = new std::vector<Step>();
}

CGradient::CGradient(int numSteps)
{
	steps = new std::vector<Step>();
	for (int i = 0; i < numSteps; i++) {
		AddStep(0.0, 0);
	}
}

CGradient::~CGradient()
{
	delete steps;
}

COLORREF CGradient::GetStepColor(int index)
{
	return steps->at(index).color;
}

double CGradient::GetStepPosition(int index)
{
	return steps->at(index).position;
}

unsigned int CGradient::GetStepCount()
{
	return steps->size();
}

void CGradient::SetStepColor(int index, COLORREF color)
{
	steps->at(index).color = color;
}

void CGradient::SetStepPosition(int index, double position)
{
	steps->at(index).position = position;
}

void CGradient::SetStep(int index, double position, COLORREF color)
{
	SetStepColor(index, color);
	SetStepPosition(index, position);
}

unsigned int CGradient::AddStep(double position, COLORREF color)
{
	steps->push_back({ color, position });
	return steps->size() - 1;
}

void CGradient::DeleteStep(int index)
{
	std::vector<Step> *oldSteps = steps;
	steps = new std::vector<Step>();
	for (unsigned int i = 0; i < oldSteps->size(); i++) {
		if (i != index) {
			steps->push_back(oldSteps->at(i));
		}
	}
}

COLORREF CGradient::ColorAtPoint(double position)
{
	int index1, index2;
	bool useSameIndex = false;

	int closestIndex = -1;
	double closestPos = 0.0;
	for (unsigned int i = 0; i < steps->size(); i++) {
		if (steps->at(i).position <= position) {
			if (steps->at(i).position == position) return steps->at(i).color;
			if (closestPos < steps->at(i).position || closestIndex == -1) {
				closestIndex = i;
				closestPos = steps->at(i).position;
			}
		}
	}

	if (closestIndex == -1) {
		// Just find the one with the lowest (leftmost) position value
		closestPos = 0.0;
		for (unsigned int i = 0; i < steps->size(); i++) {
			if (steps->at(i).position < closestPos || closestIndex == -1) {
				closestIndex = i;
				closestPos = steps->at(i).position;
			}
		}
		useSameIndex = true;
	}

	index1 = closestIndex;

	if (useSameIndex) {
		index2 = index1;
	} else {
		closestIndex = -1;
		for (unsigned int i = 0; i < steps->size(); i++) {
			if (steps->at(i).position > steps->at(index1).position) {
				if (closestPos > steps->at(i).position || closestIndex == -1) {
					closestIndex = i;
					closestPos = steps->at(i).position;
				}
			}
		}

		index2 = (closestIndex == -1) ? index1 : closestIndex;
	}

	double relativePos;
	if (index1 == index2) {
		relativePos = 0.0; // doesn't matter what goes here as long as it's not infinity or something
	} else {
		relativePos = Interpolate(position, steps->at(index1).position, steps->at(index2).position, 0.0, 1.0);
	}
	BYTE red = (BYTE)Interpolate(relativePos, 0.0, 1.0, (double)GetRValue(steps->at(index1).color), (double)GetRValue(steps->at(index2).color));
	BYTE green = (BYTE)Interpolate(relativePos, 0.0, 1.0, (double)GetGValue(steps->at(index1).color), (double)GetGValue(steps->at(index2).color));
	BYTE blue = (BYTE)Interpolate(relativePos, 0.0, 1.0, (double)GetBValue(steps->at(index1).color), (double)GetBValue(steps->at(index2).color));
	return RGB(red, green, blue);
}