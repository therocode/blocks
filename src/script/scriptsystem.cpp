#include "scriptsystem.hpp"
#include "../utilities/folderexploder.hpp"
#include "../gameinterface.hpp"
#include "../lognames.hpp"
#include "scriptentitycore.hpp"
#include "scriptmessages.hpp"

#include "interfaces/chunkinterface.hpp"
#include "interfaces/entityinterface.hpp"
#include "interfaces/extensioninterface.hpp"
#include "interfaces/worldinterface.hpp"
#include "interfaces/mathsinterface.hpp"
#include "interfaces/physicsinterface.hpp"
#include "interfaces/printinterface.hpp"
#include "interfaces/randominterface.hpp"
#include "interfaces/stringinterface.hpp"
#include "interfaces/animationinterface.hpp"

#include "callers/ongroundcaller.hpp"
#include "callers/gameeventcaller.hpp"
#include "callers/frametimecaller.hpp"
#include "callers/chunkeventcaller.hpp"
#include "callers/worldcaller.hpp"

ScriptSystem::ScriptSystem(fea::MessageBus& bus, GameInterface& worldInterface) : 
    mBus(bus),
    mGameInterface(worldInterface),
    mEngine(bus),
    mScripts(mEngine.createModule("scripts"))
{
    subscribe(mBus, *this);
    mBus.send(LogMessage{"Setting up script system", gScriptName, LogLevel::INFO});
    mBus.send(LogMessage{"Setting up interfaces", gScriptName, LogLevel::VERB});
    setupInterfaces();
    mBus.send(LogMessage{"Setting up callbacks", gScriptName, LogLevel::VERB});
    setupCallbacks();
    mBus.send(LogMessage{"Loading script sources", gScriptName, LogLevel::VERB});
    loadSources();
    mBus.send(LogMessage{"Instantiating extension classes", gScriptName, LogLevel::VERB});
    instantiateExtensions();
}

ScriptSystem::~ScriptSystem()
{
    mBus.send(LogMessage{"Shutting down script system", gScriptName, LogLevel::INFO});

    for(asIScriptObject* extension : mExtensions)
    {
        extension->Release();
    }

    mScriptEntities.clear();
    mEngine.destroyModule(mScripts);
}

void ScriptSystem::handleMessage(const RebuildScriptsRequestedMessage& received)
{
    mBus.send(LogMessage{"Compiling scripts...", gScriptName, LogLevel::INFO});
    bool succeeded = mScripts.compileFromSourceList(mSourceFiles);

    if(succeeded)
    {
        mBus.send(LogMessage{"Compilation process over.", gScriptName, LogLevel::INFO});
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
    const std::string& type = received.type;
    
    asIObjectType* objectType = mScripts.getObjectTypeByDecl(type);
    if(!objectType)
    {
        mBus.send(LogMessage{"Script runtime error: Trying to create entity of invalid type '" + type + "'", gScriptName, LogLevel::ERR});
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

        //mBus.send(LogMessage{"Created entity id " + std::to_string(id) + " of type '" + type + "'", gScriptName, LogLevel::VERB});

        mEngine.freeContext(ctx);

        ScriptEntity scriptEntity(id, wEntity, obj);
        mScriptEntities.emplace(id, scriptEntity);
    }
    else
    {
        mBus.send(LogMessage{"Script runtime error: Entity of type '" + type + "' has no valid factory function", gScriptName, LogLevel::ERR});
    }
}

void ScriptSystem::handleMessage(const EntityRemovedMessage& received)
{
    fea::EntityId id = received.id;

    auto entity = mScriptEntities.find(id);

    if(entity != mScriptEntities.end())
    {
        mScriptEntities.erase(id);
    }
}

void ScriptSystem::setupInterfaces()
{
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new MathsInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new StringInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new EntityInterface(mBus, mGameInterface, mScriptEntities)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new WorldInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new PhysicsInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new PrintInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new RandomInterface(mBus, mGameInterface)));
	mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new ChunkInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new ExtensionInterface(mBus, mGameInterface)));
    mInterfaces.push_back(std::unique_ptr<ScriptInterface>(new AnimationInterface(mBus, mGameInterface)));

    registerInterfaces();
}

void ScriptSystem::registerInterfaces()
{
    for(auto& interface : mInterfaces)
    {
        interface->registerInterface(mEngine.getEngine());
    }
}

void ScriptSystem::setupCallbacks()
{
    mCallers.push_back(std::unique_ptr<ScriptCaller>(new FrameTimeCaller(mBus, mEngine, mScriptEntities, mExtensions)));
    mCallers.push_back(std::unique_ptr<ScriptCaller>(new GameEventCaller(mBus, mEngine, mScriptEntities, mExtensions)));
    mCallers.push_back(std::unique_ptr<ScriptCaller>(new OnGroundCaller(mBus, mEngine, mScriptEntities, mExtensions)));
	mCallers.push_back(std::unique_ptr<ScriptCaller>(new ChunkEventCaller(mBus, mEngine, mScriptEntities, mExtensions)));
	mCallers.push_back(std::unique_ptr<ScriptCaller>(new WorldCaller(mBus, mEngine, mScriptEntities, mExtensions)));
}

void ScriptSystem::loadSources()
{
    FolderExploder exploder;

    mSourceFiles.clear();

    exploder.explodeFolder("data/scripts", {"as"}, mSourceFiles);
    for(auto& string : mSourceFiles)
    {
        mBus.send(LogMessage{"Adding " + string + " for compilation.", gScriptName, LogLevel::VERB});
    }

    mBus.send(LogMessage{"Compiling scripts...", gScriptName, LogLevel::INFO});
    bool succeeded = mScripts.compileFromSourceList(mSourceFiles);
    mBus.send(LogMessage{"Compilation process over.", gScriptName, LogLevel::INFO});

    if(succeeded)
    {
        mBus.send(LogMessage{"Compilation process over.", gScriptName, LogLevel::VERB});
        mBus.send(LogMessage{"Done setting up callbacks.", gScriptName, LogLevel::VERB});

        for(auto& caller : mCallers)
        {
            caller->setActive(true);
        }
    }
}

void ScriptSystem::instantiateExtensions() 
{
    asIObjectType* extensionInterface = mEngine.getEngine()->GetObjectTypeByName("IExtension");

    for(asIObjectType* type : mScripts.getObjectTypes())
    {
        mBus.send(LogMessage{"Checking type " + std::string(type->GetName()) + "...", gScriptName, LogLevel::VERB});

        if(type->Implements(extensionInterface))
        {
            mBus.send(LogMessage{std::string(type->GetName()) + " is an extension class, instantiating...", gScriptName, LogLevel::VERB});

            asIScriptObject *extension = (asIScriptObject*)mEngine.getEngine()->CreateScriptObject(type);
            mExtensions.push_back(extension);
        }
    }
}
