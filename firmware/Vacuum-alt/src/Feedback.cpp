// Feedback.cpp

#include "Feedback.h"

namespace
{
    struct TaskParams
    {
        NetworkState *ns;
        SemaphoreHandle_t *nss;
    };
    
    struct TaskParams ftp;
}

void feedbackTask(void *parameters);

void Feedback::init(struct NetworkState &ns, SemaphoreHandle_t &nsSemaphore, SemaphoreHandle_t &ss)
{
    Serial.println("[FEEDBACK] Initializing...");
    ftp.ns = &ns;
    ftp.nss = &nsSemaphore;

    xTaskCreate(
        feedbackTask,
        "FeedbackTask",
        1024,
        &ftp,
        1,
        NULL
    );
    Serial.println("[FEEDBACK] Initialization finished.\n");
}

void feedbackTask(void *parameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();
	struct TaskParams *ntp = (struct TaskParams *) parameters;

    while(1)
    {
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(Feedback::FEEDBACK_TASK_TIMEOUT));
    }
}