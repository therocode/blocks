#include "scriptsystem.hpp"
#include "../utilities/folderexploder.hpp"
#include "../gameinterface.hpp"
#include "../lognames.hpp"
#include "scriptentitycore.hpp"
#include "scriptmessages.hpp"

#include "interfaces/entityinterface.hpp"
#include "interfaces/landscapeinterface.hpp"
#include "interfaces/mathsinterface.hpp"
#include "interfaces/physicsinterface.hpp"
#include "interfaces/printinterface.hpp"
#include "interfaces/randominterface.hpp"
#include "interfaces/stringinterface.hpp"

#include "callers/ongroundcaller.hpp"
#include "callers/gameeventcaller.hpp"
#include "callers/frametimecaller.hpp"

ScriptSystem::ScriptSystem(fea::MessageBus& bus, GameInterface& worldInterface) : 
    mBus(bus),
    mGameInterface(worldInterface),
    mEngine(bus),
    mScripts(mEngine.createModule("scripts"))
{
    subscribe(mBus, *this);
    mBus.send(LogMessage{"Setting up script system", scriptName, LogLevel::INFO});
}

ScriptSystem::~ScriptSystem()
{
    mBus.send(LogMessage{"Shutting down script system", scriptName, LogLevel::INFO});
    mScriptEntities.clear();
    mEngine.destroyModule(mScripts);
}

void ScriptSystem::setup()
{
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new MathsInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new StringInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new EntityInterface(mBus, mGameInterface, mScriptEntities)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new LandscapeInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new PhysicsInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new PrintInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new RandomInterface(mBus, mGameInterface)));

    mCallers.push_back(std::unique_ptr<ScriptCaller>(new FrameTimeCaller(mBus, mEngine, mScriptEntities)));
    mCallers.push_back(std::unique_ptr<ScriptCaller>(new GameEventCaller(mBus, mEngine, mScriptEntities)));
    mCallers.push_back(std::unique_ptr<ScriptCaller>(new OnGroundCaller(mBus, mEngine, mScriptEntities)));

    registerInterface();

    FolderExploder exploder;

    mSourceFiles.clear();

    exploder.explodeFolder("data", ".*\\.as", mSourceFiles);
    for(auto& string : mSourceFiles)
    {
        mBus.send(LogMessage{"Adding " + string + " for compilation.", scriptName, LogLevel::VERB});
    }

    mBus.send(LogMessage{"Compiling scripts...", scriptName, LogLevel::INFO});
    bool succeeded = mScripts.compileFromSourceList(mSourceFiles);
    mBus.send(LogMessage{"Compilation process over.", scriptName, LogLevel::INFO});

    if(succeeded)
    {
        mBus.send(LogMessage{"Compilation process over.", scriptName, LogLevel::VERB});
        mBus.send(LogMessage{"Done setting up callbacks.", scriptName, LogLevel::VERB});

        for(auto& caller : mCallers)
        {
            caller->setActive(true);
        }
    }
}

void ScriptSystem::handleMessage(const RebuildScriptsRequestedMessage& received)
{
    mBus.send(LogMessage{"Compiling scripts...", scriptName, LogLevel::INFO});
    bool succeeded = mScripts.compileFromSourceList(mSourceFiles);
    mBus.send(LogMessage{"Compilation process over.", scriptName, LogLevel::INFO});

    if(succeeded)
    {
        mBus.send(LogMessage{"Compilation process over.", scriptName, LogLevel::VERB});
        mBus.send(LogMessage{"Done setting up callbacks.", scriptName, LogLevel::VERB});
        for(auto& caller : mCallers)
        {
            caller->setActive(true);
        }
    }
    else
    {
        for(auto& caller : mCallers)
        {
            caller->setActive(false);
        }
    }
}

void ScriptSystem::handleMessage(const EntityCreatedMessage& received)
{
    fea::WeakEntityPtr wEntity = received.entity;
    std::string type = received.type;
    
    asIObjectType* objectType = mScripts.getObjectTypeByDecl(type);
    if(!objectType)
    {
        mBus.send(LogMessage{"Script runtime error: Trying to create entity of invalid type '" + type + "'", scriptName, LogLevel::ERR});
        return;
    }

    asIScriptFunction *factory = objectType->GetFactoryByDecl(std::string(type + " @" + type +"(EntityCore@ core, uint id)").c_str());

    if(factory)
    {
        fea::EntityId id = wEntity.lock()->getId();

        ScriptEntityCore* entityCore = new ScriptEntityCore(mBus, wEntity.lock(), id);

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

        mBus.send(LogMessage{"Created entity id " + std::to_string(id) + " of type '" + type + "'", scriptName, LogLevel::VERB});

        mEngine.freeContext(ctx);

        ScriptEntity scriptEntity(id, wEntity, obj);
        mScriptEntities.emplace(id, scriptEntity);
    }
    else
    {
        mBus.send(LogMessage{"Script runtime error: Entity of type '" + type + "' has no valid factory function", scriptName, LogLevel::ERR});
    }
}

void ScriptSystem::handleMessage(const EntityRemovedMessage& received)
{
    size_t id = received.id;

    auto entity = mScriptEntities.find(id);

    if(entity != mScriptEntities.end())
    {
        mScriptEntities.erase(id);
    }
}

void ScriptSystem::registerInterface()
{
    for(auto& interface : mInterfaces)
    {
        interface->registerInterface(mEngine.getEngine());
    }
}
