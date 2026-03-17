#include <Arduino.h>

// included from ../shared_libs/
#include <ACS712XX.h>
#include <Adafruit_NeoPixel.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <L298NX2.h>
//

#include <SPI.h>
#include <HX711.h>

#include "Pins.h"
#include "Config.h"


// MQTT
EthernetClient client;
PubSubClient mqtt(client);

L298NX2 L298N(
  Pins::INMOTOR1_PIN,
  Pins::INMOTOR2_PIN,
  Pins::INMOTOR3_PIN,
  Pins::INMOTOR4_PIN
);

ACS712XX ACS712_U1(ACS712_05B, Pins::VESP321_PIN, Config::ADC_REF, Config::ADC_FS);
ACS712XX ACS712_U19(ACS712_05B, Pins::VESP322_PIN, Config::ADC_REF, Config::ADC_FS);

HX711 HX711_U5;

Adafruit_NeoPixel NeoPixelConnector;

void setup() {
  HX711_U5.begin(Pins::DT_PIN, Pins::SCK2_PIN);
  NeoPixelConnector.setPin(Pins::NEOPIX_SIGNAL_PIN);

  SPI.begin(Pins::SCK_PIN, Pins::MISO_PIN,
    Pins::MOSI_PIN, Pins::CS_PIN);

  if (Ethernet.begin(Config::ETHERNET_MAC) == 0) {
    Serial.println("DHCP failed");
  } else {
    Serial.print("IP: ");
    Serial.println(Ethernet.localIP());
  }
}

void loop() {

}