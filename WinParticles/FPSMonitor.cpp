#include "stdafx.h"
#include "FPSMonitor.h"

CFPSMonitor::CFPSMonitor()
{
	lastTickCount = GetTickCount64();
}

void CFPSMonitor::NewFrame()
{
	ULONGLONG tickCount = GetTickCount64();
	int difference = (int)(tickCount - lastTickCount);
	lastDifferences.push_back(difference);
	if (lastDifferences.size() > AVERAGE_OVER_N_FRAMES) lastDifferences.pop_front();
	int sum = 0;
	for (std::deque<int>::iterator i = lastDifferences.begin(); i != lastDifferences.end(); i++) {
		sum += *i;
	}
	fps = 1000.0f / ((float)sum / lastDifferences.size());
	lastTickCount = tickCount;
}

float CFPSMonitor::GetFPS()
{
	return fps;
}