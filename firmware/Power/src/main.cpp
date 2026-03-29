//main.cpp
#include <Arduino.h>

#include "Pins.h"
#include "Network.h"
#include "Hardware.h"

#include "Configs/NetworkConfig.h"
#include "Configs/HardwareConfig.h"

#include "States/HardwareState.h"
#include "States/HardwareCommandState.h"
#include "States/HardwareFeedbackState.h"
#include "States/NetworkState.h"

// --- States ---
// (Initialized via constructors)

struct HardwareFeedbackState hfs {};

struct HardwareCommandState hcs {};

struct NetworkState ns {};

struct HardwareState hs {};

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n>>> POWER CONTROL MODULE SYSTEM START <<<");

    // --- System initialization ---
    Network::initNetwork(ns, hcs);
    Hardware::initHardware();

    Serial.println(">>> SETUP COMPLETE <<<");
}

void loop() {
    uint32_t now = millis();

    if(now - ns.lastMqttCmdTime > NetworkConfig::MQTT_SAFETY_TIMEOUT)
    {
        Serial.println("[WATCHDOG] !!! Connection loss - STOP !!!");
    }

    Network::handleNetwork();

    // CAN:
    // TODO.

    static uint32_t lastMqttFeedback = 0;
    if(now - lastMqttFeedback < NetworkConfig::MQTT_FEEDBACK_DELAY) return;

    Network::sendFeedbackMessage(hfs);
}