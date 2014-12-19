#pragma once
#include "renderable.hpp"
#include "../utilities/glm.hpp"

class Model;
class Texture;

class VoxelChunkRenderable : public Renderable
{
    public:
        VoxelChunkRenderable();
        virtual std::type_index getType() const;
        void setModel(const Model& model);
        const Model* findModel() const;
        void setTexture(const Texture& model);
        const Texture* findTexture() const;
    private:
        const Model* mModel;
        const Texture* mTexture;
};
