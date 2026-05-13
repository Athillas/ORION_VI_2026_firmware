// Network.cpp
#include <Arduino.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

#include "Network.h"

#include "Configs/NetworkConfig.h"

#include "States/NetworkState.h"

namespace
{
    uint8_t MAC_[6];
    uint8_t IP_[4];

    struct NetworkState ns_{};

    SemaphoreHandle_t *ss_ = nullptr;  // Added for serial protection
    SemaphoreHandle_t *fms_ = nullptr;
    char *feedbackMessage_ = nullptr;
}

void callback(const char* topic, const byte* payload, uint16_t length);
void handleNetworkTask(void *parameters);

void Network::init(char (&feedback_message)[NetworkConfig::MAX_JSON_PAYLOAD], SemaphoreHandle_t &ss, SemaphoreHandle_t &fms)
{
    if(xSemaphoreTake(ss, portMAX_DELAY) == pdTRUE)
    {
        Serial.println("[MQTT] Initializing...");
        ss_ = &ss;
        xSemaphoreGive(ss);
    }

    fms_ = &fms;
    snprintf(feedbackMessage_, NetworkConfig::MAX_JSON_PAYLOAD, feedback_message);

    memcpy(MAC_, NetworkConfig::MAC, 6);
    memcpy(IP_, NetworkConfig::IP, 4);

    Ethernet.begin(MAC_, IP_);

    bool isHardwareAlive = Ethernet.hardwareStatus() != EthernetHardwareStatus::EthernetNoHardware;

    if(xSemaphoreTake(ss, portMAX_DELAY) == pdTRUE)
    {
        if(isHardwareAlive)
        {
            Serial.print("[MQTT] Hardware connected. IP: "); Serial.print(Ethernet.localIP()); Serial.println(); 
        }
        else
        {
            ns_.commsAlive = false;
            Serial.print("[MQTT] No WizNet found, recheck the wiring!");
        }
        xSemaphoreGive(ss);
    }
    ns_.client.connect(NetworkConfig::MQTT_ID);
    ns_.client.setCallback(callback);
    ns_.client.setBufferSize(NetworkConfig::MAX_JSON_PAYLOAD);
}

void callback(const char* topic, const byte* payload, uint16_t length)
{
    if(length > NetworkConfig::MAX_JSON_PAYLOAD && xSemaphoreTake(*ss_, portMAX_DELAY) == pdTRUE)
    {
        Serial.println("[CALLBACK] Packet size exceeds the MAX_JSON_PAYLOAD!");
        xSemaphoreGive(*ss_);
        return;
    }
}

void handleNetworkTask(void *parameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        struct NetworkConfig::MqttFeedbackMessage msg;
        
        if(!ns_.client.connected())
        {
            if(xSemaphoreTake(*ss_, portMAX_DELAY))
            {
                Serial.println("[HandleNetwork] MQTT client is disconnected!");
                Serial.print("\n[HandleNetwork] Attempting to connect as: ");Serial.print(NetworkConfig::MQTT_ID);Serial.println();
                xSemaphoreGive(*ss_);
            }

            if(ns_.client.connect(NetworkConfig::MQTT_ID))
            {
                ns_.client.subscribe(NetworkConfig::TOPIC_CMD);
            }
            else if(xSemaphoreTake(*ss_, portMAX_DELAY) == pdTRUE)
            {
                Serial.print("[MQTT] Failed to connect, rc="); Serial.println(ns_.client.state());
                xSemaphoreGive(*ss_);
            }
        }
        else
        {
            ns_.client.loop();
            char feedbackMessageCpy[NetworkConfig::MAX_JSON_PAYLOAD];

            if(xSemaphoreTake(*fms_, portMAX_DELAY) == pdTRUE)
            {
                snprintf(feedbackMessageCpy, NetworkConfig::MAX_JSON_PAYLOAD, feedbackMessage_);
                xSemaphoreGive(*fms_);
            }

            ns_.client.publish(
                NetworkConfig::TOPIC_ENC,
                feedbackMessageCpy,
                false
            );
        }
        
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(Network::HANDLE_NETWORK_TASK_TIMEOUT));
    }
}