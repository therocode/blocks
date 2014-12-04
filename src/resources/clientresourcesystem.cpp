#include "clientresourcesystem.hpp"
#include "iqmfromfileloader.hpp"
#include "resourcemessages.hpp"

ClientResourceSystem::ClientResourceSystem(fea::MessageBus& bus) :
    mBus(bus)
{
    std::shared_ptr<RawModel> model = mCache.access<IQMFromFileLoader>(std::string("assets/graphics/entities/poring/poring.iqm"));

    if(model)
    {
        if(model->positions.size() > 0)
        {
            mBus.send(ModelDeliverMessage{model});
        }
    }
}
