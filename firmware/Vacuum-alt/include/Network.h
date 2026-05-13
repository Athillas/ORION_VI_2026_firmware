// Network.h
#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include "States/NetworkState.h"
#include "Configs/NetworkConfig.h"

namespace Network
{
    void init(char (&feedback_message)[NetworkConfig::MAX_JSON_PAYLOAD], SemaphoreHandle_t &ss, SemaphoreHandle_t &fms);
    constexpr uint32_t HANDLE_NETWORK_TASK_TIMEOUT = 100;
}

#endif