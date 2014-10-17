#pragma once
#include <unordered_map>
#include <stack>

template<typename Type, typename IdType = uint32_t>
class IdProvider
{
    public:
        IdProvider();
        IdType getId(const Type& value);
        void free(IdType id);
    private:
        IdType getNext();
        IdType mNext;
        std::stack<IdType> mReturned;
        std::unordered_map<Type, IdType> mIds;
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
        return mIds.emplace(value, getNext()).first->second;
    }
    else
    {
        return mIds.at(value);
    }
}

template<typename Type, typename IdType>
void IdProvider<Type, IdType>::free(IdType id)
{
    if(mIds.erase(id) > 0)
        mReturned.push(id);
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
