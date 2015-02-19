#pragma once
#include <string>
#include "gfxentitydefinition.hpp"
#include "../utilities/idmapper.hpp"

class GfxEntityDefinitionFromFileLoader
{
    public:
        GfxEntityDefinitionFromFileLoader(const IdMapper<std::string>& textureIds, const IdMapper<std::string>& modelIds);
        GfxEntityDefinition load(const std::string& filename);
    private:
        const IdMapper<std::string>& mTextureIds;
        const IdMapper<std::string>& mModelIds;
};
