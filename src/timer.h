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
		///Stops timer, resetting time. The year will become 0 and we'll all live with jesus christ.
		void stop();
	private:
		high_resolution_clock::time_point mLastTime;
		high_resolution_clock::time_point mStartTime;
		high_resolution_clock mClock;
		bool mStarted;
};
