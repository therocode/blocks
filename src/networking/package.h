#pragma once
#include <vector>
#include <stdint.h>
#include <typeindex>
#include <tuple>
#include "../utilities/serialize.h"

class BasePackage
{
    public:
        BasePackage();
        BasePackage(std::type_index type);
        virtual std::vector<uint8_t> serialise() const = 0;
        virtual void deserialise(const std::vector<uint8_t>& bytes) = 0;
        std::type_index mType;
};

template<typename Tag, typename... Types>
class Package : public BasePackage
{
    public:
        Package(Types... values) : BasePackage(typeid(Package)), data(values...)
        {
        }
        Package(std::tuple<Types...> value) : BasePackage(typeid(Package)), data(value)
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
