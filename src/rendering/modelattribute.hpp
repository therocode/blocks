#pragma once

enum class ModelAttribute { POSITIONS, TEXCOORDS, NORMALS, BLENDWEIGHTS, BLENDINDICES };

namespace std
{
    template<>
        struct hash<ModelAttribute>
        {
            public:
                std::size_t operator()(ModelAttribute const& val) const 
                {
                    return (size_t) val;
                }
        };
}
