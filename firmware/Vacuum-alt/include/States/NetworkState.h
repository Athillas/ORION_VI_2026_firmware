// NetworkState.h
#ifndef NETWORK_STATE_H
#define NETWORK_STATE_H

#include <PubSubClient.h>

#include <cstdint>

struct NetworkState
{
    PubSubClient client;
    uint32_t lastMqttCmdTime;
    //bool commsAlive;
};

#endif