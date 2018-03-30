#include "CoreTimer.h"
#include <stdio.h>


CoreTimer::CoreTimer() :
	countsPerSecond(0),
	secondsPerCount(0),
	mCurrTime(0),
	mPrevTime(0),
	mBaseTime(0),
	mPausedTime(0),
	mStopTime(0),
	deltaTime(0),
	frameCount(0),
	FPS(0)
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	secondsPerCount = SCALE_TIME / (double)countsPerSecond;

}

CoreTimer& CoreTimer::getInstance()
{
	static CoreTimer instance;

	return instance;
}

void CoreTimer::Reset()
{
	
}

void CoreTimer::Start()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrTime);
	mPrevTime = mCurrTime;
}

void CoreTimer::Stop()
{
}

void CoreTimer::Tick()
{
	++frameCount;

	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrTime);
	
	deltaTime = ((mCurrTime - mPrevTime) * secondsPerCount) / SCALE_TIME;
	if (deltaTime > 1.0f) {
		FPS = frameCount;
		frameCount = 0;
		mPrevTime = mCurrTime;
	}
}

void CoreTimer::ShowFrame(HWND hwnd)
{
	const int MAX_SIZE = 50;
	static char title[MAX_SIZE];

	char str[MAX_SIZE];
	static int titleId = GetWindowTextA(hwnd, title, MAX_SIZE);
	/*char str[50];*/
	sprintf_s(str, "%s FPS: %d", title, FPS);
	SetWindowTextA(hwnd, str);
}
