#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fea/assert.hpp>
#include "glm.hpp"
#include "glmhash.hpp"
#include "coordinateconverter.hpp"

template<uint32_t factor>
class GridNotifier
{
    using Converter = CoordinateCoarseConvert<glm::i64vec3, glm::i64vec3, factor>;
    public:
        bool isActive(const glm::i64vec3& bigCell) const;
        const std::vector<glm::i64vec3>& set(const glm::i64vec3& cell, bool active);
        std::vector<glm::i64vec3> getAllActive() const;
    private:
        std::unordered_map<glm::i64vec3, int32_t> mBigCellRefCounts;
        std::unordered_set<glm::i64vec3> mActivatedCells;
        std::vector<glm::i64vec3> resultVector;
};

template<uint32_t factor>
bool GridNotifier<factor>::isActive(const glm::i64vec3& bigCell) const
{
    return mBigCellRefCounts.count(bigCell) != 0;
}

template<uint32_t factor>
const std::vector<glm::i64vec3>& GridNotifier<factor>::set(const glm::i64vec3& cell, bool active)
{
    FEA_ASSERT((mActivatedCells.count(cell) == 0) == active, "Trying to " + (active ? std::string("activate") : std::string("deactivate")) + " cell " + glm::to_string((glm::ivec3)cell) + " which is already in that state");
    resultVector.clear();

    glm::i64vec3 bigCell = Converter::convert(cell);

    if(active)
    {
        auto iterator = mBigCellRefCounts.find(bigCell);

        if(iterator != mBigCellRefCounts.end())
            iterator->second++;
        else
        {
            mBigCellRefCounts.emplace(bigCell, 1);
            resultVector.push_back(bigCell);
        }


        mActivatedCells.insert(cell);
    }
    else
    {
        auto iterator = mBigCellRefCounts.find(bigCell);

        if(--iterator->second == 0)
        {
            mBigCellRefCounts.erase(iterator);
            resultVector.push_back(bigCell);
        }

        mActivatedCells.erase(cell);
    }

    return resultVector;
}

template<uint32_t factor>
std::vector<glm::i64vec3> GridNotifier<factor>::getAllActive() const
{
    std::vector<glm::i64vec3> result;

    for(auto iterator : mBigCellRefCounts)
    {
        result.push_back(iterator.first);
    }

    return result;
}
