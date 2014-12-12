#pragma once
#include "renderable.hpp"
#include "../utilities/glm.hpp"

class Model;

class VoxelChunkRenderable : public Renderable
{
    public:
        VoxelChunkRenderable();
        virtual std::type_index getType() const;
        void setModel(const Model& model);
        const Model* findModel() const;
    private:
        const Model* mModel;
};
