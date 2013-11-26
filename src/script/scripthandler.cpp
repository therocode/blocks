#include "scripthandler.h"
#include "scriptmessages.h"
#include "../utilities/folderexploder.h"
#include "scriptentitycore.h"
#include "interfaces/entityinterface.h"
#include "interfaces/landscapeinterface.h"
#include "interfaces/mathsinterface.h"
#include "interfaces/physicsinterface.h"
#include "interfaces/printinterface.h"
#include "interfaces/randominterface.h"
#include "interfaces/stringinterface.h"

ScriptHandler::ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface) : 
    mEngine(bus),
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mWorldInterface(worldInterface),
    logName("script"),
    onFrameCallback(mEngine),
    gameStartCallback(mEngine),
    frameTick(0)
{
    mBus.addMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.addMessageSubscriber<EntityCreatedMessage>(*this);
    mBus.addMessageSubscriber<EntityRemovedMessage>(*this);

    mBus.addMessageSubscriber<FrameMessage>(*this);
    mBus.addMessageSubscriber<GameStartMessage>(*this);
    mBus.addMessageSubscriber<EntityOnGroundMessage>(*this);

    ScriptEntityCore::sWorldInterface = &worldInterface;
    ScriptEntityCore::sBus = &bus;
}

ScriptHandler::~ScriptHandler()
{
    mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.removeMessageSubscriber<EntityCreatedMessage>(*this);
    mBus.removeMessageSubscriber<EntityRemovedMessage>(*this);

    mBus.removeMessageSubscriber<FrameMessage>(*this);
    mBus.removeMessageSubscriber<GameStartMessage>(*this);
    mBus.removeMessageSubscriber<EntityOnGroundMessage>(*this);
}

void ScriptHandler::setup()
{
    mEngine.setup();

    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new MathsInterface(mBus, mWorldInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new StringInterface(mBus, mWorldInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new EntityInterface(mBus, mWorldInterface, scriptEntities)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new LandscapeInterface(mBus, mWorldInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new PhysicsInterface(mBus, mWorldInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new PrintInterface(mBus, mWorldInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new RandomInterface(mBus, mWorldInterface)));

    registerInterface();

    FolderExploder exploder;

    sourceFiles.clear();

    exploder.explodeFolder("data", ".*\\.as", sourceFiles);
    for(auto& string : sourceFiles)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Adding " + string + " for compilation.", logName, LogLevel::VERB));
    }

    mBus.sendMessage<LogMessage>(LogMessage("Compiling scripts...", logName, LogLevel::INFO));
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::INFO));

    if(succeeded)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Setting up script callbacks...", logName, LogLevel::VERB));
        registerCallbacks(scriptEntities);
        mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::VERB));
        mBus.sendMessage<LogMessage>(LogMessage("Done setting up callbacks.", logName, LogLevel::VERB));
    }
}

void ScriptHandler::destroy()
{
    scriptEntities.clear();
    mEngine.destroyModule(mScripts);
    mEngine.destroy();
}

void ScriptHandler::handleMessage(const RebuildScriptsRequestedMessage& message)
{
    mBus.sendMessage<LogMessage>(LogMessage("Compiling scripts...", logName, LogLevel::INFO));
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::INFO));

    if(succeeded)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Setting up script callbacks...", logName, LogLevel::VERB));
        registerCallbacks(scriptEntities);
        mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::VERB));
        mBus.sendMessage<LogMessage>(LogMessage("Done setting up callbacks.", logName, LogLevel::VERB));
    }
}

void ScriptHandler::handleMessage(const EntityCreatedMessage& message)
{
    fea::WeakEntityPtr wEntity;
    std::string type;
    
    std::tie(wEntity, type) = message.data;

    asIObjectType* objectType = mScripts.getObjectTypeByDecl(type);
    if(!objectType)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Script runtime error: Trying to create entity of invalid type '" + type + "'", logName, LogLevel::ERR));
        return;
    }

    asIScriptFunction *factory = objectType->GetFactoryByDecl(std::string(type + " @" + type +"(EntityCore@ core, uint id)").c_str());

    if(factory)
    {
        fea::EntityId id = wEntity.lock()->getId();

        ScriptEntityCore* entityCore = new ScriptEntityCore(id);

        asIScriptContext* ctx = mEngine.requestContext();
        // Prepare the context to call the factory function
        ctx->Prepare(factory);
        // Add an entity core
        ctx->SetArgObject(0, entityCore);
        ctx->SetArgDWord(1, id);
        // Execute the call
        ctx->Execute();
        // Get the object that was created
        asIScriptObject *obj = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
        // If you're going to store the object you must increase the reference,
        // otherwise it will be destroyed when the context is reused or destroyed.
        obj->AddRef();

        mBus.sendMessage<LogMessage>(LogMessage("Created entity id " + std::to_string(id) + " of type '" + type + "'", logName, LogLevel::VERB));

        mEngine.freeContext(ctx);

        ScriptEntity scriptEntity(id, wEntity, obj);
        scriptEntities.emplace(id, scriptEntity);
    }
    else
    {
        mBus.sendMessage<LogMessage>(LogMessage("Script runtime error: Entity of type '" + type + "' has no valid factory function", logName, LogLevel::ERR));
    }
}

void ScriptHandler::handleMessage(const EntityRemovedMessage& message)
{
    size_t id;
    std::tie(id) = message.data;

    auto entity = scriptEntities.find(id);

    if(entity != scriptEntities.end())
    {
        scriptEntities.erase(id);
    }
}

void ScriptHandler::handleMessage(const FrameMessage& received)
{
    if(!mScripts.hasErrors())
    {
        onFrameCallback.execute(frameTick);

        for(auto& entity : scriptEntities)
        {
            ScriptMemberCallback<int32_t> callback(mEngine);
            asIScriptObject* object = entity.second.getScriptObject();
            asIScriptFunction* function = object->GetObjectType()->GetMethodByDecl("void onFrame(int frameNumber)");
            if(function)
            {
                callback.setFunction(function);
                callback.execute(object, frameTick);
            }
        }

        frameTick++;
    }
}

void ScriptHandler::handleMessage(const GameStartMessage& received)
{
    if(!mScripts.hasErrors())
    {
        gameStartCallback.execute();
    }
}

void ScriptHandler::handleMessage(const EntityOnGroundMessage& received)
{
    bool landed;
    size_t id;

    std::tie(id, landed) = received.data;

    if(!mScripts.hasErrors())
    {
        for(auto& entity : scriptEntities)
        {
            if(id == entity.first)
            {
                ScriptMemberCallback<bool> callback(mEngine);
                asIScriptObject* object = entity.second.getScriptObject();
                asIScriptFunction* function = object->GetObjectType()->GetMethodByDecl("void onGround(bool landed)");
                if(function)
                {
                    callback.setFunction(function);
                    callback.execute(object, landed);
                }
            }
        }
    }
}

void ScriptHandler::registerInterface()
{
    for(auto& interface : mInterfaces)
    {
        interface->registerInterface(mEngine.getEngine());
    }
}

void ScriptHandler::registerCallbacks(ScriptEntityMap& scriptEntities)
{
    if(!mScripts.hasErrors())
    {
        onFrameCallback.setFunction(mScripts.getFunctionByDecl("void onFrame(int frameNumber)"));
        gameStartCallback.setFunction(mScripts.getFunctionByDecl("void gameStarted()"));
    }
}
