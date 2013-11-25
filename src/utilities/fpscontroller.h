#include "blockstd.h"
#include "timer.h"
#include <chrono>
#include <thread>

using namespace std::chrono;
class FPSController{
public:
    FPSController();
    ///Set max framerate.
    void setMaxFPS(int fps);
    ///Returns the average fps
    float getAverageFPS();
    ///Called at the beginning of a frame. Not required.
    void frameBegin();
    ///Called at the end of a frame. If desired framerate is set, it will wait the remaining time.
    void frameEnd();
    ///Resets timer, don't know when it would be good to run it.
    void reset();
private:
    Timer mTimer;
    float mFPS;
    high_resolution_clock::time_point mFrameBeginTime;
    std::chrono::microseconds mFrameLength;
    int   mFrameRate = 0;
};