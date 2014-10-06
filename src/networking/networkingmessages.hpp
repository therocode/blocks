#pragma once
#include <fea/util.hpp>

struct LocalConnectionAttemptMessage
{
    fea::MessageBus* clientBus;
};

struct LocalConnectionEstablishedMessage
{
    fea::MessageBus* serverBus;
};

struct LocalDisconnectionMessage
{

};
