#include "movedirection.h"

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

bool MoveDirection::getForwards()
{
    return forwardBack = 1;
}

bool MoveDirection::getBackwards()
{
    return forwardBack = -1;
}

bool MoveDirection::getLeft()
{
    return leftRight = 1;
}

bool MoveDirection::getRight()
{
    return leftRight = -1;
}

bool MoveDirection::isStill()
{
    return !forwardBack && !leftRight;
}

int8_t MoveDirection::getForwardBack()
{
    return forwardBack;
}

int8_t MoveDirection::getLeftRight()
{
    return leftRight;
}
