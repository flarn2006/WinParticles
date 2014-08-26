#include "stdafx.h"
#include "Gradient.h"
#include "Common.h"

CGradient::CGradient(int numSteps)
{
	steps = new Step[numSteps];
	this->numSteps = numSteps;
}

CGradient::~CGradient()
{
	delete[] steps;
}

COLORREF CGradient::GetStepColor(int index)
{
	return steps[index].color;
}

double CGradient::GetStepPosition(int index)
{
	return steps[index].position;
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

	int closestIndex = -1;
	double closestPos = 0.0;
	for (int i = 0; i < numSteps; i++) {
		if (steps[i].position <= position) {
			if (steps[i].position == position) return steps[i].color;
			if (closestPos < steps[i].position || closestIndex == -1) {
				closestIndex = i;
				closestPos = steps[i].position;
			}
		}
	}

	index1 = closestIndex;
	closestIndex = -1;

	for (int i = 0; i < numSteps; i++) {
		if (steps[i].position > steps[index1].position) {
			if (closestPos > steps[i].position || closestIndex == -1) {
				closestIndex = i;
				closestPos = steps[i].position;
			}
		}
	}

	index2 = closestIndex;

	double relativePos = Interpolate(position, steps[index1].position, steps[index2].position, 0.0, 1.0);
	BYTE red = (BYTE)Interpolate(relativePos, 0.0, 1.0, (double)GetRValue(steps[index1].color), (double)GetRValue(steps[index2].color));
	BYTE green = (BYTE)Interpolate(relativePos, 0.0, 1.0, (double)GetGValue(steps[index1].color), (double)GetGValue(steps[index2].color));
	BYTE blue = (BYTE)Interpolate(relativePos, 0.0, 1.0, (double)GetBValue(steps[index1].color), (double)GetBValue(steps[index2].color));
	return RGB(red, green, blue);
}