#pragma once
#include <fea/assert.hpp>
#include <unordered_map>
#include <stack>

template<typename Type, typename IdType = uint32_t>
class IdMapper
{
    public:
        IdMapper();
        IdType getId(const Type& value);
        IdType getIdConst(const Type& value) const;
        void free(const Type& value);
        const Type& valueFromId(IdType id) const;
        bool valueExists(const Type& value) const;
    private:
        IdType getNext();
        IdType mNext;
        std::stack<IdType> mReturned;
        std::unordered_map<Type, IdType> mIds;
        std::unordered_map<IdType, Type> mValues;
};

template<typename Type, typename IdType>
IdMapper<Type, IdType>::IdMapper() :
    mNext(0)
{
}

template<typename Type, typename IdType>
IdType IdMapper<Type, IdType>::getId(const Type& value)
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
IdType IdMapper<Type, IdType>::getIdConst(const Type& value) const
{
    FEA_ASSERT(mIds.count(value) != 0, "Trying const access a non-existing value.");
    const auto& iterator = mIds.find(value);

    return mIds.at(value);
}

template<typename Type, typename IdType>
void IdMapper<Type, IdType>::free(const Type& value)
{
    FEA_ASSERT(mIds.count(value) != 0, "Trying to free a nonexisting entry in id-pool.");
    const auto& iterator = mIds.find(value);

    mReturned.push(iterator->second);
    mValues.erase(iterator->second);
    mIds.erase(iterator);
}

template<typename Type, typename IdType>
const Type& IdMapper<Type, IdType>::valueFromId(IdType id) const
{
    FEA_ASSERT(mValues.count(id) > 0, "Error, cannot get value of ID " + std::to_string(id) + ". Invalid ID");
    return mValues.at(id);
}

template<typename Type, typename IdType>
IdType IdMapper<Type, IdType>::getNext()
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
    
template<typename Type, typename IdType>
bool IdMapper<Type, IdType>::valueExists(const Type& value) const
{
    return mIds.count(value) != 0;
}
