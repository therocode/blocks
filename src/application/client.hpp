#pragma once
#include <fea/userinterface.hpp>
#include "../input/inputmessages.hpp"
#include "../entity/entitymessages.hpp"
#include "../rendering/renderingmessages.hpp"
#include "../script/scriptmessages.hpp"
#include "../world/chunk.hpp"

#include "../utilities/lodepng.hpp"
#include "../utilities/logger.hpp"
#include "applicationmessages.hpp"

#include "../utilities/fpscontroller.hpp"

class Renderer;
class InputAdaptor;
class ServerClientBridge;

class Client :
    public fea::MessageReceiver<PlayerActionMessage,
                                PlayerMoveDirectionMessage,
                                PlayerMoveActionMessage,
                                PlayerPitchYawMessage,
                                RebuildScriptsRequestedMessage,
                                WindowFocusLostMessage,
                                WindowInputMessage>
{
    public:
        Client(fea::MessageBus& bus);
        ~Client();
        bool loadTexture(const std::string& path, uint32_t width, uint32_t height, std::vector<unsigned char>& result);
        void setup();
        void handleInput();
        void render();
        virtual void handleMessage(const PlayerActionMessage& received);
        virtual void handleMessage(const PlayerMoveDirectionMessage& received);
        virtual void handleMessage(const PlayerMoveActionMessage& received);
        virtual void handleMessage(const PlayerPitchYawMessage& received);
        virtual void handleMessage(const RebuildScriptsRequestedMessage& received);
        virtual void handleMessage(const WindowFocusLostMessage& received);
        virtual void handleMessage(const WindowInputMessage& received);
        bool requestedQuit();
        void setServerBridge(std::unique_ptr<ServerClientBridge> bridge);
        fea::MessageBus& getBus();
    private:
        long mFrame = 0;
        FPSController mFPSCounter;
        void fetchServerData();
        void updateChunk(const ChunkCoord& coordinate);
        bool mLockedMouse;
        fea::MessageBus& mBus;
        Logger mLogger;
        fea::Window mWindow;
        std::unique_ptr<Renderer> mRenderer;
        std::unique_ptr<InputAdaptor> mInputAdaptor;
        bool mQuit;
        std::string mLogName;

        ChunkMap mLocalChunks;

        std::unique_ptr<ServerClientBridge> mBridge;
};
