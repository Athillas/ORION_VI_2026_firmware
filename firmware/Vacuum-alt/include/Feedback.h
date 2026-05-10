// Feedback.h
#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <PubSubClient.h>
#include "States/NetworkState.h"

namespace Feedback
{
    void init(struct NetworkState &ns, SemaphoreHandle_t &nss, SemaphoreHandle_t &ss);
    constexpr uint32_t FEEDBACK_TASK_TIMEOUT = 100;
}

#endif