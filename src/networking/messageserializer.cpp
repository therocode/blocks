#include "messageserializer.hpp"

MessageType decodeType(const ByteVector& data)
{
    MessageType* typePointer = (MessageType*)&(*(data.end() - sizeof(MessageType)));

    return *typePointer;
}
