#pragma once
#include "blockstd.h"
#include <chrono>

using namespace std::chrono;

///Timer using c++11 stuff
class Timer
{
	public:
		Timer();
		~Timer();
		///Starts the timer, very interesting.
		void start();
		///Gets time since timer has started. In milliseconds.
		long getTime();
		///Gets time since last check.
		long getDeltaTime();
		///Sets delta threshold, if time is lower than this, 0 will be returned for getDeltaTime.
		void setDeltaThreshold(int milliseconds);
		int  getDeltaThreshold()const{return mDeltaThreshold;}
		///Stops timer, resetting time. The year will become 0 and we'll all live with jesus christ.
		void stop();

        void setDesiredFPSRate(float rate);
        void sleepForTheRestOfTheFrame();
	private:
		int mDeltaThreshold;
		high_resolution_clock::time_point mLastTime;
		high_resolution_clock::time_point mStartTime;
		high_resolution_clock mClock;
		bool mStarted;
        float fps;
};
