#pragma once
#include <vector>
#include <stdint.h>
#include <iostream>
#include <tuple>
#include "../utilities/serialize.h"
#include "packagetype.h"

class BasePackage
{
    public:
        BasePackage();
        BasePackage(PackageType type, bool unreliable = false, int channel = 0);
        virtual std::vector<uint8_t> serialise() const = 0;
        virtual void deserialise(const std::vector<uint8_t>& bytes) = 0;
        PackageType mType;
        bool mUnreliable;
        int  mChannel = 0;
};

template<typename Tag, PackageType TypeId, bool Unreliable, int Channel, typename... Types>
class Package : public BasePackage
{
    public:
        Package()
        {
        }
        Package(Types... values) : BasePackage(TypeId, Unreliable, Channel), data(values...)
        {
        }
        Package(std::tuple<Types...> value) : BasePackage(TypeId, Unreliable, Channel), data(value)
        {
        }
        virtual std::vector<uint8_t> serialise() const override
        {
            std::vector<uint8_t> bytes;
            
            uint8_t* typePointer = (uint8_t*)&mType;
            for(uint32_t i = 0; i < sizeof(PackageType); i++)
            {
                bytes.push_back(*typePointer);
                typePointer++;
            }

            serialize(data, bytes);
            return bytes;
        }
        virtual void deserialise(const std::vector<uint8_t>& bytes) override
        {
            auto byteIterator = bytes.begin();

            uint8_t* typePointer = (uint8_t*)&mType;
            for(uint32_t i = 0; i < sizeof(PackageType); i++)
            {
                *typePointer = *byteIterator;
                byteIterator++;
                typePointer++;
            }

            data = deserialize<std::tuple<Types...> >(byteIterator, bytes.end());
        }
        std::tuple<Types...> getData()
        {
            return data;
        }
    private:
        std::tuple<Types...> data;
};
