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
        bool getForwards() const;
        bool getBackwards() const;
        bool getLeft() const;
        bool getRight() const;
        bool isStill() const;
        int8_t getForwardBack() const;
        int8_t getLeftRight() const;
    private:
        int8_t forwardBack;
        int8_t leftRight;
};
