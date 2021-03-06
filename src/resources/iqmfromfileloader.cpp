#include "iqmfromfileloader.hpp"
#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>
#include "../utilities/glm.hpp"
#include <fea/assert.hpp>
#include "resourceexception.hpp"

RawModel IQMFromFileLoader::load(const std::string& filename)
{
    RawModel rawModel;

    std::fstream file;
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    if(file.fail())
    {
        throw ResourceException("Cannot open file " + filename);
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> iqmData(fileSize);
    file.read((char*)iqmData.data(), fileSize);

    char* headerBytes = iqmData.data();
    char* headerBytesIter = headerBytes;

    file.seekg(0);

    iqmheader header;
    readIqmHeader(headerBytes, header);

    char* textIterator = headerBytes + header.ofs_text;

    std::string strings;
    strings.resize(header.num_text);

    std::copy(textIterator, textIterator + header.num_text, &strings[0]);

    if(std::string(header.magic) != std::string(IQM_MAGIC))
    {
        throw ResourceException("Not an IQM file. File should start with '" + std::string(IQM_MAGIC) + "' but was '" + std::string(header.magic) + "'");
    }

    if(header.version != IQM_VERSION)
    {
        throw ResourceException("Wrong IQM version. Got " + std::to_string(header.version) + " should be " + std::to_string(IQM_VERSION));
    }
    if(header.filesize > (16<<20))
    {
        throw ResourceException("IQM file bigger than 16mb");
    }

    char* vertexArrayBytesIterator = headerBytes + header.ofs_vertexarrays;
    
    for(uint32_t i = 0; i < header.num_vertexarrays; i++)
    {
        iqmvertexarray vertexArray;
        vertexArrayBytesIterator = readIqmVertexArray(vertexArrayBytesIterator, vertexArray);

        //to read others, read all array headers at once
        if(vertexArray.type == IQM_POSITION)
        {
            std::vector<float> positions(header.num_vertexes * 3);
            std::copy(headerBytes + vertexArray.offset, headerBytes + vertexArray.offset + sizeof(float) * header.num_vertexes * 3, (char*)positions.data());

            rawModel.positions = std::move(positions);
        }
        else if(vertexArray.type == IQM_NORMAL)
        {
            std::vector<float> normals(header.num_vertexes * 3);
            std::copy(headerBytes + vertexArray.offset, headerBytes + vertexArray.offset + sizeof(float) * header.num_vertexes * 3, (char*)normals.data());

            rawModel.normals = std::move(normals);
        }
        else if(vertexArray.type == IQM_TEXCOORD)
        {
            std::vector<float> texCoords(header.num_vertexes * 2);
            std::copy(headerBytes + vertexArray.offset, headerBytes + vertexArray.offset + sizeof(float) * header.num_vertexes * 2, (char*)texCoords.data());

            rawModel.texCoords = std::move(texCoords);
        }
        else if(vertexArray.type == IQM_BLENDINDEXES)
        {
            std::vector<uint8_t> blendIndices(header.num_vertexes * 4);
            std::copy(headerBytes + vertexArray.offset, headerBytes + vertexArray.offset + sizeof(uint8_t) * header.num_vertexes * 4, (char*)blendIndices.data());

            rawModel.blendIndices = std::move(blendIndices);
        }
        else if(vertexArray.type == IQM_BLENDWEIGHTS)
        {
            std::vector<uint8_t> blendWeights(header.num_vertexes * 4);
            std::copy(headerBytes + vertexArray.offset, headerBytes + vertexArray.offset + sizeof(uint8_t) * header.num_vertexes * 4, (char*)blendWeights.data());

            rawModel.blendWeights = std::move(blendWeights);
        }
    }

    char* meshBytesIterator = headerBytes + header.ofs_meshes;

    for(uint32_t i = 0; i < header.num_meshes; i++)
    {
        iqmmesh mesh;
        meshBytesIterator = readIqmMesh(meshBytesIterator, mesh);

        iqmtriangle triangle;
        char* triangleIterator = headerBytes + header.ofs_triangles + (mesh.first_triangle * sizeof(triangle.vertex));
        std::vector<uint32_t> indices(mesh.num_triangles * 3);

        for(uint32_t j = 0; j < mesh.num_triangles; j++)
        {
            std::copy(triangleIterator, triangleIterator + sizeof(triangle.vertex), (char*)&triangle.vertex);
            triangleIterator += sizeof(triangle.vertex);

            indices.push_back(triangle.vertex[0]);
            indices.push_back(triangle.vertex[1]);
            indices.push_back(triangle.vertex[2]);
        }

        rawModel.indices.push_back(indices);
    }

    char* jointBytesIterator = headerBytes + header.ofs_joints;

    std::vector<Matrix3x4> baseframe(header.num_joints);
    std::vector<Matrix3x4> inversebaseframe(header.num_joints);

    rawModel.jointStructure.resize(header.num_joints);

    for(uint32_t i = 0; i < header.num_joints; i++)
    {
        iqmjoint joint;
        jointBytesIterator = readIqmJoint(jointBytesIterator, joint);

        baseframe[i] = Matrix3x4(Quat(joint.rotate).normalize(), Vec3(joint.translate), Vec3(joint.scale));
        inversebaseframe[i].invert(baseframe[i]);
        if(joint.parent >= 0) 
        {   
            baseframe[i] = baseframe[joint.parent] * baseframe[i];
            inversebaseframe[i] *= inversebaseframe[joint.parent];
        }  

        rawModel.jointStructure[i] = joint.parent;
    }


    std::vector<Matrix3x4> frames(header.num_frames * header.num_poses);
    //std::vector<ushort> framedata(headerBytes + header.ofs_frames, headerBytes + header.ofs_frames + header.num_frames * header.num_framechannels);
    //auto frameDataIter = framedata.begin();
    uint16_t* frameDataIter = (uint16_t*)(headerBytes + header.ofs_frames);

    if(header.num_frames > 0 && header.num_joints == 0)
    {
        throw ResourceException("iqm file with " + std::to_string(header.num_frames) + " frames have zero joints.");
    }

    for(int32_t frameIndex = 0; frameIndex < header.num_frames; frameIndex++)
    {
        char* poseBytesIterator = headerBytes + header.ofs_poses;
        for(int32_t poseIndex = 0; poseIndex < header.num_poses; poseIndex++)
        {
            iqmpose pose;
            poseBytesIterator = readIqmPose(poseBytesIterator, pose);

            Quat rotate;
            Vec3 translate, scale;
            translate.x = pose.channeloffset[0]; if(pose.mask&0x01) translate.x += *frameDataIter++ * pose.channelscale[0];
            translate.y = pose.channeloffset[1]; if(pose.mask&0x02) translate.y += *frameDataIter++ * pose.channelscale[1];
            translate.z = pose.channeloffset[2]; if(pose.mask&0x04) translate.z += *frameDataIter++ * pose.channelscale[2];
            rotate.x = pose.channeloffset[3]; if(pose.mask&0x08) rotate.x += *frameDataIter++ * pose.channelscale[3];
            rotate.y = pose.channeloffset[4]; if(pose.mask&0x10) rotate.y += *frameDataIter++ * pose.channelscale[4];
            rotate.z = pose.channeloffset[5]; if(pose.mask&0x20) rotate.z += *frameDataIter++ * pose.channelscale[5];
            rotate.w = pose.channeloffset[6]; if(pose.mask&0x40) rotate.w += *frameDataIter++ * pose.channelscale[6];
            scale.x = pose.channeloffset[7]; if(pose.mask&0x80) scale.x += *frameDataIter++ * pose.channelscale[7];
            scale.y = pose.channeloffset[8]; if(pose.mask&0x100) scale.y += *frameDataIter++ * pose.channelscale[8];
            scale.z = pose.channeloffset[9]; if(pose.mask&0x200) scale.z += *frameDataIter++ * pose.channelscale[9];

            Matrix3x4 m(rotate.normalize(), translate, scale);

            if(pose.parent >= 0)
            {
                frames[frameIndex * header.num_poses + poseIndex] = baseframe[pose.parent] * m * inversebaseframe[poseIndex];
            }
            else
            {
                frames[frameIndex * header.num_poses + poseIndex] = m * inversebaseframe[poseIndex];
            }
        }
    }

    char* animBytesIterator = headerBytes + header.ofs_anims;


    for(uint32_t animationIndex = 0; animationIndex < header.num_anims; animationIndex++)
    {
        iqmanim animation;
        animBytesIterator = readIqmAnim(animBytesIterator, animation);
        RawAnimation rawAnimation;

        uint32_t firstFrame = animation.first_frame;
        uint32_t frameAmount = animation.num_frames * header.num_poses;

        rawAnimation.rotations.resize(frameAmount);
        rawAnimation.translations.resize(frameAmount);

        for(uint32_t i = 0; i < frameAmount; i++)
        {
            uint32_t frameIndex = i + firstFrame;
            rawAnimation.rotations[i][0][0] = frames[frameIndex].a.x;
            rawAnimation.rotations[i][1][0] = frames[frameIndex].a.y;
            rawAnimation.rotations[i][2][0] = frames[frameIndex].a.z;
            rawAnimation.rotations[i][0][1] = frames[frameIndex].b.x;
            rawAnimation.rotations[i][1][1] = frames[frameIndex].b.y;
            rawAnimation.rotations[i][2][1] = frames[frameIndex].b.z;
            rawAnimation.rotations[i][0][2] = frames[frameIndex].c.x;
            rawAnimation.rotations[i][1][2] = frames[frameIndex].c.y;
            rawAnimation.rotations[i][2][2] = frames[frameIndex].c.z;

            rawAnimation.translations[i][0] = frames[frameIndex].a.w;
            rawAnimation.translations[i][1] = frames[frameIndex].b.w;
            rawAnimation.translations[i][2] = frames[frameIndex].c.w;
        }

        rawAnimation.framerate = animation.framerate;
        rawAnimation.frameAmount = animation.num_frames;
        rawAnimation.name = std::string(&strings[animation.name]);

        rawModel.animations.push_back(rawAnimation);
    }

    return rawModel;
}

char* IQMFromFileLoader::readIqmHeader(char* headerPointer, iqmheader& result)
{
    std::copy(headerPointer, headerPointer + sizeof(result.magic), (char*)&result.magic);
    headerPointer += sizeof(result.magic);
    std::copy(headerPointer, headerPointer + sizeof(result.version), (char*)&result.version);
    headerPointer += sizeof(result.version);
    std::copy(headerPointer, headerPointer + sizeof(result.filesize), (char*)&result.filesize);
    headerPointer += sizeof(result.filesize);
    std::copy(headerPointer, headerPointer + sizeof(result.flags), (char*)&result.flags);
    headerPointer += sizeof(result.flags);
    std::copy(headerPointer, headerPointer + sizeof(result.num_text), (char*)&result.num_text);
    headerPointer += sizeof(result.num_text);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_text), (char*)&result.ofs_text);
    headerPointer += sizeof(result.ofs_text);
    std::copy(headerPointer, headerPointer + sizeof(result.num_meshes), (char*)&result.num_meshes);
    headerPointer += sizeof(result.num_meshes);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_meshes), (char*)&result.ofs_meshes);
    headerPointer += sizeof(result.ofs_meshes);
    std::copy(headerPointer, headerPointer + sizeof(result.num_vertexarrays), (char*)&result.num_vertexarrays);
    headerPointer += sizeof(result.num_vertexarrays);
    std::copy(headerPointer, headerPointer + sizeof(result.num_vertexes), (char*)&result.num_vertexes);
    headerPointer += sizeof(result.num_vertexes);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_vertexarrays), (char*)&result.ofs_vertexarrays);
    headerPointer += sizeof(result.ofs_vertexarrays);
    std::copy(headerPointer, headerPointer + sizeof(result.num_triangles), (char*)&result.num_triangles);
    headerPointer += sizeof(result.num_triangles);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_triangles), (char*)&result.ofs_triangles);
    headerPointer += sizeof(result.ofs_triangles);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_adjacency), (char*)&result.ofs_adjacency);
    headerPointer += sizeof(result.ofs_adjacency);
    std::copy(headerPointer, headerPointer + sizeof(result.num_joints), (char*)&result.num_joints);
    headerPointer += sizeof(result.num_joints);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_joints), (char*)&result.ofs_joints);
    headerPointer += sizeof(result.ofs_joints);
    std::copy(headerPointer, headerPointer + sizeof(result.num_poses), (char*)&result.num_poses);
    headerPointer += sizeof(result.num_poses);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_poses), (char*)&result.ofs_poses);
    headerPointer += sizeof(result.ofs_poses);
    std::copy(headerPointer, headerPointer + sizeof(result.num_anims), (char*)&result.num_anims);
    headerPointer += sizeof(result.num_anims);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_anims), (char*)&result.ofs_anims);
    headerPointer += sizeof(result.ofs_anims);
    std::copy(headerPointer, headerPointer + sizeof(result.num_frames), (char*)&result.num_frames);
    headerPointer += sizeof(result.num_frames);
    std::copy(headerPointer, headerPointer + sizeof(result.num_framechannels), (char*)&result.num_framechannels);
    headerPointer += sizeof(result.num_framechannels);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_frames), (char*)&result.ofs_frames);
    headerPointer += sizeof(result.ofs_frames);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_bounds), (char*)&result.ofs_bounds);
    headerPointer += sizeof(result.ofs_bounds);
    std::copy(headerPointer, headerPointer + sizeof(result.num_comment), (char*)&result.num_comment);
    headerPointer += sizeof(result.num_comment);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_comment), (char*)&result.ofs_comment);
    headerPointer += sizeof(result.ofs_comment);
    std::copy(headerPointer, headerPointer + sizeof(result.num_extensions), (char*)&result.num_extensions);
    headerPointer += sizeof(result.num_extensions);
    std::copy(headerPointer, headerPointer + sizeof(result.ofs_extensions), (char*)&result.ofs_extensions);
    headerPointer += sizeof(result.ofs_extensions);

    return headerPointer;
}

