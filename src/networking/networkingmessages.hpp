#pragma once
#include <fea/util.hpp>
#include <functional>
#include <memory>

class LocalServerClientBridge;
class BasePackage;

struct LocalConnectionAttemptMessage
{
    fea::MessageBus* clientBus;
};

struct LocalConnectionEstablishedMessage
{
    fea::MessageBus* serverBus;
};
