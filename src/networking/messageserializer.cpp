#include "messageserializer.hpp"

MessageType decodeType(const ByteVector& data)
{
    if(data.size() >= sizeof(MessageType))
    {
        MessageType* typePointer = (MessageType*)&(*(data.end() - sizeof(MessageType)));

        return *typePointer;
    }
    else
        return -1;
}
