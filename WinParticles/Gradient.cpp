#include "stdafx.h"
#include "Gradient.h"
#include "Common.h"

CGradient::CGradient()
{
	steps = new std::vector<Step>();
}

CGradient::CGradient(const CGradient &other)
{
	steps = new std::vector<Step>();
	for (std::vector<Step>::iterator i = other.steps->begin(); i != other.steps->end(); i++) {
		steps->push_back(*i);
	}
	PrecalculateColors();
}

CGradient::CGradient(int numSteps)
{
	steps = new std::vector<Step>();
	for (int i = 0; i < numSteps; i++) {
		AddStep(0.0, 0);
	}
	for (int i = 0; i < PRECALC_STEPS; i++) {
		precalc[i] = 0;
	}
	usePrecalc = true;
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
	usePrecalc = false;
	steps->at(index).color = color;
}

void CGradient::SetStepPosition(int index, double position)
{
	usePrecalc = false;
	steps->at(index).position = position;
}

void CGradient::SetStep(int index, double position, COLORREF color)
{
	SetStepColor(index, color);
	SetStepPosition(index, position);
}

unsigned int CGradient::AddStep(double position, COLORREF color)
{
	usePrecalc = false;
	steps->push_back({ color, position });
	return steps->size() - 1;
}

void CGradient::DeleteStep(int index)
{
	usePrecalc = false;
	std::vector<Step> *oldSteps = steps;
	steps = new std::vector<Step>();
	for (unsigned int i = 0; i < oldSteps->size(); i++) {
		if (i != index) {
			steps->push_back(oldSteps->at(i));
		}
	}
}

void CGradient::DeleteAllSteps()
{
	usePrecalc = false;
	steps->clear();
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