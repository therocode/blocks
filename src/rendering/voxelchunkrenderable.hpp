#pragma once
#include "renderable.hpp"
#include "../utilities/glm.hpp"
#include "../world/worlddefines.hpp"

class ChunkModel;
class TextureArray;

class VoxelChunkRenderable : public Renderable
{
    public:
        VoxelChunkRenderable();
        virtual std::type_index getType() const;
        void setModel(const ChunkModel& model);
        const ChunkModel* findModel() const;
        void setTexture(const TextureArray& texture);
        const TextureArray* findTextureArray() const;
        void setCoordinate(const ChunkCoord& coordinate);
        const ChunkCoord& getCoordinate() const;
        void updated();
        bool isUpdated() const;
    private:
        const ChunkModel* mModel;
        const TextureArray* mTextureArray;
        ChunkCoord mCoordinate;
        bool mUpdated;
};
