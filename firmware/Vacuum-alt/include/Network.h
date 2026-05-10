// Network.h
#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include "States/NetworkState.h"

namespace Network
{
    void init(struct NetworkState &ns, SemaphoreHandle_t &nss, SemaphoreHandle_t &ss);
    constexpr uint32_t HANDLE_NETWORK_TASK_TIMEOUT = 100;
}

#endif