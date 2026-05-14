// AppState.h
#ifndef APP_STATE_H
#define APP_STATE_H

#include <cstdint>

#include "Configs/VacuumConfig.h"
#include "AppErrorState.h"

using namespace VacuumConfig;

struct AppState
{
    VacuumState vacuum_state;
    VacuumCommand vacuum_command;
    AppErrorState error_state;
    bool isReady;
};

#endif