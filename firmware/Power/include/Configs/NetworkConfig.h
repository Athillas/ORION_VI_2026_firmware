// NetworkConfig.h
#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include <cstdint>

namespace NetworkConfig
{
    // --- Network adresses ---
    constexpr const char* MQTT_SERVER_IP          = "192.168.1.1"; 
    constexpr const char* MQTT_SERVER_ID          = "ESP32_ODrive";
    inline byte MAC[6]; // non-const because of Ethernet.begin type limitations. Defined in Network.cpp.
    inline byte IP[4];  // same as ^.

    // --- MQTT configuration ---
    constexpr uint16_t MQTT_PORT                  = 1883;
    constexpr uint16_t MQTT_FEEDBACK_DELAY        = 100;
    
    // --- MQTT Topics ---
    constexpr char* const TOPIC_POWER_CONTROL_ENC = "Power_control_enc";
    constexpr char* const TOPIC_POWER_CONTROL_CMD = "Power_control_cmd";

    // --- Network Safety ---
    constexpr uint16_t MQTT_SAFETY_TIMEOUT        = 1000;
    constexpr uint16_t RECONNECTION_TIMEOUT       = 5000;
    constexpr uint16_t MAX_JSON_PAYLOAD           = 256;
}

#endif