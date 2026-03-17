#include <Arduino.h>

// included from ../shared_libs/
#include <ACS712XX.h>
#include <Adafruit_NeoPixel.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <CAN.h>
//

#include <SPI.h>

#include <DS18B20.h>

#include "Pins.h"
#include "Config.h"

// MQTT
EthernetClient client;
PubSubClient mqtt(client);

// Amper-meters
ACS712XX WL(ACS712_30A, Pins::ADC_WL_PIN, Config::ADC_REF, Config::ADC_FS);
ACS712XX WR(ACS712_30A, Pins::ADC_WR_PIN, Config::ADC_REF, Config::ADC_FS);
ACS712XX RAM(ACS712_30A, Pins::ADC_RAM_PIN, Config::ADC_REF, Config::ADC_FS);
ACS712XX ELEK(ACS712_30A, Pins::ADC_ELEK_PIN, Config::ADC_REF, Config::ADC_FS);
ACS712XX SCI(ACS712_30A, Pins::ADC_SCI_PIN, Config::ADC_REF, Config::ADC_FS);
ACS712XX INNE(ACS712_30A, Pins::ADC_INNE_PIN, Config::ADC_REF, Config::ADC_FS);

DS18B20 DS18B20_T1(Pins::TEMP_PIN);

Adafruit_NeoPixel NeoPixelConnector;

void setup() {
    NeoPixelConnector.setPin(Pins::NEOPIXEL_PIN);

    CAN.begin(Config::BAUD_RATE);
    CAN.setPins(Pins::RX_CAN_PIN, Pins::TX_CAN_PIN);

    SPI.begin(Pins::WIZ_SCLK_PIN, Pins::WIZ_MISO_PIN,
        Pins::WIZ_MOSI_PIN, Pins::WIZ_SCN_PIN);


    if (Ethernet.begin(Config::ETHERNET_MAC) == 0) {
        Serial.println("DHCP failed");
    } else {
        Serial.print("IP: ");
        Serial.println(Ethernet.localIP());
    }
}

void loop() {
}