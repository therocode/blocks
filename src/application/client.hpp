#pragma once
#include <fea/userinterface.hpp>
#include "../world/chunk.hpp"
#include "../networking/clientnetworkingsystem.hpp"

#include "../utilities/lodepng.hpp"
#include "../utilities/logger.hpp"

#include "../utilities/fpscontroller.hpp"

#include "../world/worldmessages.hpp"


class Renderer;
class InputAdaptor;
class ServerClientBridge;

class Client :
    public fea::MessageReceiver<PlayerActionMessage,
                                ChunkLoadedMessage,
                                VoxelSetMessage,
                                ClientChunkDeletedMessage,
                                CursorLockedMessage>
{
    public:
        Client(fea::MessageBus& bus, const NetworkParameters& parameters);
        ~Client();
        bool loadTexture(const std::string& path, uint32_t width, uint32_t height, std::vector<unsigned char>& result);
        void update();
        void render();
        void handleMessage(const PlayerActionMessage& received) override;
        void handleMessage(const ChunkLoadedMessage& received) override;
        void handleMessage(const VoxelSetMessage& received) override;
        void handleMessage(const ClientChunkDeletedMessage& received) override;
        void handleMessage(const CursorLockedMessage& received) override;
        bool requestedQuit();
        void setServerBridge(std::unique_ptr<ServerClientBridge> bridge);
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

        ChunkMap mLocalChunks;

        ClientNetworkingSystem mClientNetworkingSystem;
};
