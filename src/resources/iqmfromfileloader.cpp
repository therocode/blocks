#include "iqmfromfileloader.hpp"
#include "iqm.h"
#include <cstring>
#include <iostream>
#include <fstream>

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

    iqmheader header;

    file.read((char*)&header.magic, sizeof(header.magic));
    file.read((char*)&header.version, sizeof(header.version));
    file.read((char*)&header.filesize, sizeof(header.filesize));
    file.read((char*)&header.flags, sizeof(header.flags));
    file.read((char*)&header.num_text, sizeof(header.num_text));
    file.read((char*)&header.ofs_text, sizeof(header.ofs_text));
    file.read((char*)&header.num_meshes, sizeof(header.num_meshes));
    file.read((char*)&header.ofs_meshes, sizeof(header.ofs_meshes));
    file.read((char*)&header.num_vertexarrays, sizeof(header.num_vertexarrays));
    file.read((char*)&header.num_vertexes, sizeof(header.num_vertexes));
    file.read((char*)&header.ofs_vertexarrays, sizeof(header.ofs_vertexarrays));
    file.read((char*)&header.num_triangles, sizeof(header.num_triangles));
    file.read((char*)&header.ofs_triangles, sizeof(header.ofs_triangles));
    file.read((char*)&header.ofs_adjacency, sizeof(header.ofs_adjacency));
    file.read((char*)&header.num_joints, sizeof(header.num_joints));
    file.read((char*)&header.ofs_joints, sizeof(header.ofs_joints));
    file.read((char*)&header.num_poses, sizeof(header.num_poses));
    file.read((char*)&header.ofs_poses, sizeof(header.ofs_poses));
    file.read((char*)&header.num_anims, sizeof(header.num_anims));
    file.read((char*)&header.ofs_anims, sizeof(header.ofs_anims));
    file.read((char*)&header.num_frames, sizeof(header.num_frames));
    file.read((char*)&header.num_framechannels, sizeof(header.num_framechannels));
    file.read((char*)&header.ofs_frames, sizeof(header.ofs_frames));
    file.read((char*)&header.ofs_bounds, sizeof(header.ofs_bounds));
    file.read((char*)&header.num_comment, sizeof(header.num_comment));
    file.read((char*)&header.ofs_comment, sizeof(header.ofs_comment));
    file.read((char*)&header.num_extensions, sizeof(header.num_extensions));
    file.read((char*)&header.ofs_extensions, sizeof(header.ofs_extensions));



    std::cout << "Magic: " << header.magic << "\n";
    std::cout << "Version: " << header.version << "\n";
    std::cout << "Filesize: " << header.filesize << "\n";
    std::cout << "Num meshes: " << header.num_meshes << "\n";
    std::cout << "Num vertexarrays: " << header.num_vertexarrays << "\n";
    std::cout << "Num vertices: " << header.num_vertexes << "\n";
    std::cout << "Num triangles: " << header.num_triangles << "\n";
    std::cout << "Num joints: " << header.num_joints << "\n";
    std::cout << "Num poses: " << header.num_poses << "\n";
    std::cout << "Num anims: " << header.num_anims << "\n";
    std::cout << "Num frames: " << header.num_frames << "\n";
    std::cout << "Num comment: " << header.num_comment << "\n";
    std::cout << "Num extensions: " << header.num_extensions << "\n";

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

    return rawModel;
}
