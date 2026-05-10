// Vacuum.cpp

#include <Arduino.h>
#include <cstdint>

#include "Vacuum.h"

namespace
{
    struct TaskParams
    {
        VacuumState *vs;
        SemaphoreHandle_t *vss;
    };
    
    struct TaskParams vtp;
}

static void vacuum_task(void *parameters);


void Vacuum::init(VacuumState &vs, SemaphoreHandle_t &vss, SemaphoreHandle_t &ss)
{
    Serial.println("[VACUUM] Initializing...");

    vtp.vs = &vs;
    vtp.vss = &vss;
    
    if(xTaskCreate(
        vacuum_task,
        "VacuumTask",
        1024,
        &vtp,
        1,
        NULL
    ) != pdTRUE)
    {
        Serial.println("[VACUUM] Failed to create VacuumTask!");
    }
    else
    {
        Serial.println("[VACUUM] Initialization finished.\n");
    }
    
}

static void vacuum_task(void *parameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();
	struct TaskParams *ntp = (struct TaskParams *) parameters;

    while(1)
    {
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(Vacuum::VACUUM_TASK_TIMEOUT));
    }
}