#pragma once
#include <stdint.h>

class ScriptEntityCore
{
    private:
        int32_t refCount;

    public:
        ScriptEntityCore()
        {
            refCount = 1;
        }
        static ScriptEntityCore* asFactory()
        {
            return new ScriptEntityCore();
        }
        void addRef()
        {
            refCount++;
        }
        void release()
        {
            refCount--;
            if( refCount <= 0 )
                delete this;
        }
};
