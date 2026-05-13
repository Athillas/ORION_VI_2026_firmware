// Feedback.h
#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <PubSubClient.h>
#include "States/NetworkState.h"
#include "Configs/NetworkConfig.h"

namespace Feedback
{
    void init(char (&feedback_message)[NetworkConfig::MAX_JSON_PAYLOAD], SemaphoreHandle_t &ss, SemaphoreHandle_t &fms);
    constexpr uint32_t FEEDBACK_TASK_TIMEOUT = 100;
}

#endif