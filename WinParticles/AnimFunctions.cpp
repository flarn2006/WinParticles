#include "stdafx.h"
#include "AnimFunctions.h"
#include "Common.h"
#include <cmath>

double AnimFunctions::Saw(double time)
{
	return time;
}

double AnimFunctions::Sine(double time)
{
	return (std::sin(2 * M_PI * time) + 1.0) / 2.0;
}

double AnimFunctions::Square(double time)
{
	return (time < 0.5) ? 1.0 : 0.0;
}

double AnimFunctions::Triangle(double time)
{
	return 2 * (time - ((time < 0.5) ? 0.0 : 0.5));
}

double AnimFunctions::Random(double time)
{
	return RandInRange(0.0, 1.0);
}