#include "timer.h"

Timer::Timer()
{

}

Timer::~Timer()
{

}

void Timer::start()
{
	mStartTime = mLastTime = system_clock::now();
	mStarted = true;
}

void Timer::stop()
{
	mStarted = false;
}

long Timer::getTime()
{
	if(!mStarted)
	{
		return 0;
	}
	system_clock::time_point newTime = system_clock::now();
	return duration_cast<milliseconds>(newTime - mStartTime).count();
}

long Timer::getDeltaTime()
{
	if(!mStarted)
	{
		return 0;
	}
	system_clock::time_point newTime = system_clock::now();
	long deltaTime = duration_cast<milliseconds>(newTime - mLastTime).count();
	if(deltaTime > 0)
	{
		mLastTime = newTime;
	}
	return deltaTime;
}
