#include "../serializable.hpp"

enum MessageType { CLIENT_JOIN_REQUESTED };

//network protocol:
struct ClientJoinRequestedMessage : public Serializable
{
    public:
        std::vector<uint8_t> toBytes() const override;
        void fromBytes(const std::vector<uint8_t>& bytes) override;
    private:
        ClientJoinRequestedMessage 
};
