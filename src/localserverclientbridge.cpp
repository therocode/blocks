#include "localserverclientbridge.h"

void LocalServerClientBridge::flush()
{
}
void LocalServerClientBridge::connect(LocalServerClientBridge* other)
{
    mOther = other;
}
