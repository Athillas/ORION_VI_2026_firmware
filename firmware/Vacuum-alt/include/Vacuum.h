// Vacuum.h
#ifndef VACUUM_H
#define VACUUM_H

#include <cstdint>
#include "States/AppState.h"

namespace Vacuum
{
    void init(struct AppState &app_state, SemaphoreHandle_t &app_state_mutex, SemaphoreHandle_t &serial_mutex);
    constexpr uint32_t VACUUM_TASK_TIMEOUT = 100;
}

#endif