#pragma once
#include <vector>
#include <stdint.h>
#include <typeindex>
#include <tuple>
#include "../utilities/serialize.h"
#include "packagetype.h"

class BasePackage
{
    public:
        BasePackage();
        BasePackage(PackageType type);
        virtual std::vector<uint8_t> serialise() const = 0;
        virtual void deserialise(const std::vector<uint8_t>& bytes) = 0;
        PackageType mType;
};

template<typename Tag, PackageType TypeId, typename... Types>
class Package : public BasePackage
{
    public:
        Package(Types... values) : BasePackage(TypeId), data(values...)
        {
        }
        Package(std::tuple<Types...> value) : BasePackage(TypeId), data(value)
        {
        }
        virtual std::vector<uint8_t> serialise() const override
        {
            std::vector<uint8_t> bytes;
            serialize(data, bytes);
            return bytes;
        }
        virtual void deserialise(const std::vector<uint8_t>& bytes) override
        {
            data = deserialize<std::tuple<Types...> >(bytes);
        }
        std::tuple<Types...> getData()
        {
            return data;
        }
    private:
        std::tuple<Types...> data;
};
