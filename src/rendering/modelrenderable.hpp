#pragma once
#include "renderable.hpp"
#include "../utilities/glm.hpp"

class Model;
class TextureArray;

class ModelRenderable : public Renderable
{
    public:
        ModelRenderable();
        virtual std::type_index getType() const;
        void setPosition(const glm::vec3& position);
        const glm::vec3& getPosition() const;
        void setColor(const glm::vec3& color);
        const glm::vec3& getColor() const;
		void setOrientation(glm::quat orientation);
		const glm::quat getOrientation() const;
        void setFrameOffset(float offset);
        float getFrameOffset() const;
        void setModel(const Model& model);
        const Model* findModel() const;
        void setTexture(const TextureArray& texture, uint32_t index);
        const TextureArray* findTextureArray() const;
        uint32_t getTextureIndex() const;
        const std::string& getAnimation() const;
        void setAnimation(const std::string& animation);
    private:
        glm::vec3 mPosition;
        glm::vec3 mColor;
		glm::quat mOrientation;
        float mFrameOffset;
        const Model* mModel;
        const TextureArray* mTextureArray;
        uint32_t mTextureIndex;
        std::string mAnimation;
};
