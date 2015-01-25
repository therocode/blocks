#pragma once
#include <cstdint>
#include <unordered_map>
#include <memory>
#include "buffer.hpp"
#include "mesh.hpp"
#include "animation.hpp"

class Model
{
    public:
        enum { POSITIONS, TEXCOORDS, NORMALS, BLENDWEIGHTS, BLENDINDICES };
        void addVertexArray(int32_t type, std::vector<float> vertices);
        void addVertexArray(int32_t type, std::vector<uint8_t> vertices);
        const Buffer* findVertexArray(int32_t type) const;
        void addMesh(int32_t id, std::unique_ptr<Mesh> mesh);
        const Mesh* findMesh(int32_t id) const;
        const std::unordered_map<int32_t, std::unique_ptr<Mesh>>& getMeshes() const;
        void addAnimation(const std::string& name, std::unique_ptr<Animation> animation);
        const Animation* findAnimation(const std::string& name) const;
        void setJointStructure(const std::vector<int32_t>& jointStructure);
        const std::vector<int32_t>& getJointStructure() const;
        //tempcrap:
        const Animation* getAnimation() const;
    private:
        std::unordered_map<int32_t, std::unique_ptr<Buffer>> mVertexArrays;
        std::unordered_map<int32_t, std::unique_ptr<Mesh>> mMeshes;
        std::unordered_map<std::string, std::unique_ptr<Animation>> mAnimations;
        std::vector<int32_t> mJointStructure;
};
