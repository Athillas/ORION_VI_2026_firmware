// Network.cpp
#include <Arduino.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

#include "Network.h"

#include "Configs/NetworkConfig.h"
#include "Configs/VacuumConfig.h"

#include "States/NetworkState.h"

namespace
{
    uint8_t MAC_[6];
    uint8_t IP_[4];

    struct NetworkState ns_{};
    
    struct AppState *as_ = nullptr;

    SemaphoreHandle_t *as_mutex_ = nullptr;
    SemaphoreHandle_t *serial_mutex_ = nullptr;  // Added for serial protection
    SemaphoreHandle_t *fm_mutex_ = nullptr;
    char *feedback_message_ = nullptr;
}

void callback(const char* topic, const byte* payload, uint16_t length);
void handleNetworkTask(void *parameters);

void Network::init(char (&feedback_message)[NetworkConfig::MAX_JSON_PAYLOAD], struct AppState &app_state, SemaphoreHandle_t &app_state_mutex, SemaphoreHandle_t &serial_mutex, SemaphoreHandle_t &feedback_message_mutex)
{
    serial_mutex_ = &serial_mutex;
    as_mutex_ = &app_state_mutex;
    fm_mutex_ = &feedback_message_mutex;

    if(xSemaphoreTake(serial_mutex, portMAX_DELAY) == pdTRUE)
    {
        Serial.println("[Network::init] Initializing...");
        xSemaphoreGive(serial_mutex);
    }

    if(xSemaphoreTake(app_state_mutex, portMAX_DELAY))
    {
        as_ = &app_state;
        xSemaphoreGive(app_state_mutex);
    }

    if(xSemaphoreTake(feedback_message_mutex, portMAX_DELAY))
    {
        memcpy(feedback_message_, feedback_message, sizeof(feedback_message));
        xSemaphoreGive(feedback_message_mutex);
    }

    memcpy(MAC_, NetworkConfig::MAC, 6);
    memcpy(IP_, NetworkConfig::IP, 4);

    Ethernet.begin(MAC_, IP_);

    if(xSemaphoreTake(serial_mutex, portMAX_DELAY) == pdTRUE)
    {
        if(Ethernet.hardwareStatus() != EthernetHardwareStatus::EthernetNoHardware)
        {
            Serial.print("[Network::init] Hardware connected. IP: "); Serial.print(Ethernet.localIP()); Serial.println(); 
        }
        else
        {
            Serial.print("[Network::init] No WizNet found, recheck the wiring!");
        }
        xSemaphoreGive(serial_mutex);
    }

    ns_.client.connect(NetworkConfig::MQTT_ID);
    ns_.client.setCallback(callback);
    ns_.client.setBufferSize(NetworkConfig::MAX_JSON_PAYLOAD);
}

void callback(const char* topic, const byte* payload, uint16_t length)
{
    if(length > NetworkConfig::MAX_JSON_PAYLOAD && xSemaphoreTake(*serial_mutex_, portMAX_DELAY) == pdTRUE)
    {
        Serial.println("[CALLBACK] Packet size exceeds the MAX_JSON_PAYLOAD!");
        xSemaphoreGive(*serial_mutex_);
        return;
    }
    
    if(strncmp(topic, NetworkConfig::TOPIC_CMD, NetworkConfig::TOPIC_CMD_LENGTH) != 0)
    return;
    
    ns_.lastMqttCmdTime = millis();
    StaticJsonDocument<NetworkConfig::MAX_JSON_PAYLOAD> doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    
    if(error)
	{
        if(xSemaphoreTake(*serial_mutex_, portMAX_DELAY))
        {
            Serial.print("[CALLBACK] Failed to deserialize the JSON!"); Serial.println(error.c_str());
            xSemaphoreGive(*serial_mutex_);
        }
		return;
	}
    
    VacuumConfig::VacuumCommand vc_;
    JsonArray parsed_payload = doc.as<JsonArray>();
    // parsing app_state uint8_t, fallback value is 7 (static_cast<uint8_t> is needed, read https://arduinojson.org/v7/api/jsonvariant/or/)
    uint8_t vacuum_command = parsed_payload[0].as<uint8_t>() | static_cast<uint8_t>(7U); 
    if(xSemaphoreTake(*as_mutex_, portMAX_DELAY))
    {
        if(vacuum_command > VacuumConfig::VacuumCommand::CLEAR_ERRORS)
        {
            as_->error_state = AppErrorState::UNKNOWN_COMMAND_ERROR;
        }
        else
        {
            as_->vacuum_command = static_cast<VacuumConfig::VacuumCommand>(vacuum_command);
        }
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
            if(xSemaphoreTake(*serial_mutex_, portMAX_DELAY))
            {
                Serial.println("[HandleNetwork] MQTT client is disconnected!");
                Serial.print("\n[HandleNetwork] Attempting to connect app_state: ");Serial.print(NetworkConfig::MQTT_ID);Serial.println();
                xSemaphoreGive(*serial_mutex_);
            }

            if(ns_.client.connect(NetworkConfig::MQTT_ID))
            {
                ns_.client.subscribe(NetworkConfig::TOPIC_CMD);
            }
            else if(xSemaphoreTake(*serial_mutex_, portMAX_DELAY) == pdTRUE)
            {
                Serial.print("[HandleNetwork] Failed to connect, rc="); Serial.println(ns_.client.state());
                xSemaphoreGive(*serial_mutex_);
            }
        }
        else
        {
            if(!ns_.client.loop())
            {
                if(xSemaphoreTake(*serial_mutex_, portMAX_DELAY))
                {
                    Serial.println("[HandleNetwork] Failed to call client.loop().");
                    xSemaphoreGive(*serial_mutex_);
                }
            }
            char feedbackMessageCpy[NetworkConfig::MAX_JSON_PAYLOAD];

            if(xSemaphoreTake(*fm_mutex_, portMAX_DELAY) == pdTRUE)
            {
                memcpy(feedbackMessageCpy, feedback_message_, sizeof(*feedback_message_));
                xSemaphoreGive(*fm_mutex_);
            }

            if(!ns_.client.publish(
                NetworkConfig::TOPIC_ENC,
                feedbackMessageCpy,
                false
            ))
            {
                if(xSemaphoreTake(*serial_mutex_, portMAX_DELAY))
                {
                    Serial.println("[HandleNetwork] Failed to publish feedback message!");
                    xSemaphoreGive(*serial_mutex_);
                }
            }
        }
        
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(Network::HANDLE_NETWORK_TASK_TIMEOUT));
    }
}