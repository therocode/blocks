#include "signalcatcher.hpp"

#if defined(__linux)

Signal caughtSignal = NO_SIGNAL;

void quitHandler(int s)
{
    caughtSignal = QUIT_SIGNAL;
}

SignalCatcher::SignalCatcher()
{
    sigIntHandler.sa_handler = quitHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
}

Signal SignalCatcher::getSignal() const
{
    return caughtSignal;
}

#else
#warning "Signal catcher not implemented for this system"
SignalCatcher::SignalCatcher()
{
}

Signal SignalCatcher::getSignal() const
{
    return NO_SIGNAL;
}

#endif
