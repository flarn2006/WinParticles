#pragma once
#include <Windows.h>
#include <deque>

class CFPSMonitor
{
private:
	const int AVERAGE_OVER_N_FRAMES = 10;
	ULONGLONG lastTickCount;
	std::deque<int> lastDifferences;
	float fps;

public:
	CFPSMonitor();
	void NewFrame();
	float GetFPS();
};

