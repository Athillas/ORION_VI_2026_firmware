// NetworkState.h
#ifndef NETWORK_STATE_H
#define NETWORK_STATE_H

#include <Arduino.h>
#include <cstdint>
#include <Ethernet.h>
#include <PubSubClient.h>

#include "Configs/NetworkConfig.h"


struct NetworkState
{
    IPAddress ip;
    EthernetClient ethClient;
    PubSubClient client;
    StaticJsonDocument<NetworkConfig::MAX_JSON_PAYLOAD> feedback_doc;
    char feedback_buf[NetworkConfig::MAX_JSON_PAYLOAD];
    uint32_t lastMqttCmdTime = 0;

    NetworkState() : client(ethClient) {}
};

#endif