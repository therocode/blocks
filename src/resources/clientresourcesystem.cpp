#include "clientresourcesystem.hpp"
#include "iqmfromfileloader.hpp"
#include "resourcemessages.hpp"
#include "../utilities/folderexploder.hpp"
#include "../application/applicationmessages.hpp"
#include "../lognames.hpp"

ClientResourceSystem::ClientResourceSystem(fea::MessageBus& bus) :
    mBus(bus)
{
    mBus.send(LogMessage{"Scanning 'assets' for resources...", resourceName, LogLevel::INFO});

    FolderExploder exploder;

    std::vector<std::string> mResources;

    exploder.explodeFolder("assets", ".*\\.iqm|.*\\.blend", mResources);

    mBus.send(LogMessage{"Found " + std::to_string(mResources.size()) + " resources to load.", resourceName, LogLevel::INFO});

    for(auto& resourcePath : mResources)
    {
        std::string fileType = resourcePath.substr(resourcePath.find_last_of('.') + 1, std::string::npos);

        mResourceList[fileType].push_back(resourcePath);
    }

    loadModels(mResourceList["iqm"]);
}

void ClientResourceSystem::loadModels(const std::vector<std::string>& models)
{
    mBus.send(LogMessage{"Loading models. " + std::to_string(models.size()) + " models to load.", resourceName, LogLevel::INFO});
    for(const auto& path : models)
    {
        mBus.send(LogMessage{"Loading " + path + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<RawModel> model = mCache.access<IQMFromFileLoader>(path);
        if(model)
        {
            mBus.send(ModelDeliverMessage{model});
        }
    }
}
