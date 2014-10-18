#include "stdafx.h"
#include "Gradient.h"
#include "Common.h"

CGradient::CGradient()
{
}

CGradient::CGradient(int numSteps)
{
	for (int i = 0; i < numSteps; i++) {
		steps.push_back({ 0, 0 });
	}
}

CGradient::~CGradient()
{
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
	steps[index].position = position;
}

void CGradient::SetStep(int index, double position, COLORREF color)
{
	SetStepColor(index, color);
	SetStepPosition(index, position);
}

COLORREF CGradient::ColorAtPoint(double position)
{
	int index1, index2;
	bool useSameIndex = false;

	int closestIndex = -1;
	double closestPos = 0.0;
	for (unsigned int i = 0; i < steps.size(); i++) {
		if (steps[i].position <= position) {
			if (steps[i].position == position) return steps[i].color;
			if (closestPos < steps[i].position || closestIndex == -1) {
				closestIndex = i;
				closestPos = steps[i].position;
			}
		}
	}

	if (closestIndex == -1) {
		// Just find the one with the lowest (leftmost) position value
		closestPos = 0.0;
		for (unsigned int i = 0; i < steps.size(); i++) {
			if (steps[i].position < closestPos || closestIndex == -1) {
				closestIndex = i;
				closestPos = steps[i].position;
			}
		}
		useSameIndex = true;
	}

	index1 = closestIndex;

	if (useSameIndex) {
		index2 = index1;
	} else {
		closestIndex = -1;
		for (unsigned int i = 0; i < steps.size(); i++) {
			if (steps[i].position > steps[index1].position) {
				if (closestPos > steps[i].position || closestIndex == -1) {
					closestIndex = i;
					closestPos = steps[i].position;
				}
			}
		}

		index2 = (closestIndex == -1) ? index1 : closestIndex;
	}

	double relativePos;
	if (index1 == index2) {
		relativePos = 0.0; // doesn't matter what goes here as long as it's not infinity or something
	} else {
		relativePos = Interpolate(position, steps[index1].position, steps[index2].position, 0.0, 1.0);
	}
	BYTE red = (BYTE)Interpolate(relativePos, 0.0, 1.0, (double)GetRValue(steps[index1].color), (double)GetRValue(steps[index2].color));
	BYTE green = (BYTE)Interpolate(relativePos, 0.0, 1.0, (double)GetGValue(steps[index1].color), (double)GetGValue(steps[index2].color));
	BYTE blue = (BYTE)Interpolate(relativePos, 0.0, 1.0, (double)GetBValue(steps[index1].color), (double)GetBValue(steps[index2].color));
	return RGB(red, green, blue);
}