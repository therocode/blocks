#pragma once
#include <vector>
#include <stdint.h>
#include <typeindex>
#include <featherkit/messaging.h>
#include "serialize.h"

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
        Package(Types... values) : BasePackage(typeid(Package)), message(values...)
        {
        }
        virtual std::vector<uint8_t> serialise() const override
        {
            std::vector<uint8_t> bytes;
            serialize(message.data, bytes);
            return bytes;
        }
        virtual void deserialise(const std::vector<uint8_t>& bytes) override
        {
            message.data = deserialize<std::tuple<Types...> >(bytes);
        }
    private:
        fea::Message<Tag, Types...> message;
};
