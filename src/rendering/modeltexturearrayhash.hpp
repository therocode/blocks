#pragma once
#include <functional>

namespace std
{
    template<>
        struct hash<std::pair<const Model*, const TextureArray*>>
        {
            public:
                std::size_t operator()(std::pair<const Model*, const TextureArray*> const& val) const 
                {
                    return (size_t) std::hash<const Model*>()(val.first) ^ std::hash<const TextureArray*>()(val.second);
                }
        };
}
