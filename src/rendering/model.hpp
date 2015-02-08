#pragma once
#include <cstdint>
#include <unordered_map>
#include <memory>
#include "mesh.hpp"
#include "animation.hpp"
#include "modelattribute.hpp"

class Model
{
    public:
        void addVertexArray(ModelAttribute type, const std::vector<float>& vertices);
        void addBlendArray(ModelAttribute type, const std::vector<uint8_t>& vertices);
        const std::vector<float>* findVertexArray(ModelAttribute type) const;
        const std::vector<uint8_t>* findBlendArray(ModelAttribute type) const;
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
        std::unordered_map<ModelAttribute, std::vector<float>> mVertexArrays;
        std::unordered_map<ModelAttribute, std::vector<uint8_t>> mBlendArrays;
        std::unordered_map<int32_t, std::unique_ptr<Mesh>> mMeshes;
        std::unordered_map<std::string, std::unique_ptr<Animation>> mAnimations;
        std::vector<int32_t> mJointStructure;
};
