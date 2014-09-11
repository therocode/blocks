#include "movedirection.hpp"

MoveDirection::MoveDirection()
{
}

MoveDirection::MoveDirection(int8_t fb, int8_t lr)
{
    forwardBack = fb;
    leftRight = lr;
}

MoveDirection::MoveDirection(bool forwards, bool backwards, bool left, bool right)
{
    setDirection(forwards, backwards, left, right);
}

void MoveDirection::setDirection(bool forwards, bool backwards, bool left, bool right)
{
    forwardBack = 0;
    leftRight = 0;

    if(forwards)
        forwardBack++;
    if(backwards)
        forwardBack--;
    if(left)
        leftRight++;
    if(right)
        leftRight--;
}

bool MoveDirection::getForwards() const
{
    return forwardBack == 1;
}

bool MoveDirection::getBackwards() const
{
    return forwardBack == -1;
}

bool MoveDirection::getLeft() const
{
    return leftRight == 1;
}

bool MoveDirection::getRight() const
{
    return leftRight == -1;
}

bool MoveDirection::isStill() const
{
    return !forwardBack && !leftRight;
}

int8_t MoveDirection::getForwardBack() const
{
    return forwardBack;
}

int8_t MoveDirection::getLeftRight() const
{
    return leftRight;
}
