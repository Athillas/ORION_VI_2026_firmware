// main.cpp
#include <Arduino.h>
#include <PubSubClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "Pins.h"
#include "Vacuum.h"
#include "Network.h"
#include "Feedback.h"

#include "Configs/HardwareConfig.h"
#include "Configs/NetworkConfig.h"

#include "States/AppState.h"
#include "States/NetworkState.h"
#include "States/AppErrorState.h"

AppState app_state = {
    .vacuum_state = VacuumState::IDLE,
    .vacuum_command = VacuumCommand::TO_IDLE,
    .error_state = AppErrorState::NO_ERROR,
    .isReady = false
};

char feedback_message[NetworkConfig::MAX_JSON_PAYLOAD] {0};

// Shared state
SemaphoreHandle_t app_state_mutex, serial_mutex, feedback_message_mutex; // App State, Serial and Feedback Message Semaphores

void setup()
{
    Serial.begin(HardwareConfig::SERIAL_BAUD_RATE);

    vTaskDelay(pdTICKS_TO_MS(1000)); // wait for the serial to get configured to not miss any messages.

    app_state_mutex = xSemaphoreCreateMutex();
    serial_mutex = xSemaphoreCreateMutex();
    feedback_message_mutex = xSemaphoreCreateMutex();

    Serial.println("--- VACUUM SYSTEM BOOT ---\n\n");

    Serial.println("[PINS] Initializing...");
    Pins::init();
    Serial.println("[PINS] Initialization finished.\n");
    vTaskDelay(pdTICKS_TO_MS(10));

    Vacuum::init(app_state, app_state_mutex, serial_mutex);
    vTaskDelay(pdTICKS_TO_MS(10));

    Network::init(feedback_message, app_state, app_state_mutex, serial_mutex, feedback_message_mutex);
    vTaskDelay(pdTICKS_TO_MS(10));

    Feedback::init(feedback_message, app_state, serial_mutex, feedback_message_mutex, app_state_mutex);

    if(xSemaphoreTake(serial_mutex, portMAX_DELAY))
    {
        Serial.println("\n\n--- VACUUM SYSTEM BOOT FINISHED ---\n\n");
        xSemaphoreGive(serial_mutex);
    }

    vTaskDelete(NULL);
}

void loop()
{
    Serial.println("Shouldn't get there...");
}