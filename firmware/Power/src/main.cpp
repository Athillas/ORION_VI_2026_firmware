//main.cpp
#include <Arduino.h>

// included from ../shared_libs/
// ---
#include <ACS712XX.h>
#include <Adafruit_NeoPixel.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <CAN.h>
// ---

#include <SPI.h>
#include <DS18B20.h>
#include <Pins.h>
#include <Configs/NetworkConfig.h>
#include <Configs/HardwareConfig.h>
#include <ArduinoJson.h>

#include "States/HardwareCommandState.h"
#include "States/HardwareState.h"
#include "States/NetworkState.h"
#include "Network.h"

// --- Hardware classes definitions ---

// Amper-meters
ACS712XX WL(ACS712_30A, Pins::ADC_WL_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS);
ACS712XX WR(ACS712_30A, Pins::ADC_WR_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS);
ACS712XX RAM(ACS712_30A, Pins::ADC_RAM_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS);
ACS712XX ELEK(ACS712_30A, Pins::ADC_ELEK_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS);
ACS712XX SCI(ACS712_30A, Pins::ADC_SCI_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS);
ACS712XX INNE(ACS712_30A, Pins::ADC_INNE_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS);


// Thermometer
DS18B20 DS18B20_T1(Pins::TEMP_PIN);

// NeoPixel
Adafruit_NeoPixel NeoPixelConnector;

// --- States ---

struct HardwareState hs {
    .TEMP        = 0.0f,
    .BAT_1_ADC   = 0,
    .BAT_2_ADC   = 0,
    .BAT_3_ADC   = 0,
    .BAT_4_ADC   = 0,
    .ADC_WL      = 0,
    .ADC_WR      = 0,
    .ADC_RAM     = 0,
    .ADC_ELEK    = 0,
    .ADC_SCI     = 0,
    .ADC_INNE    = 0,
};

struct HardwareCommandState hcs {
    .S_W_L       = false,
    .S_W_R       = false,
    .S_INNE      = false,
    .S_SCI       = false,
    .S_ELEK      = false,
    .S_RAM       = false,
    .FAN1_ON     = false,
    .NEOPIXEL    = false,
};

// Initiated via constructor, look in NetworkState struct definition.
struct NetworkState ns;

void initNetworkStateStruct();

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n>>> POWER CONTROL MODULE SYSTEM START <<<");

    NeoPixelConnector.setPin(Pins::NEOPIXEL_PIN);
    initNetworkStateStruct();
    Network::initNetwork(ns, hcs);

    Serial.println(">>> SETUP COMPLETE <<<");
}

void loop() {
    uint32_t now = millis();

    if(now - ns.lastMqttCmdTime > NetworkConfig::MQTT_SAFETY_TIMEOUT)
    {
        Serial.println("[WATCHDOG] !!! Connection loss - STOP !!!");
    }

    Network::handleNetwork();

    // CAN:
    // TODO.

    static uint32_t lastMqttFeedback = 0;
    if(now - lastMqttFeedback < NetworkConfig::MQTT_FEEDBACK_DELAY) return;

    Network::sendFeedbackMessage(hs);
}

void initNetworkStateStruct()
{
    ns.ip = IPAddress(
        NetworkConfig::IP[0], NetworkConfig::IP[1], 
        NetworkConfig::IP[2], NetworkConfig::IP[3]
    );
    ns.feedback_doc.clear();
    ns.feedback_buf[0] = '\0';
    ns.lastMqttCmdTime = 0;
}