#pragma once
#include <memory>

class RawModel;
class ShaderSource;
class ShaderDefinition;
class Texture;

template<typename Resource>
struct ResourceDeliverMessage
{
    uint32_t id;
    std::shared_ptr<Resource> resource;
};
