#pragma once
#include <fea/userinterface.hpp>
#include "../world/chunk.hpp"
#include "../networking/clientnetworkingsystem.hpp"
#include "../utilities/logger.hpp"
#include "../utilities/fpscontroller.hpp"
#include "../world/worldmessages.hpp"
#include "../world/highlightmanager.hpp"
#include "../client/clientworld.hpp"


class RenderingSystem;
class InputAdaptor;
class ResourceSystem;

class Client :
    public fea::MessageReceiver<ClientActionMessage,
                                CursorLockedMessage,
                                GameStartMessage,
                                LocalPlayerAttachedToEntityMessage,
                                //ClientEnteredWorldMessage,
                                //ClientPositionMessage,
                                ClientEntityMovedMessage,
                                MoveGfxEntityMessage,
                                OrientateGfxEntityMessage>
{
    public:
        Client(fea::MessageBus& bus, const NetworkParameters& parameters);
        ~Client();
        void update();
        void render();
        void handleMessage(const ClientActionMessage& received) override;
        void handleMessage(const CursorLockedMessage& received) override;
        void handleMessage(const GameStartMessage& received) override;
        void handleMessage(const LocalPlayerAttachedToEntityMessage& received) override;
        //void handleMessage(const ClientEnteredWorldMessage& received) override;
        //void handleMessage(const ClientPositionMessage& received) override;
        void handleMessage(const ClientEntityMovedMessage& received) override;
        void handleMessage(const MoveGfxEntityMessage& received) override;
        void handleMessage(const OrientateGfxEntityMessage& received) override;
        bool requestedQuit();
    private:
        int64_t mFrame = 0;
        uint64_t mFrameNumber;
        FPSController mFPSCounter;
        void updateVoxelLookAt();
        bool mLockedMouse;
        fea::MessageBus& mBus;
        Logger mLogger;
        fea::Window mWindow;
        std::unique_ptr<RenderingSystem> mRenderingSystem;
        std::unique_ptr<InputAdaptor> mInputAdaptor;
        std::unique_ptr<ResourceSystem> mResourceSystem;
        bool mQuit;

        ClientWorld mClientWorld;
        WorldId mCurrentWorld;
        fea::EntityId mCurrentEntity;
        uint32_t mHighlightRadius;
        ChunkCoord mLastChunk;

        std::unique_ptr<ClientNetworkingSystem> mClientNetworkingSystem;

        glm::quat mOrientation;
        glm::vec3 mPosition;
};
