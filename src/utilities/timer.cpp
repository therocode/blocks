#include "timer.h"
#include <thread>

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
	high_resolution_clock::time_point newTime = mClock.now();
	return duration_cast<milliseconds>(newTime - mStartTime).count();
}

long Timer::getDeltaTime()
{
	if(!mStarted)
	{
		return 0;
	}
	high_resolution_clock::time_point newTime = mClock.now();
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

void Timer::setDesiredFPSRate(int rate)
{
    fps = rate;
}

void Timer::sleepForTheRestOfTheFrame()
{
    std::chrono::microseconds frameLength = std::chrono::microseconds(1000000) / fps;
    std::chrono::microseconds microsecondsElapsed = duration_cast<microseconds>(mClock.now() - mLastTime);
    std::chrono::microseconds microsecondsLeft =  frameLength - microsecondsElapsed;

    if(microsecondsLeft > std::chrono::microseconds(0))
    {
        std::this_thread::sleep_for(microsecondsLeft);
    }
}
