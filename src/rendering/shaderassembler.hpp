#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class ShaderAssembler
{
    private:
        struct ShaderVariable
        {
            std::string registerName;
            std::string type;
            std::string currentChainer;
        };

        struct ShaderModule
        {
            std::unordered_map<std::string, std::string> variableMappings;
            std::string source;
        };
    public:
        std::string assemble(const std::string& source, const std::vector<std::string>& moduleSources);
    private:
        std::vector<ShaderModule> parseModules(const std::vector<std::string>& moduleSources) const;
        std::string replaceAll(std::string input, const std::string& from, const std::string& to) const;
        std::unordered_map<std::string, std::string> mOriginalMappings;
};

/*
#version 330
precision highp float;

uniform float shadedRatio;
uniform sampler2DArray textureArray;

in vec3 unshadedColor;

in vec2 fragmentTexCoord;
flat in uint fragmentTextureIndex;
in vec3 objectColor;
in vec3 projectionNormal;
in vec3 surfaceToCamera;
in vec3 surfaceToLight;

out vec4 fragColor;


//temp globals
vec3 surfaceColor = vec3(1.0, 1.0, 1.0);
vec3 specularColor = vec3(1.0, 1.0, 1.0);
float materialShininess = 40.0;

void main()
{
    //ambient
    vec3 ambientColor = vec3(0.1, 0.1, 0.1) * surfaceColor;

    //diffuse
    vec3 diffuseColor = surfaceColor * clamp(dot(projectionNormal, -surfaceToLight), 0, 1);

    //specular
    float specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(surfaceToLight, projectionNormal))), materialShininess);
    vec3 specularColor = specularColor * specularCoefficient;

    fragColor.rgb = mix(unshadedColor, ambientColor + diffuseColor + specularColor , shadedRatio);
    fragColor.a = 1.0;

    fragColor = fragColor * texture(textureArray, vec3(fragmentTexCoord, fragmentTextureIndex));

    vec3 prefinalColor = fragColor.rgb;

    ~~color~vec4~prefinalColor~

    ~%~

    fragColor = vec4(prefinalColor, 1.0f);
}

mix(~color~, vec3(1.0f, 0.0f, 0.0f, 1.0f), 0.5);

~color~ = vec3(1.0, 1.0, 1.0) - ~color~;
*/
