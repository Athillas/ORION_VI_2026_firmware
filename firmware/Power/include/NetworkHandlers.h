// NetworkHandlers.h
#ifndef NETWORK_HANDLERS_H
#define NETWORK_HANDLERS_H

#include <ArduinoJson.h>
#include "Configs/NetworkConfig.h"
#include "States/HardwareCommandState.h"

namespace NetworkHandlers
{
    // struct NetworkHandler
    // {
    //     char* topic;
    //     void (*handler) (void);
    // };
    void controlCmdHandler(StaticJsonDocument<NetworkConfig::MAX_JSON_PAYLOAD> &doc, struct HardwareCommandState &hcs);
}

#endif