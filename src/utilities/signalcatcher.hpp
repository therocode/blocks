#pragma once

#if defined(__linux)
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#endif

enum Signal { NO_SIGNAL, QUIT_SIGNAL };

class SignalCatcher
{
    public:
        SignalCatcher();
        Signal getSignal() const;

#if defined(__linux)
        struct sigaction sigIntHandler;
#endif
};
