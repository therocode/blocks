#include "fpscontroller.h"

FPSController::FPSController(){
    reset();
}

void FPSController::setMaxFPS(int fps){
    mFrameRate = fps;
    if(mFrameRate != 0)
    mFrameLength = std::chrono::microseconds(1000000) / mFrameRate;
}

float FPSController::getAverageFPS(){
    return mFPS;
}

void FPSController::frameBegin(){
    mFrameBeginTime = mTimer.getTimePoint();
}

void FPSController::frameEnd(){
    if(mFrameRate == 0){
        return;
    }
    std::chrono::microseconds microsecondsElapsed = duration_cast<microseconds>(mTimer.getTimePoint() - mFrameBeginTime);
    mFPS = 1000000.f / (float)(microsecondsElapsed.count());
    std::chrono::microseconds microsecondsLeft =  mFrameLength - microsecondsElapsed;
    if(microsecondsLeft > std::chrono::microseconds(0)){
        std::this_thread::sleep_for(microsecondsLeft);
    }
}

void FPSController::reset(){
    mTimer.stop();
    mTimer.start();
}

