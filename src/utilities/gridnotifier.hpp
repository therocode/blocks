#pragma once
#include <unordered_map>
#include <unordered_set>
#include <memory>
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
        std::unique_ptr<glm::i64vec3> set(const glm::i64vec3& cell, bool active);
    private:
        std::unordered_map<glm::i64vec3, int32_t> mBigCellRefCounts;
        std::unordered_set<glm::i64vec3> mActivatedCells;
};

template<uint32_t factor>
bool GridNotifier<factor>::isActive(const glm::i64vec3& bigCell) const
{
    return mBigCellRefCounts.count(bigCell) != 0;
}

template<uint32_t factor>
std::unique_ptr<glm::i64vec3> GridNotifier<factor>::set(const glm::i64vec3& cell, bool active)
{
    FEA_ASSERT((mActivatedCells.count(cell) == 0) == active, "Trying to " + (active ? std::string("activate") : std::string("deactivate")) + " cell " + glm::to_string((glm::ivec3)cell) + " which is already in that state");

    glm::i64vec3 bigCell = Converter::convert(cell);

    std::unique_ptr<glm::i64vec3> result;

    if(active)
    {
        auto iterator = mBigCellRefCounts.find(bigCell);

        if(iterator != mBigCellRefCounts.end())
            iterator->second++;
        else
        {
            mBigCellRefCounts.emplace(bigCell, 1);
            result = std::unique_ptr<glm::i64vec3>(new glm::i64vec3(bigCell));
        }


        mActivatedCells.insert(cell);
    }
    else
    {
        auto iterator = mBigCellRefCounts.find(bigCell);

        if(--iterator->second == 0)
        {
            mBigCellRefCounts.erase(iterator);
            result = std::unique_ptr<glm::i64vec3>(new glm::i64vec3(bigCell));
        }

        mActivatedCells.erase(cell);
    }

    return result;
}
