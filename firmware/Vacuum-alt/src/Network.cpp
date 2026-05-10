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

    NetworkState *ns_ = nullptr;
    SemaphoreHandle_t *nss_ = nullptr;
    SemaphoreHandle_t *ss_ = nullptr;  // Added for serial protection
}

void callback(const char* topic, const byte* payload, uint16_t length);
void handleNetworkTask(void *parameters);

static void logMessage(const char* msg);
static void logError(const char* msg, int code);
static bool reconnectMqtt();
static void handleConnectedClient(PubSubClient* client);
static void handleDisconnectedClient();

void Network::init(struct NetworkState &ns, SemaphoreHandle_t &nss, SemaphoreHandle_t &ss)
{
	if(xSemaphoreTake(ss, portMAX_DELAY))
	{
        Serial.println("[NETWORK] Initializing...");
		xSemaphoreGive(ss);
	}

	if(xSemaphoreTake(ss, portMAX_DELAY))
	{
		ss_ = &ss;
	}
	// Initialize task params
    if (xSemaphoreTake(nss, portMAX_DELAY))
    {
        ns_ = &ns;
        nss_ = &nss;
        xSemaphoreGive(nss);
    }

    // Initialize network config (no semaphore needed as task isn't running yet)
    memcpy(MAC_, NetworkConfig::MAC, 6);
    memcpy(IP_, NetworkConfig::IP, 4);

    Ethernet.begin(MAC_, IP_);

    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        if (xSemaphoreTake(nss, portMAX_DELAY))
        {
            ns.commsAlive = false;
            xSemaphoreGive(nss);
        }
        
        if (ss != nullptr)
        {
            xSemaphoreTake(ss, portMAX_DELAY);
            Serial.println("[ETHERNET] ERROR: W5500 not found!");
            xSemaphoreGive(ss);
        }
        return;
    }
    else
    {
		xSemaphoreTake(ss, portMAX_DELAY);
		Serial.print("[ETHERNET] Ethernet IP: "); 
		Serial.println(Ethernet.localIP());
		xSemaphoreGive(ss);
    }

    // Setup MQTT client
    if (xSemaphoreTake(nss, portMAX_DELAY))
    {
        ns.client.setServer(NetworkConfig::MQTT_SERVER_IP, NetworkConfig::MQTT_PORT);
        ns.client.setCallback(callback);
        ns.client.setBufferSize(NetworkConfig::MAX_JSON_PAYLOAD);
        xSemaphoreGive(nss);
    }

    // Create network task
    if (!xTaskCreate(
        handleNetworkTask,
        "HandleNetwork",
        2048,  // Increased stack size for safety
        NULL,
        1,
        NULL
    ))
    {
		xSemaphoreTake(ss, portMAX_DELAY);
		Serial.println("[NETWORK] ERROR: Failed to create the HandleNetwork task! Try rebooting the board.");
		xSemaphoreGive(ss);
        return;
    }

	xSemaphoreTake(ss, portMAX_DELAY);
	Serial.println("[NETWORK] Initialization finished.\n");
	xSemaphoreGive(ss);
}

void callback(const char* topic, const byte* payload, uint16_t length)
{
	if(ns_ == nullptr || nss_ == nullptr)
	{
		xSemaphoreTake(*ss_, portMAX_DELAY);
		Serial.println("[MQTT] Network is not initialized");
		xSemaphoreGive(*ss_);
	}

    // Quick validation without needing semaphore
    if (length > NetworkConfig::MAX_JSON_PAYLOAD)
    {
		xSemaphoreTake(*ss_, portMAX_DELAY);
		Serial.println("[MQTT] ERROR: Payload is too big!");
		xSemaphoreGive(*ss_);
        return; 
    }
    
    if (strcmp(topic, NetworkConfig::TOPIC_CMD) != 0) return;

    // Parse JSON (no semaphore needed - using local variables)
    StaticJsonDocument<NetworkConfig::MAX_JSON_PAYLOAD> doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error)
    {
		xSemaphoreTake(*ss_, portMAX_DELAY);
		Serial.print("[MQTT] Failed to deserialize the JSON! "); 
		Serial.println(error.c_str());
		xSemaphoreGive(*ss_);
        return;
    }
    
	xSemaphoreTake(*nss_, pdMS_TO_TICKS(50));
	ns_->lastMqttCmdTime = millis();
	xSemaphoreGive(*nss_);
}

void handleNetworkTask(void *parameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();

    while (1)
    {
        if (nss_ == nullptr || ns_ == nullptr)
        {
            vTaskDelay(pdMS_TO_TICKS(Network::HANDLE_NETWORK_TASK_TIMEOUT));
            continue;
        }

        if (!xSemaphoreTake(*nss_, pdMS_TO_TICKS(100)))
        {
            logMessage("[HandleNetwork] WARNING: Could not acquire semaphore");

            vTaskDelayUntil(
                &lastWakeTime,
                pdMS_TO_TICKS(Network::HANDLE_NETWORK_TASK_TIMEOUT)
            );

            continue;
        }

        PubSubClient* client = &(ns_->client);
        bool connected = client->connected();

        xSemaphoreGive(*nss_);

        if (connected)
            handleConnectedClient(client);
        else
            handleDisconnectedClient();

        vTaskDelayUntil(
            &lastWakeTime,
            pdMS_TO_TICKS(Network::HANDLE_NETWORK_TASK_TIMEOUT)
        );
    }
}

static void logMessage(const char* msg)
{
    if (ss_ == nullptr || *ss_ == nullptr)
        return;

    xSemaphoreTake(*ss_, portMAX_DELAY);
    Serial.println(msg);
    xSemaphoreGive(*ss_);
}

static void logError(const char* msg, int code)
{
    if (ss_ == nullptr || *ss_ == nullptr)
        return;

    xSemaphoreTake(*ss_, portMAX_DELAY);
    Serial.print(msg);
    Serial.println(code);
    xSemaphoreGive(*ss_);
}

static bool reconnectMqtt()
{
    if (!xSemaphoreTake(*nss_, pdMS_TO_TICKS(50)))
    {
        logMessage("[HandleNetwork] ERROR: Could not acquire semaphore for reconnection");
        return false;
    }

    PubSubClient* client = &(ns_->client);

    bool connected = client->connect(NetworkConfig::MQTT_SERVER_ID);

    if (!connected)
    {
        ns_->commsAlive = false;

        logError(
            "[HandleNetwork] ERROR: Failed to connect to MQTT server, rc=",
            client->state()
        );

        xSemaphoreGive(*nss_);
        return false;
    }

    ns_->commsAlive = true;

    client->subscribe(NetworkConfig::TOPIC_CMD);

    xSemaphoreGive(*nss_);

    logMessage("[HandleNetwork] Successfully connected to the MQTT server!");

    return true;
}

static void handleConnectedClient(PubSubClient* client)
{
    if (client->loop())
        return;

    logMessage("[HandleNetwork] ERROR: Failed to run callback loop!");

    logError(
        "[HandleNetwork] Client state: ",
        client->state()
    );

    if (xSemaphoreTake(*nss_, pdMS_TO_TICKS(50)))
    {
        ns_->commsAlive = false;
        xSemaphoreGive(*nss_);
    }
}

static void handleDisconnectedClient()
{
    logMessage(
        "[HandleNetwork] ERROR: Lost connection with MQTT server, attempting to reconnect..."
    );

    reconnectMqtt();
}