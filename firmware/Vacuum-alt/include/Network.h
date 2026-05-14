// Network.h
#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>

#include "Configs/NetworkConfig.h"

#include "States/AppState.h"
#include "States/NetworkState.h"

namespace Network
{
    void init(char (&feedback_message)[NetworkConfig::MAX_JSON_PAYLOAD], struct AppState &app_state, SemaphoreHandle_t &app_state_mutex, SemaphoreHandle_t &serial_mutex, SemaphoreHandle_t &feedback_message_mutex);
    constexpr uint32_t HANDLE_NETWORK_TASK_TIMEOUT = 100;
}

#endif