#pragma once
#include <cstdint>
#include <unordered_map>

class Mesh;

class Model
{
    public:
    private:
        std::unordered_map<int32_t, const Mesh&> mMeshes;
};
