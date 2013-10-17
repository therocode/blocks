#pragma once
#include <vector>
#include <stdint.h>
#include <typeindex>
#include <featherkit/messaging.h>

class BasePackage
{
    public:
        BasePackage();
        BasePackage(std::type_index type);
        virtual std::vector<uint8_t> serialise() const = 0;
        virtual void deserialise(std::vector<uint8_t> bytes) = 0;
        std::type_index mType;
};

template<typename... Types>
class Package
{
    private:
        fea::Message<Types...> message;
};
