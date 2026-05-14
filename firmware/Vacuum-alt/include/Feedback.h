// Feedback.h
#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <PubSubClient.h>

#include "Configs/NetworkConfig.h"

#include "States/AppState.h"

namespace Feedback
{
    void init(char (&feedback_message)[NetworkConfig::MAX_JSON_PAYLOAD], AppState &app_state, SemaphoreHandle_t &serial_mutex, SemaphoreHandle_t &feedback_message_mutex, SemaphoreHandle_t &app_state_mutex);

    constexpr uint8_t FEEDBACK_TASK_TIMEOUT = 100;

    constexpr float AMMETER_VOLTS           = 3.3; // 3.3V for ESP32
    constexpr uint16_t AMMETER_MAX_ADC      = 4095; // 4096 for ESP32
    constexpr float AMMETER_MV_PER_AMPERE   = 185; // 185 for 5A ACS712 version (https://github.com/RobTillaart/ACS712)
    
}

#endif