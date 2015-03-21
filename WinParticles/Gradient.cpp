#include "stdafx.h"
#include "Gradient.h"

CGradient::CGradient()
{
}

CGradient::CGradient(int numSteps)
{
	for (int i = 0; i < numSteps; i++) {
		AddStep(0.0, 0);
	}
	for (int i = 0; i < PRECALC_STEPS; i++) {
		precalc[i] = 0;
	}
	usePrecalc = true;
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
	usePrecalc = false;
	steps[index].color = color;
}

void CGradient::SetStepPosition(int index, double position)
{
	usePrecalc = false;
	steps[index].position = position;
}

void CGradient::SetStep(int index, double position, COLORREF color)
{
	SetStepColor(index, color);
	SetStepPosition(index, position);
}

unsigned int CGradient::AddStep(double position, COLORREF color)
{
	usePrecalc = false;
	steps.push_back({ color, position });
	return steps.size() - 1;
}

void CGradient::DeleteStep(int index)
{
	usePrecalc = false;
	DeleteVectorItem(steps, index);
}

void CGradient::DeleteAllSteps()
{
	usePrecalc = false;
	steps.clear();
}

COLORREF CGradient::ColorAtPoint(double position)
{
	if (usePrecalc) {
		return precalc[(int)((PRECALC_STEPS - 1) * position)];
	} else {
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
		
		return InterpolateColor(steps[index1].color, steps[index2].color, relativePos);
	}
}

void CGradient::PrecalculateColors()
{
	if (GetStepCount() >= PRECALC_MIN_STEPS) {
		for (int i = 0; i < PRECALC_STEPS; i++) {
			precalc[i] = ColorAtPoint(i / (double)(PRECALC_STEPS - 1));
		}
		usePrecalc = true;
	} else {
		usePrecalc = false;
	}
}

void CGradient::SpaceEvenly()
{
	unsigned int count = GetStepCount();
	for (unsigned int i = 0; i < count; i++) {
		double newPos = (double)i / (count - 1);
		SetStepPosition(i, newPos);
	}
}