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

#include "States/VacuumState.h"
#include "States/NetworkState.h"

// Shared state
SemaphoreHandle_t vss, nss, ss;
VacuumState vs = VacuumState::IDLE;
struct NetworkState ns{};

void setup() {
    Serial.begin(HardwareConfig::SERIAL_BAUD_RATE);

    vTaskDelay(pdTICKS_TO_MS(1000)); // wait for the serial to get configured to not miss any messages.
    
    vss = xSemaphoreCreateMutex();
    ss = xSemaphoreCreateMutex();

    if(xSemaphoreTake(ss, portMAX_DELAY))
    {
        Serial.println("--- VACUUM SYSTEM BOOT ---\n\n");
        Serial.println("[PINS] Initializing...");
        xSemaphoreGive(ss);
    }
    Pins::init();
    if(xSemaphoreTake(ss, portMAX_DELAY))
    {
        Serial.println("[PINS] Initialization finished.\n");
        xSemaphoreGive(ss);
    }
    vTaskDelay(pdTICKS_TO_MS(10));


    Vacuum::init(vs, vss, ss);
    vTaskDelay(pdTICKS_TO_MS(10));

    Network::init(ns, nss, ss);
    vTaskDelay(pdTICKS_TO_MS(10));

    Feedback::init(ns, nss, ss);

    if(xSemaphoreTake(ss, portMAX_DELAY))
    {
        Serial.println("\n\n--- VACUUM SYSTEM BOOT FINISHED ---\n\n");
        xSemaphoreGive(ss);
    }
}

void loop() {

}