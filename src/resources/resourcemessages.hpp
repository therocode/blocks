#pragma once
#include <memory>

class RawModel;
class ShaderSource;
class ShaderDefinition;
class Texture;

struct ModelDeliverMessage
{
    std::shared_ptr<RawModel> model;
};

struct ShaderSourceDeliverMessage
{
    std::shared_ptr<ShaderSource> shaderSource;
};

struct ShaderDefinitionDeliverMessage
{
    std::string name;
    std::shared_ptr<ShaderDefinition> shaderDefinition;
};

struct TextureDeliverMessage
{
    std::shared_ptr<Texture> texture;
};
