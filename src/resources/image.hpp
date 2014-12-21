#pragma once
#include <vector>
#include "../utilities/glm.hpp"

class Image
{
    public:
        Image();
        Image(const glm::uvec2& size, const std::vector<uint8_t>& pixelData);
        const uint8_t* getPixelsPointer() const;
        const glm::uvec2& getSize() const;
    private:
        glm::uvec2 mSize;
        std::vector<uint8_t> mPixels;
};
