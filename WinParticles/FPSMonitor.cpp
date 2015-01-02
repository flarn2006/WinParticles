#include "stdafx.h"
#include "FPSMonitor.h"

CFPSMonitor::CFPSMonitor()
{
	lastTickCount = GetTickCount64();
}

void CFPSMonitor::NewFrame()
{
	ULONGLONG tickCount = GetTickCount64();
	unsigned int difference = (unsigned int)(tickCount - lastTickCount);
	lastDifferences.push_back(difference);
	if (lastDifferences.size() > AVERAGE_OVER_N_FRAMES) lastDifferences.pop_front();
	int sum = 0;
	for (std::deque<unsigned >::iterator i = lastDifferences.begin(); i != lastDifferences.end(); i++) {
		sum += *i;
	}
	fps = 1000.0 / ((double)sum / lastDifferences.size());
	lastTickCount = tickCount;
}

double CFPSMonitor::GetFPS()
{
	return fps;
}

unsigned int CFPSMonitor::GetLastFrameTime()
{
	return lastDifferences.back();
}