#pragma once
#include <stdint.h>

class MoveDirection
{
    public:
        MoveDirection();
        MoveDirection(int8_t forwardBack, int8_t leftRight);
        MoveDirection(bool forwards, bool backwards, bool left, bool right);
        void setDirection(bool forwards, bool backwards, bool left, bool right);
        void setForwards(bool forwards);
        void setBackwards(bool backwards);
        void setLeft(bool left);
        void setRight(bool right);
        bool getForwards();
        bool getBackwards();
        bool getLeft();
        bool getRight();
        bool isStill();
        int8_t getForwardBack();
        int8_t getLeftRight();
    private:
        int8_t forwardBack;
        int8_t leftRight;
};
