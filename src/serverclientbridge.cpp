#include "serverclientbridge.h"

void ServerClientBridge::enqueuePackage(std::unique_ptr<Package>&& package)
{
    mOutgoing.push_back(std::move(package));
}
