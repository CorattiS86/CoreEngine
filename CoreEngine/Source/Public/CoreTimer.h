#pragma once

#include <Windows.h>
#include "CoreWindow.h"

class CoreTimer
{	
	CoreTimer();
public:

	static CoreTimer& getInstance();

	void Reset();
	void Start();
	void Stop();
	void Tick();
	void ShowFrame(HWND hwnd);

private:

	const double SCALE_TIME = 1000.0;

	__int64 countsPerSecond;
	double secondsPerCount;

	__int64 mCurrTime;
	__int64 mPrevTime;
	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;

	double deltaTime;
	int frameCount;

	int FPS;
};

