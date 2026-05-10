// Vacuum.h
#ifndef VACUUM_H
#define VACUUM_H

#include <cstdint>
#include "States/VacuumState.h"

namespace Vacuum
{
    void init(VacuumState &vs, SemaphoreHandle_t &vss, SemaphoreHandle_t &ss);
    constexpr uint32_t VACUUM_TASK_TIMEOUT = 100;
}

#endif