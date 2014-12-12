#pragma once
#include <fea/assert.hpp>
#include <unordered_map>
#include <stack>

template<typename Type, typename IdType = uint32_t>
class IdProvider
{
    public:
        IdProvider();
        IdType getId(const Type& value);
        void free(const Type& value);
        const Type& valueFromId(IdType id) const;
    private:
        IdType getNext();
        IdType mNext;
        std::stack<IdType> mReturned;
        std::unordered_map<Type, IdType> mIds;
        std::unordered_map<IdType, Type> mValues;
};

template<typename Type, typename IdType>
IdProvider<Type, IdType>::IdProvider() :
    mNext(0)
{
}

template<typename Type, typename IdType>
IdType IdProvider<Type, IdType>::getId(const Type& value)
{
    const auto& iterator = mIds.find(value);

    if(iterator == mIds.end())
    {
        IdType newId = getNext();
        mValues.emplace(newId, value);
        return mIds.emplace(value, newId).first->second;
    }
    else
    {
        return mIds.at(value);
    }
}

template<typename Type, typename IdType>
void IdProvider<Type, IdType>::free(const Type& value)
{
    const auto& iterator = mIds.find(value);

    if(iterator != mIds.end())
    {
        mReturned.push(iterator->second);
        mValues.erase(iterator->second);
        mIds.erase(iterator);
    }
}

template<typename Type, typename IdType>
const Type& IdProvider<Type, IdType>::valueFromId(IdType id) const
{
    FEA_ASSERT(mValues.count(id) > 0, "Error, cannot get value of ID " + std::to_string(id) + ". Invalid ID");
    return mValues.at(id);
}

template<typename Type, typename IdType>
IdType IdProvider<Type, IdType>::getNext()
{
    if(mReturned.size() == 0)
        return mNext++;
    else
    {
        IdType next = mReturned.top();
        mReturned.pop();
        return next;
    }
}