char* IQMFromFileLoader::readIqmVertexArray(char* vertexArrayPointer, iqmvertexarray& result)
{
    std::copy(vertexArrayPointer, vertexArrayPointer + sizeof(result.type), (char*)&result.type);
    vertexArrayPointer += sizeof(result.type);
    std::copy(vertexArrayPointer, vertexArrayPointer + sizeof(result.flags), (char*)&result.flags);
    vertexArrayPointer += sizeof(result.flags);
    std::copy(vertexArrayPointer, vertexArrayPointer + sizeof(result.format), (char*)&result.format);
    vertexArrayPointer += sizeof(result.format);
    std::copy(vertexArrayPointer, vertexArrayPointer + sizeof(result.size), (char*)&result.size);
    vertexArrayPointer += sizeof(result.size);
    std::copy(vertexArrayPointer, vertexArrayPointer + sizeof(result.offset), (char*)&result.offset);
    vertexArrayPointer += sizeof(result.offset);

    return vertexArrayPointer;
}

char* IQMFromFileLoader::readIqmMesh(char* meshArrayPointer, iqmmesh& result)
{
    std::copy(meshArrayPointer, meshArrayPointer + sizeof(result.name), (char*)&result.name);
    meshArrayPointer += sizeof(result.name);
    std::copy(meshArrayPointer, meshArrayPointer + sizeof(result.material), (char*)&result.material);
    meshArrayPointer += sizeof(result.material);
    std::copy(meshArrayPointer, meshArrayPointer + sizeof(result.first_vertex), (char*)&result.first_vertex);
    meshArrayPointer += sizeof(result.first_vertex);
    std::copy(meshArrayPointer, meshArrayPointer + sizeof(result.num_vertexes), (char*)&result.num_vertexes);
    meshArrayPointer += sizeof(result.num_vertexes);
    std::copy(meshArrayPointer, meshArrayPointer + sizeof(result.first_triangle), (char*)&result.first_triangle);
    meshArrayPointer += sizeof(result.first_triangle);
    std::copy(meshArrayPointer, meshArrayPointer + sizeof(result.num_triangles), (char*)&result.num_triangles);
    meshArrayPointer += sizeof(result.num_triangles);

    return meshArrayPointer;
}

