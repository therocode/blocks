#include "scripthandler.hpp"
#include "scriptmessages.hpp"
#include "../utilities/folderexploder.hpp"
#include "scriptentitycore.hpp"
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

ScriptHandler::ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface) : 
    mEngine(bus),
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mWorldInterface(worldInterface),
    logName("script")
{
    subscribe(mBus, *this);

    ScriptEntityCore::sWorldInterface = &worldInterface;
    ScriptEntityCore::sBus = &bus;
}

ScriptHandler::~ScriptHandler()
{
    scriptEntities.clear();
    mEngine.destroyModule(mScripts);
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

    mCallers.push_back(std::unique_ptr<ScriptCaller>(new FrameTimeCaller(mBus, mEngine, scriptEntities)));
    mCallers.push_back(std::unique_ptr<ScriptCaller>(new GameEventCaller(mBus, mEngine, scriptEntities)));
    mCallers.push_back(std::unique_ptr<ScriptCaller>(new OnGroundCaller(mBus, mEngine, scriptEntities)));

    registerInterface();

    FolderExploder exploder;

    sourceFiles.clear();

    exploder.explodeFolder("data", ".*\\.as", sourceFiles);
    for(auto& string : sourceFiles)
    {
        mBus.send<LogMessage>(LogMessage{"Adding " + string + " for compilation.", logName, LogLevel::VERB});
    }

    mBus.send<LogMessage>(LogMessage{"Compiling scripts...", logName, LogLevel::INFO});
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.send<LogMessage>(LogMessage{"Compilation process over.", logName, LogLevel::INFO});

    if(succeeded)
    {
        mBus.send<LogMessage>(LogMessage{"Setting up script callbacks...", logName, LogLevel::VERB});
        registerCallbacks(scriptEntities);
        mBus.send<LogMessage>(LogMessage{"Compilation process over.", logName, LogLevel::VERB});
        mBus.send<LogMessage>(LogMessage{"Done setting up callbacks.", logName, LogLevel::VERB});

        for(auto& caller : mCallers)
        {
            caller->setActive(true);
        }
    }
}

void ScriptHandler::handleMessage(const RebuildScriptsRequestedMessage& received)
{
    mBus.send<LogMessage>(LogMessage{"Compiling scripts...", logName, LogLevel::INFO});
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.send<LogMessage>(LogMessage{"Compilation process over.", logName, LogLevel::INFO});

    if(succeeded)
    {
        mBus.send<LogMessage>(LogMessage{"Setting up script callbacks...", logName, LogLevel::VERB});
        registerCallbacks(scriptEntities);
        mBus.send<LogMessage>(LogMessage{"Compilation process over.", logName, LogLevel::VERB});
        mBus.send<LogMessage>(LogMessage{"Done setting up callbacks.", logName, LogLevel::VERB});
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

void ScriptHandler::handleMessage(const EntityCreatedMessage& received)
{
    fea::WeakEntityPtr wEntity = received.entity;
    std::string type = received.type;
    
    asIObjectType* objectType = mScripts.getObjectTypeByDecl(type);
    if(!objectType)
    {
        mBus.send<LogMessage>(LogMessage{"Script runtime error: Trying to create entity of invalid type '" + type + "'", logName, LogLevel::ERR});
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

        mBus.send<LogMessage>(LogMessage{"Created entity id " + std::to_string(id) + " of type '" + type + "'", logName, LogLevel::VERB});

        mEngine.freeContext(ctx);

        ScriptEntity scriptEntity(id, wEntity, obj);
        scriptEntities.emplace(id, scriptEntity);
    }
    else
    {
        mBus.send<LogMessage>(LogMessage{"Script runtime error: Entity of type '" + type + "' has no valid factory function", logName, LogLevel::ERR});
    }
}

void ScriptHandler::handleMessage(const EntityRemovedMessage& received)
{
    size_t id = received.id;

    auto entity = scriptEntities.find(id);

    if(entity != scriptEntities.end())
    {
        scriptEntities.erase(id);
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
    }
}
