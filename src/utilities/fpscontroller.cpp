#include "fpscontroller.hpp"

FPSController::FPSController(){
    mSampleTime = std::chrono::milliseconds(1000);
    reset();
}

void FPSController::setSampleTime(float seconds){
    mSampleTime = std::chrono::milliseconds((int)(seconds * 1000.f));
}

void FPSController::setMaxFPS(int fps){
    mFrameRate = fps;
    if(mFrameRate != 0)
    mFrameLength = std::chrono::microseconds(1000000) / mFrameRate;
}

float FPSController::getAverageFPS(){
    return mFPS / ((float)mSampleTime.count() / 1000.0f);
}

void FPSController::frameBegin(){
    mFrameBeginTime = mTimer.getTimePoint();
    mBeginFrameCalled = true;
}

void FPSController::frameEnd(){
    high_resolution_clock::time_point currentTime = mTimer.getTimePoint();

    high_resolution_clock::time_point lastTime;

    if(mBeginFrameCalled){
        lastTime = mFrameBeginTime;
        mBeginFrameCalled = false;
    }else{
        lastTime = mLastFrameEnd;
    }

    mLastFrameEnd = currentTime;
    std::chrono::microseconds microsecondsElapsed = duration_cast<microseconds>(currentTime - lastTime);
    mFramesElapsed ++;

    if(currentTime - mSampleBegin > mSampleTime){
        mSampleBegin = currentTime;
        mFPS = mFramesElapsed;
        mFramesElapsed = 0;
    }

    if(mFrameRate == 0){
        return;
    }

    std::chrono::microseconds microsecondsLeft = mFrameLength - microsecondsElapsed;
    if(microsecondsLeft > std::chrono::microseconds(0)){
        std::this_thread::sleep_for(microsecondsLeft);
    }
}

void FPSController::reset(){
    mTimer.stop();
    mTimer.start();
    mSampleBegin = mTimer.getTimePoint();
}