char* IQMFromFileLoader::readIqmJoint(char* jointArrayPointer, iqmjoint& result)
{
    std::copy(jointArrayPointer, jointArrayPointer + sizeof(result.name), (char*)&result.name);
    jointArrayPointer += sizeof(result.name);
    std::copy(jointArrayPointer, jointArrayPointer + sizeof(result.parent), (char*)&result.parent);
    jointArrayPointer += sizeof(result.parent);
    std::copy(jointArrayPointer, jointArrayPointer + sizeof(result.translate), (char*)&result.translate);
    jointArrayPointer += sizeof(result.translate);
    std::copy(jointArrayPointer, jointArrayPointer + sizeof(result.rotate), (char*)&result.rotate);
    jointArrayPointer += sizeof(result.rotate);
    std::copy(jointArrayPointer, jointArrayPointer + sizeof(result.scale), (char*)&result.scale);
    jointArrayPointer += sizeof(result.scale);

    return jointArrayPointer;
}

char* IQMFromFileLoader::readIqmPose(char* poseArrayPointer, iqmpose& result)
{
    std::copy(poseArrayPointer, poseArrayPointer + sizeof(result.parent), (char*)&result.parent);
    poseArrayPointer += sizeof(result.parent);
    std::copy(poseArrayPointer, poseArrayPointer + sizeof(result.mask), (char*)&result.mask);
    poseArrayPointer += sizeof(result.mask);
    std::copy(poseArrayPointer, poseArrayPointer + sizeof(result.channeloffset), (char*)&result.channeloffset);
    poseArrayPointer += sizeof(result.channeloffset);
    std::copy(poseArrayPointer, poseArrayPointer + sizeof(result.channelscale), (char*)&result.channelscale);
    poseArrayPointer += sizeof(result.channelscale);

    return poseArrayPointer;
}

char* IQMFromFileLoader::readIqmAnim(char* animArrayPointer, iqmanim& result)
{
    std::copy(animArrayPointer, animArrayPointer + sizeof(result.name), (char*)&result.name);
    animArrayPointer += sizeof(result.name);
    std::copy(animArrayPointer, animArrayPointer + sizeof(result.first_frame), (char*)&result.first_frame);
    animArrayPointer += sizeof(result.first_frame);
    std::copy(animArrayPointer, animArrayPointer + sizeof(result.num_frames), (char*)&result.num_frames);
    animArrayPointer += sizeof(result.num_frames);
    std::copy(animArrayPointer, animArrayPointer + sizeof(result.framerate), (char*)&result.framerate);
    animArrayPointer += sizeof(result.framerate);
    std::copy(animArrayPointer, animArrayPointer + sizeof(result.flags), (char*)&result.flags);
    animArrayPointer += sizeof(result.flags);

    return animArrayPointer;
}
