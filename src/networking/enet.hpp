#pragma once

class ENet
{
    public:
        ENet();
        ~ENet();
        bool isInitialized() const;
    private:
        bool mInitialized;
};
