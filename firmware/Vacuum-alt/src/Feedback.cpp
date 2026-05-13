// Feedback.cpp

#include "Feedback.h"

namespace
{
    struct TaskParams
    {
        char *feedbackMessage;
        SemaphoreHandle_t *ss;
        SemaphoreHandle_t *fms;
    };
    
    struct TaskParams ftp;
}

void feedbackTask(void *parameters);

void Feedback::init(char (&feedback_message)[NetworkConfig::MAX_JSON_PAYLOAD], SemaphoreHandle_t &ss, SemaphoreHandle_t &fms)
{
    if(xSemaphoreTake(ss, portMAX_DELAY))
    {
        Serial.println("[FEEDBACK] Initializing...");
        ftp.ss = &ss;
        xSemaphoreGive(ss);
    }

    snprintf(ftp.feedbackMessage, NetworkConfig::MAX_JSON_PAYLOAD, feedback_message);
    ftp.fms = &fms; 

    if(xSemaphoreTake(ss, portMAX_DELAY))
    {
        if(xTaskCreate(
            feedbackTask,
            "FeedbackTask",
            1024,
            NULL,
            1,
            NULL
        ) != pdTRUE)
        {
            
            Serial.println("[FEEDBACK] Failed to create the feedback task!");
        }
        else
        {
            Serial.println("[FEEDBACK] Feedback task created successfully. Initialization complete.");
        }
        
        xSemaphoreGive(ss);
    }
}

void feedbackTask(void *parameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();

    while(1)
    {
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(Feedback::FEEDBACK_TASK_TIMEOUT));
    }
}