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

struct IncomingConnectionMessage
{
    uint32_t clientId;
};

struct ClientPackageReceived
{
    uint32_t clientId;
    const std::unique_ptr<BasePackage>& package;
};

struct ClientDisconnectedMessage
{
    uint32_t clientId;
};
