#include "debugrenderer.hpp"
#include "shapes.hpp"
#include "shaderattribute.hpp"
#include "camera.hpp"
#include <vector>
#include <string>
#include <iostream>
#include "../resources/image.hpp"

DebugRenderer::DebugRenderer() :
    mRenderAmount(0)
{
    mVertexArray.bind();
    mVertexBuffer.setData(cubeVertices);

    mVertexArray.setVertexAttribute(ShaderAttribute::POSITION, 3, mVertexBuffer);
    mVertexArray.setVertexAttribute(ShaderAttribute::NORMAL, 3, mVertexBuffer);
    //mVertexArray.setVertexAttribute(ShaderAttribute::TEXCOORD, 2, mVertexBuffer);

    mVertexArray.setInstanceAttribute(MODELMATRIX1, 4, mModelMatrixBuffer1, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX2, 4, mModelMatrixBuffer2, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX3, 4, mModelMatrixBuffer3, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX4, 4, mModelMatrixBuffer4, 1);

    mVertexArray.setInstanceAttribute(COLOR, 3, mColorBuffer, 1);
	mVertexArray.unbind();

	mLineVertexArray.bind();
	mLineVertexArray.setVertexAttribute(ShaderAttribute::POSITION, 3, mLineVertexBuffer);
	mLineVertexArray.setVertexAttribute(COLOR, 3, mLineColorBuffer);
    mLineVertexArray.setVertexAttribute(MODELMATRIX1, 4, mModelMatrixBuffer1);
    mLineVertexArray.setVertexAttribute(MODELMATRIX2, 4, mModelMatrixBuffer2);
    mLineVertexArray.setVertexAttribute(MODELMATRIX3, 4, mModelMatrixBuffer3);
    mLineVertexArray.setVertexAttribute(MODELMATRIX4, 4, mModelMatrixBuffer4);
	mLineVertexArray.unbind();

    data1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    data2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    data3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    data4 = { 0.0f, 0.0f, 0.0f, 1.0f };

    Image image({16, 16}, fea::Color::White);
    mWhiteTextureArray.create({16, 16}, 1, image.getPixelsPointer());
}

void DebugRenderer::queue(const Renderable& renderable)
{
    const DebugRenderable& debugRenderable = (const DebugRenderable&) renderable;

	if(debugRenderable.getShape() == DebugRenderable::LINE)
	{
		const glm::vec3& lineStart = debugRenderable.getLineStartPoint();
		const glm::vec3& lineEnd = debugRenderable.getLineEndPoint();

		mLineCoords.push_back(lineStart.x);
		mLineCoords.push_back(lineStart.y);
		mLineCoords.push_back(lineStart.z);
		mLineCoords.push_back(lineEnd.x);
		mLineCoords.push_back(lineEnd.y);
		mLineCoords.push_back(lineEnd.z);

		const glm::vec3& startColor = debugRenderable.getLineStartColor();
		const glm::vec3& endColor = debugRenderable.getLineEndColor();

		mLineColorData.push_back(startColor.x);
		mLineColorData.push_back(startColor.y);
		mLineColorData.push_back(startColor.z);
		mLineColorData.push_back(endColor.x);
		mLineColorData.push_back(endColor.y);
		mLineColorData.push_back(endColor.z);
	}
	else
	{
		const glm::vec3& position = debugRenderable.getPosition();
		const glm::vec3& color = debugRenderable.getColor();
		const float pitch = debugRenderable.getPitch();
		const float yaw = debugRenderable.getYaw();

		data4[0] = position.x;
		data4[1] = position.y;
		data4[2] = position.z;

		const float sinPitch = std::sin(pitch);
		const float cosPitch = std::cos(pitch);
		const float sinYaw = std::sin(yaw);
		const float cosYaw = std::cos(yaw);
		data1[0] = cosYaw;
		// data1[1] = 0;
		data1[2] = -sinYaw;
		data2[0] = sinPitch * sinYaw;
		data2[1] = cosPitch;
		data2[2] = sinPitch * cosYaw;
		data3[0] = cosPitch * sinYaw;
		data3[1] = -sinPitch;
		data3[2] = cosPitch * cosYaw;
		
		mModelMatrixData1.insert(mModelMatrixData1.end(), data1.begin(), data1.end());
		mModelMatrixData2.insert(mModelMatrixData2.end(), data2.begin(), data2.end());
		mModelMatrixData3.insert(mModelMatrixData3.end(), data3.begin(), data3.end());
		mModelMatrixData4.insert(mModelMatrixData4.end(), data4.begin(), data4.end());

		mColorData.push_back(color.x);
		mColorData.push_back(color.y);
		mColorData.push_back(color.z);

		mRenderAmount++;
	}
}

void DebugRenderer::render(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
{
    GLuint id = mWhiteTextureArray.getId();
    
    shader.setUniform("textureArray", UniformType::TEXTURE_ARRAY, &id);

    if(!mModelMatrixData1.empty())
    {
        mVertexArray.bind();

        mModelMatrixBuffer1.setData(mModelMatrixData1);
        mModelMatrixBuffer2.setData(mModelMatrixData2);
        mModelMatrixBuffer3.setData(mModelMatrixData3);
        mModelMatrixBuffer4.setData(mModelMatrixData4);

        mColorBuffer.setData(mColorData);

        mModelMatrixData1.clear();
        mModelMatrixData2.clear();
        mModelMatrixData3.clear();
        mModelMatrixData4.clear();

        mColorData.clear();

        shader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));
        float shadedRatio = 0.0f;
        shader.setUniform("shadedRatio", UniformType::FLOAT, &shadedRatio);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mRenderAmount);
        mRenderAmount = 0;

        mVertexArray.unbind();
    }

	if(!mLineCoords.empty())
    {
        std::vector<float> data1 = { 1.0f, 0.0f, 0.0f, 0.0f };
        std::vector<float> data2 = { 0.0f, 1.0f, 0.0f, 0.0f };
        std::vector<float> data3 = { 0.0f, 0.0f, 1.0f, 0.0f };
        std::vector<float> data4 = { 0.0f, 0.0f, 0.0f, 1.0f };

        for(int32_t i = 0; i < mLineCoords.size() / 3; i++)
        {
            mModelMatrixData1.insert(mModelMatrixData1.begin(), data1.begin(), data1.end());
            mModelMatrixData2.insert(mModelMatrixData2.begin(), data2.begin(), data2.end());
            mModelMatrixData3.insert(mModelMatrixData3.begin(), data3.begin(), data3.end());
            mModelMatrixData4.insert(mModelMatrixData4.begin(), data4.begin(), data4.end());
        }

        mLineVertexArray.bind();

        mModelMatrixBuffer1.setData(mModelMatrixData1);
        mModelMatrixBuffer2.setData(mModelMatrixData2);
        mModelMatrixBuffer3.setData(mModelMatrixData3);
        mModelMatrixBuffer4.setData(mModelMatrixData4);

        mLineVertexBuffer.setData(mLineCoords);
        mLineColorBuffer.setData(mLineColorData);
        glDrawArrays(GL_LINES, 0, mLineCoords.size() / 3);
        mLineCoords.clear();
        mLineColorData.clear();

        mModelMatrixData1.clear();
        mModelMatrixData2.clear();
        mModelMatrixData3.clear();
        mModelMatrixData4.clear();

        mLineVertexArray.unbind();
    }
}

std::type_index DebugRenderer::getRenderableType() const
{
    return std::type_index(typeid(DebugRenderable));   
}
