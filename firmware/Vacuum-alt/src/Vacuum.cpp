// Vacuum.cpp
#include <Arduino.h>
#include <cstdint>

#include <Servo.h>

#include "Vacuum.h"
#include "Pins.h"

#include "Configs/VacuumConfig.h"

namespace
{
    struct TaskParams
    {
        struct AppState *app_state;
        SemaphoreHandle_t *app_state_mutex;
        SemaphoreHandle_t *serial_mutex;
    };
    
    struct TaskParams task_params;

    Servo servoLid;
}

static void vacuum_task(void *parameters);

static void handle_to_idle();
static void handle_to_collecting();
static void handle_to_dropping();
static void handle_to_cleaning();
static void handle_to_abort();

void Vacuum::init(struct AppState &as, SemaphoreHandle_t &app_state_mutex, SemaphoreHandle_t &serial_mutex)
{
    if(xSemaphoreTake(serial_mutex, portMAX_DELAY))
    {
        Serial.println("[VACUUM] Initializing...");
        xSemaphoreGive(serial_mutex);
    }

    task_params.serial_mutex = &serial_mutex;
    task_params.app_state_mutex = &app_state_mutex;
    bool is_task_created = false;

    servoLid.attach(Pins::PWM_SERVO_PIN, 1000, 2000);
    
    if(xSemaphoreTake(app_state_mutex, portMAX_DELAY))
    {
        task_params.app_state = &as;
        xSemaphoreGive(app_state_mutex);
    }

    if(xSemaphoreTake(serial_mutex, portMAX_DELAY))
    {
        if(xTaskCreate(
            vacuum_task,
            "VACUUM",
            1024,
            &task_params,
            1,
            NULL
        ) == pdTRUE)
        {
            Serial.println("[VACUUM] Failed to create VACUUM task!");
        }
        else
        {
            Serial.println("[VACUUM] Initialization finished.\n");
        }
        xSemaphoreGive(serial_mutex);
    }
}

static void vacuum_task(void *parameters)
{
    TickType_t last_wake_time = xTaskGetTickCount();

    for(;;)
    {
        VacuumCommand vacuum_command_ = VacuumCommand::TO_IDLE;
        VacuumState vacuum_state_ = VacuumState::IDLE;

        if(xSemaphoreTake(*task_params.app_state_mutex, portMAX_DELAY))
        {
            vacuum_command_ = task_params.app_state->vacuum_command;
            vacuum_state_ = task_params.app_state->vacuum_state;
            xSemaphoreGive(*task_params.app_state_mutex);
        }

        switch (vacuum_command_)
        {
            case VacuumCommand::TO_IDLE:
                handle_to_idle();
                break;
            case VacuumCommand::TO_COLLECTING:
                handle_to_collecting();
                break;
            case VacuumCommand::TO_DROPPING:
                handle_to_dropping();
                break;
            case VacuumCommand::TO_ABORT:
                handle_to_abort();
                break;
            case VacuumCommand::CLEAR_ERRORS:
                if(xSemaphoreTake(*task_params.app_state_mutex, portMAX_DELAY))
                {
                    task_params.app_state->error_state = AppErrorState::NO_ERROR;
                    xSemaphoreGive(*task_params.app_state_mutex);
                }
                break;
        }

        switch (vacuum_state_)
        {
            
        }

        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(Vacuum::VACUUM_TASK_TIMEOUT));
    }
}

static void handle_to_idle()
{
    if(xSemaphoreTake(*task_params.app_state_mutex, portMAX_DELAY))
    {
        if(canTransition(task_params.app_state->vacuum_state, VacuumCommand::TO_IDLE))
        {

        }
        xSemaphoreGive(*task_params.app_state_mutex);
    }
}

static void handle_to_collecting()
{

}

static void handle_to_dropping()
{

}

static void handle_to_cleaning()
{
    
}

static void handle_to_abort()
{
    
}
