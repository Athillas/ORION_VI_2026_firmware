#include <Arduino.h>


// included from ../shared_libs/
#include <ACS712XX.h>
#include <Adafruit_NeoPixel.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <L298NX2.h>
#include <CAN.h>
//

#include <SPI.h>
#include <Wire.h>

#include "Pins.h"
#include "Config.h"

// MQTT
EthernetClient client;
PubSubClient mqtt(client);

// !!! TODO:  Create a class with sensors !!!
L298NX2 L298N(Pins::ENABLE_PIN, Pins::IN_POS_PIN,
  Pins::IN_NEG_PIN, Pins::ENABLE_PIN,
  Pins::IN_POS_PIN, Pins::IN_NEG_PIN);
// For now, sensors are configured separately in Pins.h init_pins method.

ACS712XX ACS712(ACS712_30A, Pins::ADC_PIN,
  Config::ADC_REF, Config::ADC_FS);

Adafruit_NeoPixel NeoPixelConnector;

void setup() {
  NeoPixelConnector.setPin(Pins::NEOPIXEL_PIN);

  CAN.begin(Config::BAUD_RATE);
  CAN.setPins(Pins::RX_CAN_PIN, Pins::TX_CAN_PIN);

  Wire.begin(Pins::SDA_PIN, Pins::SCL_PIN);
  
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