#pragma once
#include <Windows.h>
#include <deque>

class CFPSMonitor
{
private:
	const unsigned int AVERAGE_OVER_N_FRAMES = 10;
	ULONGLONG lastTickCount;
	std::deque<unsigned int> lastDifferences;
	double fps;

public:
	CFPSMonitor();
	void NewFrame();
	double GetFPS();
	unsigned int GetLastFrameTime();
};

