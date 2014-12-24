#include "animationshader.hpp"

std::string AnimationShader::vertexSource = R"(
#version 330

uniform mat4 viewProjectionMatrix;

layout(location = ~POSITION~) in vec3 in_position;
layout(location = ~NORMAL~) in vec3 in_normal;
layout(location = ~TEXCOORD~) in vec2 in_texCoord;
layout(location = ~COLOR~) in vec3 color;
layout(location = ~TEXTUREINDEX~) in uint textureIndex;
layout(location = ~MODELMATRIX1~) in mat4 modelMatrix;
layout(location = ~NORMALMATRIX1~) in mat4 normalMatrix;
layout(location = ~BLENDINDICES~) in uvec4 blendIndices;
layout(location = ~BLENDWEIGHTS~) in uvec4 blendWeights;

out vec3 objectColor;

out vec3 projectionNormal;
out vec3 surfaceToCamera;
out vec3 surfaceToLight;

out vec3 unshadedColor;
out vec2 fragmentTexCoord;
flat out uint fragmentTextureIndex;

vec3 lightDirection = vec3(1.0f, -1.0f, -1.0f);

void main()
{
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(vec3(in_position.x, in_position.y, in_position.z), 1.0);
    vec3 normal = (viewProjectionMatrix * modelMatrix * vec4(in_normal, 0.0)).xyz;

    projectionNormal = normal;

    surfaceToCamera = normalize(-gl_Position.xyz);
    surfaceToLight = normalize(((viewProjectionMatrix * vec4(normalize(lightDirection), 0.0))).xyz);
    unshadedColor = color;

    fragmentTexCoord = in_texCoord;
    fragmentTextureIndex = textureIndex;

    ~~position|vec4|gl_Position~~

    ~%~
})";

std::string AnimationShader::fragmentSource = R"(
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
    vec3 prefinalColor = fragColor.xyz;

    ~~color|vec3|prefinalColor~~

    ~%~

    fragColor = vec4(prefinalColor, 1.0f);
})";
