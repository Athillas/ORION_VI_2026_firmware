// AppErrorState.h
#ifndef APP_ERROR_STATE_H
#define APP_ERROR_STATE_H

#include <cstdint>

enum AppErrorState : uint8_t
{
    NO_ERROR                = 0,
    HARDWARE_ERROR          = 1,
    STATE_TRANSITION_ERROR  = 2,
    UNKNOWN_COMMAND_ERROR   = 3
};

#endif