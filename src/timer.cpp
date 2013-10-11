#include "timer.h"

Timer::Timer()
{
	mDeltaThreshold = 0;
}

Timer::~Timer()
{

}

void Timer::start()
{
	mStartTime = mLastTime = mClock.now();
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
	system_clock::time_point newTime = mClock.now();
	return duration_cast<milliseconds>(newTime - mStartTime).count();
}

long Timer::getDeltaTime()
{
	if(!mStarted)
	{
		return 0;
	}
	system_clock::time_point newTime = mClock.now();
	long deltaTime = duration_cast<milliseconds>(newTime - mLastTime).count();
	if(deltaTime > mDeltaThreshold)
	{
		mLastTime = newTime;
	}else
	{
		deltaTime = 0;
	}
	return deltaTime;
}

void Timer::setDeltaThreshold(int milliseconds)
{
	mDeltaThreshold = milliseconds;	
}
