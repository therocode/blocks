#pragma once
#include <string>
#include "texture.hpp"

class TextureFromFileLoader
{
    public:
        Texture load(const std::string& filename);
    private:
};
