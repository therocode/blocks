#pragma once
#include <cstdint>
#include <unordered_map>

class Mesh;

class Model
{
    public:
        void addMesh(int32_t id, const Mesh& mesh);
        const Mesh* findMesh(int32_t id) const;
    private:
        std::unordered_map<int32_t, const Mesh&> mMeshes;
};
