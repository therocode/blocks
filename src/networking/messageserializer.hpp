#pragma once
#include <vector>
#include <memory>
#include <fea/assert.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/complex.hpp>
#include <stdexcept>

class DeserializeException : public std::runtime_error
{
    public:
        DeserializeException() :
            std::runtime_error("Error deserializing bytes")
    {
    }
};

using MessageType = int32_t;
using ByteVector = std::vector<uint8_t>;

template <typename Type>
ByteVector serializeMessage(const Type& message)
{
    MessageType type = message.getType();

    std::stringstream stream;

    cereal::PortableBinaryOutputArchive outArchive(stream);

    outArchive(message);

    std::string temp = stream.str();
    ByteVector result(temp.begin(), temp.end());

    uint8_t* typePointer = (uint8_t*)&type;

    for(uint8_t i = 0; i < sizeof(MessageType); i++)
    {
        result.push_back(*typePointer);
        typePointer++;
    }

    return result;
}

MessageType decodeType(const ByteVector& data);

template <typename Type>
Type deserializeMessage(const ByteVector& data)
{
    try
    {
        MessageType type = decodeType(data);

        std::stringstream stream(std::string(data.begin(), data.end() - sizeof(MessageType)));

        cereal::PortableBinaryInputArchive inArchive(stream);

        Type result;

        inArchive(result);

        FEA_ASSERT(type == result.getType(), "Deserializing something where the type is not correct!");

        return result;
    }
    catch (...)
    {
        throw DeserializeException();
    }
}
