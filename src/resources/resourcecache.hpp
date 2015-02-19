#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <sstream>

class ResourceCache
{
    public:
        template<typename Loader, typename Accessor, typename... LoaderParameters>
        const std::shared_ptr<decltype(((Loader*)nullptr)->load(Accessor()))> access(const Accessor& accessor, LoaderParameters... parameters);
        template<typename Loader, typename Accessor>
        bool isLoaded(const Accessor& accessor);
        template<typename Loader, typename Accessor>
        void unload(const Accessor& accessor);
    private:
        std::unordered_map<std::type_index, std::unordered_map<std::string, std::shared_ptr<void>>> mResources;
};


template<typename Loader, typename Accessor, typename... LoaderParameters>
const std::shared_ptr<decltype(((Loader*)nullptr)->load(Accessor()))> ResourceCache::access(const Accessor& accessor, LoaderParameters... parameters)
{
    bool exists = true;

    auto loaderIterator = mResources.find(typeid(Loader));

    std::stringstream accessorStream;
    accessorStream << accessor;

    if(loaderIterator == mResources.end())
    {
        exists = false;
    }
    else
    {
        auto accessorIterator = loaderIterator->second.find(accessorStream.str());
        
        if(accessorIterator == loaderIterator->second.end())
        {
            exists = false;
        }
    }

    if(!exists)
    {
        Loader loader(parameters...);
        std::shared_ptr<decltype(((Loader*)nullptr)->load(Accessor()))> resourcePtr = std::make_shared<decltype(((Loader*)nullptr)->load(Accessor()))>(loader.load(accessor));

        mResources[typeid(Loader)].emplace(accessorStream.str(), std::static_pointer_cast<void>(resourcePtr));

        return resourcePtr;
    }
    else
    {
        return std::static_pointer_cast<decltype(((Loader*)nullptr)->load(Accessor()))>(loaderIterator->second.at(accessorStream.str()));
    }
}

template<typename Loader, typename Accessor>
bool ResourceCache::isLoaded(const Accessor& accessor)
{
    auto loaderIterator = mResources.find(typeid(Loader));

    if(loaderIterator == mResources.end())
    {
        return false;
    }
    else
    {
        std::stringstream accessorStream;
        accessorStream << accessor;
        auto accessorIterator = loaderIterator->second.find(accessorStream.str());

        if(accessorIterator == loaderIterator->second.end())
        {
            return false;
        }
    }
    return true;
}

template<typename Loader, typename Accessor>
void ResourceCache::unload(const Accessor& accessor)
{
    auto loaderIterator = mResources.find(typeid(Loader));

    std::stringstream accessorStream;
    accessorStream << accessor;

    if(loaderIterator != mResources.end())
    {
        loaderIterator->second.erase(accessorStream.str());

        if(loaderIterator->second.size() == 0)
        {
            mResources.erase(loaderIterator);
        }
    }
}
