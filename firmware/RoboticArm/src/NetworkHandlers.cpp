// NetworkHandlers.cpp
#include <Arduino.h>
#include <ArduinoJson.h>

#include "NetworkHandlers.h"

#include "States/NetworkState.h"
#include "States/HardwareCommandState.h"
#include "States/HardwareFeedbackState.h"

#include "Configs/NetworkConfig.h"


void NetworkHandlers::controlCmdHandler(StaticJsonDocument<NetworkConfig::MAX_JSON_PAYLOAD> &doc,
    struct NetworkState &ns,
    struct HardwareCommandState &hcs)
{
    if(doc.containsKey("S_W_L") && doc["S_W_L"].is<uint8_t>())
    {
        hcs.S_W_L     = doc["S_W_L"];
    }

    if(doc.containsKey("S_W_R") && doc["S_W_R"].is<uint8_t>())
    {
        hcs.S_W_R     = doc["S_W_R"];
    }

    if(doc.containsKey("S_INNE") && doc["S_INNE"].is<uint8_t>())
    {
        hcs.S_INNE    = doc["S_INNE"];
    }

    if(doc.containsKey("S_SCI") && doc["S_SCI"].is<uint8_t>())
    {
        hcs.S_SCI     = doc["S_SCI"];
    }

    if(doc.containsKey("S_ELEC") && doc["S_ELEC"].is<uint8_t>())
    {
        hcs.S_ELEK    = doc["S_ELEC"];
    }

    if(doc.containsKey("S_RAM") && doc["S_RAM"].is<uint8_t>())
    {
        hcs.S_RAM     = doc["S_RAM"];
    }

    if(doc.containsKey("FAN1_ON") && doc["FAN1_ON"].is<uint8_t>())
    {
        hcs.FAN1_ON   = doc["FAN1_ON"];
    }

    if(doc.containsKey("NEOPIXEL") && doc["NEOPIXEL"].is<uint8_t>())
    {
        hcs.NEOPIXEL  = doc["NEOPIXEL"];
    }
}

void NetworkHandlers::feedbackEncHandler(
    StaticJsonDocument<NetworkConfig::MAX_JSON_PAYLOAD> &doc,
    struct NetworkState &ns,
    const struct HardwareFeedbackState &hfs
)
{
    doc.clear();
    
    doc["eventType"] = NetworkConfig::TOPIC_ENC;

    auto payload = doc.createNestedObject("payload");

    payload["TEMP"]      = hfs.TEMP;
    payload["BAT_1_ADC"] = hfs.BAT_1_ADC;
    payload["BAT_2_ADC"] = hfs.BAT_2_ADC;
    payload["BAT_3_ADC"] = hfs.BAT_3_ADC;
    payload["BAT_4_ADC"] = hfs.BAT_4_ADC;
    payload["ADC_WL"]    = hfs.ADC_WL;
    payload["ADC_WR"]    = hfs.ADC_WR;
    payload["ADC_RAM"]   = hfs.ADC_RAM;
    payload["ADC_ELEK"]  = hfs.ADC_ELEK;
    payload["ADC_SCI"]   = hfs.ADC_SCI;
    payload["ADC_INNE"]  = hfs.ADC_INNE;

    char buffer[NetworkConfig::MAX_JSON_PAYLOAD];
    serializeJson(doc, buffer);

    if(!ns.client.publish(NetworkConfig::TOPIC_ENC, buffer))
    {
        Serial.println("[MQTT] Publish failure!");
    }
}

void NetworkHandlers::errorEncHandler(
        StaticJsonDocument<NetworkConfig::MAX_JSON_PAYLOAD> &doc,
        struct NetworkState &ns,
        const uint32_t errorDesc
    )
{
    doc.clear();
    doc["eventType"] = NetworkConfig::TOPIC_ENC;
    char errStr[12];
    snprintf(errStr, sizeof(errStr), "0x%X", errorDesc);
    doc["error"] = errStr;

    char buffer[NetworkConfig::MAX_JSON_PAYLOAD];
    serializeJson(doc, buffer);

    if(!ns.client.publish(NetworkConfig::TOPIC_ENC, buffer))
    {
        Serial.println("[MQTT] Error publish failure!");
    }
}