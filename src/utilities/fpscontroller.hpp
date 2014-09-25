#pragma once
#include "blockstd.hpp"
#include "timer.hpp"
#include <chrono>

using namespace std::chrono;
class FPSController{
public:
    FPSController();
    ///Set fps counter sample time
    void setSampleTime(float seconds);
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
    std::chrono::milliseconds mSampleTime;
    Timer mTimer;
    int   mFPS;
    int   mFramesElapsed;
    high_resolution_clock::time_point mSampleBegin;
    high_resolution_clock::time_point mFrameBeginTime;
    high_resolution_clock::time_point mLastFrameEnd;
    std::chrono::microseconds mFrameLength;
    int   mFrameRate = 0;
    bool  mBeginFrameCalled = false;
};
