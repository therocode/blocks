#pragma once
#include "scriptengine.h"
#include "scriptentity.h"
#include "scriptcallback.h"
#include "../world/worldinterface.h"
#include "../utilities/random.h"
#include "scriptmessages.h"
#include <map>
#include "../application/applicationmessages.h"
#include <unordered_map>

class ScriptHandler :
    public fea::MessageReceiver<RebuildScriptsRequestedMessage>,
    public fea::MessageReceiver<EntityCreatedMessage>,
    public fea::MessageReceiver<EntityRemovedMessage>,
    public fea::MessageReceiver<ScriptEntityFinishedMessage>,
    public fea::MessageReceiver<FrameMessage>,
    public fea::MessageReceiver<GameStartMessage>,
    public fea::MessageReceiver<EntityOnGroundMessage>
{
    public:
        ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~ScriptHandler();
        void setup();
        void destroy();
        void handleMessage(const RebuildScriptsRequestedMessage& message);
        void handleMessage(const EntityCreatedMessage& message);
        void handleMessage(const EntityRemovedMessage& message);
        void handleMessage(const ScriptEntityFinishedMessage& message);
        void handleMessage(const FrameMessage& received);
        void handleMessage(const GameStartMessage& received);
        void handleMessage(const EntityOnGroundMessage& received);
        void requestRemovalOfEntity(asIScriptObject* obj);
        asIScriptObject* createEntity(const std::string& type, float x, float y, float z);
        void removeEntity(asIScriptObject* entity);
        void setEntityCreator(EntityCreator creator);
    private:
        void registerInterface();
        void registerCallbacks(const std::map<size_t, ScriptEntity>& scriptEntities);
        fea::MessageBus& mBus;
        ScriptEngine mEngine;
        ScriptModule mScripts;
        WorldInterface& mWorldInterface;
        std::vector<std::string> sourceFiles;
        std::map<size_t, ScriptEntity> scriptEntities;
        std::unordered_map<asIScriptObject*, size_t> scriptEntityIds;
        std::string logName;
        Random random;

        //interface functions
        void scriptPrint(const std::string& text);
        void scriptPrint(const std::string& text, uint32_t level);
        void setGravity(float constant);
        void applyImpulse(size_t id, const glm::vec3& force);

        void setVoxelType(float x, float y, float z, uint16_t type);
        void setVoxelType(const glm::vec3& coordinate, uint16_t type);
        VoxelType getVoxelType(float x, float y, float z);
        VoxelType getVoxelType(const glm::vec3& coordinate);

        ScriptCallback<int32_t> onFrameCallback;
        ScriptCallback<> gameStartCallback;
        EntityCreator mEntityCreator;
        int32_t frameTick;
};
