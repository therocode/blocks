#pragma once
#include <fea/assert.hpp>
#include <stack>
#include <stack>
#include <string>

template<typename IdType = uint32_t>
class IdPool
{
    public:
        IdPool();
        IdType next();
        void free(const IdType& value);
    private:
        IdType mNext;
        std::set<IdType> mReturned;
};

template<typename IdType>
IdPool<IdType>::IdPool() :
    mNext(0)
{
}

template<typename IdType>
IdType IdPool<IdType>::next()
{
    if(mReturned.empty())
    {
        return mNext++;
    }
    else
    {
        IdType result = *mReturned.begin();
        mReturned.erase(result);
        return result;
    }
}

template<typename IdType>
void IdPool<IdType>::free(const IdType& value)
{
    FEA_ASSERT(mReturned.count(value) == 0 && value < mNext, "Trying to free a nonexisting entry in id-pool. Entry '" + std::to_string(value) + "' does not exist!");

    mReturned.insert(value);
}
