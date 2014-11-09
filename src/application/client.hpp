#pragma once
#include <fea/userinterface.hpp>
#include "../world/chunk.hpp"
#include "../networking/clientnetworkingsystem.hpp"
#include "../world/chunkmap.hpp"
#include "../utilities/lodepng.hpp"
#include "../utilities/logger.hpp"
#include "../utilities/fpscontroller.hpp"
#include "../world/worldmessages.hpp"
#include "../world/highlightmanager.hpp"


class RenderingSystem;
class InputAdaptor;

class Client :
    public fea::MessageReceiver<ClientActionMessage,
                                ClientChunksDeliveredMessage,
                                VoxelUpdatedMessage,
                                ClientChunkDeletedMessage,
                                CursorLockedMessage,
                                GameStartMessage,
                                ClientAttachedToEntityMessage,
                                ClientEnteredWorldMessage,
                                ClientPositionMessage,
                                MoveGfxEntityMessage,
                                RotateGfxEntityMessage>
{
    public:
        Client(fea::MessageBus& bus, const NetworkParameters& parameters);
        ~Client();
        bool loadTexture(const std::string& path, uint32_t width, uint32_t height, std::vector<unsigned char>& result);
        void update();
        void render();
        void handleMessage(const ClientActionMessage& received) override;
        void handleMessage(const ClientChunksDeliveredMessage& received) override;
        void handleMessage(const VoxelUpdatedMessage& received) override;
        void handleMessage(const ClientChunkDeletedMessage& received) override;
        void handleMessage(const CursorLockedMessage& received) override;
        void handleMessage(const GameStartMessage& received) override;
        void handleMessage(const ClientAttachedToEntityMessage& received) override;
        void handleMessage(const ClientEnteredWorldMessage& received) override;
        void handleMessage(const ClientPositionMessage& received) override;
        void handleMessage(const MoveGfxEntityMessage& received) override;
        void handleMessage(const RotateGfxEntityMessage& received) override;
        bool requestedQuit();
    private:
        int64_t mFrame = 0;
        uint64_t mFrameNumber;
        FPSController mFPSCounter;
        void updateChunk(const ChunkCoord& coordinate);
        void updateVoxelLookAt();
        bool mLockedMouse;
        fea::MessageBus& mBus;
        Logger mLogger;
        fea::Window mWindow;
        std::unique_ptr<RenderingSystem> mRenderingSystem;
        std::unique_ptr<InputAdaptor> mInputAdaptor;
        bool mQuit;

        std::string mCurrentWorld;
        fea::EntityId mCurrentEntity;
        uint32_t mHighlightRadius;
        HighlightManager mHighlightedChunks;
        ChunkCoord mLastChunk;
        ChunkMap mLocalChunks;

        std::unique_ptr<ClientNetworkingSystem> mClientNetworkingSystem;

        float mPitch;
        float mYaw;
        glm::vec3 mPosition;
};
