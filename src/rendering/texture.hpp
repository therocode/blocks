#include "utilities/glm.hpp"
#include <string>

///A texture that can read PNGs from file, and do stuff with it.
class Texture{
    public:
        Texture();
        void loadFromFile(const std::string path);
        size_t getSize() const;

    private:
        size_t  mDataSize;
        int     mWidth;
        int     mHeight;
};
