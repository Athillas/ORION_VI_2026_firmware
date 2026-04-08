// NetworkState.h
#ifndef NETWORK_STATE_H
#define NETWORK_STATE_H

#include <Arduino.h>
#include <cstdint>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "Configs/NetworkConfig.h"


struct NetworkState
{
    IPAddress ip;
    EthernetClient ethClient;
    PubSubClient client;
    uint32_t lastMqttCmdTime = 0;

    NetworkState() : client(ethClient) {
        ip = IPAddress(
            NetworkConfig::IP_[0], NetworkConfig::IP_[1], 
            NetworkConfig::IP_[2], NetworkConfig::IP_[3]
        );
    }
};

#endif