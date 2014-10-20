#pragma once
#include <iostream>
#include <algorithm>

inline int32_t getNthIndexOf(const std::string& line, char character, int32_t occurrences)
{
    int pos = 0;

    while(pos < line.length() && occurrences + 1)
    {
        if(line[pos++] == character)
            occurrences--;
    }

    return pos - 1;
}

inline std::string removeAfterComma(const std::string& line, int32_t commaIndex)
{
    return line.substr(0, getNthIndexOf(line, ',', commaIndex));
}

inline std::string removeBeforeComma(const std::string& line, int32_t commaIndex)
{
    return line.substr(getNthIndexOf(line, ',', commaIndex) + 1, std::string::npos);
}

#define named_enum(Type, ...) enum Type { __VA_ARGS__}; const std::string Type##String = #__VA_ARGS__; \
    inline std::string Type##ToString(int32_t value) \
    { \
        std::string withoutSpace = Type##String;\
        withoutSpace.erase(std::remove_if(withoutSpace.begin(), withoutSpace.end(), ::isspace), withoutSpace.end());\
        int32_t enumAmount = std::count(withoutSpace.begin(), withoutSpace.end(), ',') + 1;\
        if(value == 0)\
            return removeAfterComma(withoutSpace, 0);\
        else if(value == enumAmount -1) \
            return removeBeforeComma(withoutSpace, value - 1);\
        else \
            return removeBeforeComma(removeAfterComma(withoutSpace, value), value - 1);\
    }
