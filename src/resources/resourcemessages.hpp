#pragma once
#include <memory>

class RawModel;
class ShaderSource;
class ShaderDefinition;

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
    std::shared_ptr<ShaderDefinition> shaderDefinition;
};
