#include "client.h"
#include <iostream>
#include <featherkit/util/window/sfml/sfmlwindowbackend.h>
#include <featherkit/util/input/sfml/sfmlinputbackend.h>
#include <typeinfo>
#include "../networking/packages.h"
#include "../application/applicationmessages.h"

Client::Client() : mWindow(new fea::util::SFMLWindowBackend(mSfWindow)),
                   mRenderer(mBus),
                   mInputAdaptor(mSfWindow, mBus),
                   mQuit(false),
                   mLogName("client"),
                   mBridge(nullptr)
{
	mBus.addMessageSubscriber<PlayerActionMessage>(*this);
	mBus.addMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
}

Client::~Client()
{
    mBus.removeMessageSubscriber<PlayerActionMessage>(*this);
    mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
}

void Client::setup()
{
    mWindow.create(fea::VideoMode(800, 600, 32), "Blocky", fea::Style::Default, fea::ContextSettings(32));

//	mWindow.setFramerateLimit(30);
    mRenderer.setup();
    mBus.sendMessage<WindowResizeMessage>(WindowResizeMessage(800, 600));
	// mWindow.lockCursor(true);

//    std::cout << "client setup\n";
}

void Client::handleInput()
{
    if(mBridge)
    {
        fetchServerData();
    }

    mInputAdaptor.update();

    if(mBridge)
    {
        mBridge->flush();
    }
}

void Client::render()
{
    mRenderer.cameraUpdate();
    mRenderer.render();
    mWindow.swapBuffers();
}

void Client::destroy()
{
    mWindow.close();
    mBus.sendMessage<LogMessage>(LogMessage("client destroyed", mLogName));
}

void Client::handleMessage(const PlayerActionMessage& received)
{
    size_t playerId;
    InputAction action;
	std::tie(playerId, action) = received.data;

    if(action == InputAction::QUIT)
    {
        mQuit = true;
    }
    else
    {
        mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerActionPackage(playerId, action)));
    }
}

void Client::handleMessage(const RebuildScriptsRequestedMessage& received)
{
    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new RebuildScriptsRequestedPackage('0')));
}

bool Client::requestedQuit()
{
    return mQuit;
}

void Client::setServerBridge(std::unique_ptr<ServerClientBridge> bridge)
{
    mBridge = std::move(bridge);
    mBus.sendMessage<LogMessage>(LogMessage("client connected to server", mLogName));
}

void Client::fetchServerData()
{
    std::shared_ptr<BasePackage> package;

    while(mBridge->pollPackage(package))
    {
        if(package->mType == typeid(ChunkLoadedPackage))
        {
            ChunkLoadedPackage* chunkPackage = (ChunkLoadedPackage*)package.get();
            ChunkCoordinate coordinate;
            VoxelTypeArray types;
            std::tie(coordinate, types) = chunkPackage->getData();
            mBus.sendMessage<ChunkCreatedMessage>(ChunkCreatedMessage(coordinate, types));
        }
        else if(package->mType == typeid(GfxEntityAddedPackage))
        {
            GfxEntityAddedPackage* gfxAddedPackage = (GfxEntityAddedPackage*)package.get();

            size_t id;
            glm::vec3 position;

            std::tie(id, position) = gfxAddedPackage->getData();
            mBus.sendMessage<AddGfxEntityMessage>(AddGfxEntityMessage(id, position));
        }
        else if(package->mType == typeid(GfxEntityMovedPackage))
        {
            GfxEntityMovedPackage* gfxMovedPackage = (GfxEntityMovedPackage*)package.get();

            size_t id;
            glm::vec3 position;

            std::tie(id, position) = gfxMovedPackage->getData();

            mBus.sendMessage<MoveGfxEntityMessage>(MoveGfxEntityMessage(id, position));
        }
        else if(package->mType == typeid(GfxEntityRemovedPackage))
        {
            GfxEntityRemovedPackage* gfxRemovedPackage = (GfxEntityRemovedPackage*)package.get();

            size_t id;

            std::tie(id) = gfxRemovedPackage->getData();

            mBus.sendMessage<RemoveGfxEntityMessage>(RemoveGfxEntityMessage(id));
        }
    }
}