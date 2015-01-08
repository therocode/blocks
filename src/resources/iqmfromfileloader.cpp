#include "iqmfromfileloader.hpp"
#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>
#include "../utilities/glm.hpp"

RawModel IQMFromFileLoader::load(const std::string& filename)
{
    RawModel rawModel;

    std::fstream file;
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    if (file.fail())
    {
        std::cout << "ERROR: Cannot open the file...\n";
        exit(0); //should be exception
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

    //std::cout << header.num_text << "\n";

    char* textIterator = headerBytes + header.ofs_text;

    std::string strings;
    strings.resize(header.num_text);

    std::copy(textIterator, textIterator + header.num_text, &strings[0]);

    //std::cout << "numstrings " << header.num_text << "\n";
    //std::cout << "texts: " << strings << "\n";

    //std::cout << std::string(headerBytes + header.ofs_text) << "\n";

    if(std::string(header.magic) != std::string(IQM_MAGIC))
    {
        std::cout << "ERROR: not an IQM file. Start string should be '" << IQM_MAGIC << "' but was '" << header.magic << "'\n";
        exit(0); //exception
    }
    if(header.version != IQM_VERSION)
    {
        std::cout << "Wrong IQM version: got " << header.version << " should be " << IQM_VERSION << "\n";
        exit(0); //exception
    }
    if(header.filesize > (16<<20))
    {
        std::cout << "IQM file bigger than 16MB error! It was " << header.filesize << " big!\n";
        exit(0); //exception
    }

    std::cout << "num_anims:" << header.num_anims << "\n";
    std::cout << "num_joints:" << header.num_joints << "\n";
    std::cout << "num_poses:" << header.num_poses << "\n";
    std::cout << "num_frames:" << header.num_frames << "\n";

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
            std::vector<uint8_t> blendWeights(header.num_vertexes * 4);
            std::copy(headerBytes + vertexArray.offset, headerBytes + vertexArray.offset + sizeof(uint8_t) * header.num_vertexes * 4, (char*)blendWeights.data());

            rawModel.blendIndices = std::move(blendWeights);
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

        //std::cout << "loaded mesh with material: '" << std::string(&strings[mesh.material]) << "'\n";
        rawModel.indices.push_back(indices);
    }

    char* jointBytesIterator = headerBytes + header.ofs_joints;

    std::cout << "found " << header.num_joints << " joints\n";

    std::vector<Joint> skeleton(header.num_joints);

    for(uint32_t i = 0; i < header.num_joints; i++)
    {
        iqmjoint joint;
        jointBytesIterator = readIqmJoint(jointBytesIterator, joint);

        glm::mat4x4 scaling = glm::scale(glm::vec3(joint.scale[0], joint.scale[1], joint.scale[2]));
        glm::mat4x4 rotationAndScaling = glm::mat4_cast(glm::normalize(glm::quat(joint.rotate[0], joint.rotate[1], joint.rotate[2], joint.rotate[3]))) * scaling;
        glm::mat4x4 jointTransformation = glm::translate(glm::vec3(joint.translate[0], joint.translate[1], joint.translate[2])) * rotationAndScaling;

        if(joint.parent >= 0)
            jointTransformation = skeleton[joint.parent].transformation * jointTransformation;

        std::cout << "joint " << i << " has:\nname: " << std::string(&strings[joint.name]) << "\nparent: " << joint.parent << "\ntranslation: " << glm::vec3(joint.translate[0], joint.translate[1], joint.translate[2]) << "\nrotation: " << glm::quat(joint.rotate[0], joint.rotate[1], joint.rotate[2], joint.rotate[3]) << "\nscale: " << glm::vec3(joint.scale[0], joint.scale[1], joint.scale[2]) << "\ntransform:\n" << jointTransformation << "\n\n";

        skeleton[i] = {joint.parent, jointTransformation};
    }

    rawModel.skeleton = skeleton;

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
