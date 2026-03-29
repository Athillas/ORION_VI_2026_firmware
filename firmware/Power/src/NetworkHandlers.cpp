// NetworkHandlers.cpp
#include <Arduino.h>
#include "NetworkHandlers.h"

#include "States/HardwareCommandState.h"
#include "Configs/NetworkConfig.h"


void NetworkHandlers::controlCmdHandler(StaticJsonDocument<NetworkConfig::MAX_JSON_PAYLOAD> &doc, struct HardwareCommandState &hcs)
{
    if(doc.containsKey("S_W_L") && doc["S_W_L"].is<bool>())
    {
        hcs.S_W_L     = doc["S_W_L"];
    }

    if(doc.containsKey("S_W_R") && doc["S_W_R"].is<bool>())
    {
        hcs.S_W_R     = doc["S_W_R"];
    }

    if(doc.containsKey("S_INNE") && doc["S_INNE"].is<bool>())
    {
        hcs.S_INNE    = doc["S_INNE"];
    }

    if(doc.containsKey("S_SCI") && doc["S_SCI"].is<bool>())
    {
        hcs.S_SCI     = doc["S_SCI"];
    }

    if(doc.containsKey("S_ELEC") && doc["S_ELEC"].is<bool>())
    {
        hcs.S_ELEK    = doc["S_ELEC"];
    }

    if(doc.containsKey("S_RAM") && doc["S_RAM"].is<bool>())
    {
        hcs.S_RAM     = doc["S_RAM"];
    }

    if(doc.containsKey("FAN1_ON") && doc["FAN1_ON"].is<bool>())
    {
        hcs.FAN1_ON   = doc["FAN1_ON"];
    }

    if(doc.containsKey("NEOPIXEL") && doc["NEOPIXEL"].is<bool>())
    {
        hcs.NEOPIXEL  = doc["NEOPIXEL"];
    }
}