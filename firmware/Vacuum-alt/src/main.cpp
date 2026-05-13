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

#include "States/VacuumState.h"
#include "States/NetworkState.h"

// Shared state
SemaphoreHandle_t vss, ss, fms; // Vaacum State, Serial and Feedback Message Semaphores

VacuumState vs = VacuumState::IDLE;

char feedback_message[NetworkConfig::MAX_JSON_PAYLOAD] {0};

void setup()
{
    Serial.begin(HardwareConfig::SERIAL_BAUD_RATE);

    vTaskDelay(pdTICKS_TO_MS(1000)); // wait for the serial to get configured to not miss any messages.

    vss = xSemaphoreCreateMutex();
    ss = xSemaphoreCreateMutex();

    Serial.println("--- VACUUM SYSTEM BOOT ---\n\n");

    Serial.println("[PINS] Initializing...");
    Pins::init();
    Serial.println("[PINS] Initialization finished.\n");
    vTaskDelay(pdTICKS_TO_MS(10));

    Vacuum::init(vs, vss, ss);
    vTaskDelay(pdTICKS_TO_MS(10));

    Network::init(feedback_message, ss, fms);
    vTaskDelay(pdTICKS_TO_MS(10));

    Feedback::init(feedback_message, ss, fms);

    if(xSemaphoreTake(ss, portMAX_DELAY))
    {
        Serial.println("\n\n--- VACUUM SYSTEM BOOT FINISHED ---\n\n");
        xSemaphoreGive(ss);
    }
    vTaskDelete(NULL);
}

void loop()
{

    Serial.println("Shouldn't get there...");
}