#pragma once
#include <string>
#include "../utilities/gimgui.hpp"

class FontFromFileLoader
{
    public:
        gim::Font load(const std::string& filename);
};
