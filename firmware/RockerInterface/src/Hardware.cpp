// Hardware.cpp
#include "Hardware.h"
#include <Arduino.h>

// included from ../shared_libs/
// ---
#include <ACS712.h>
#include <Adafruit_NeoPixel.h>
// ---

#include "Pins.h"
#include "Configs/HardwareConfig.h"
#include "States/HardwareState.h"

void Hardware::initHardware()
{
    /*
    Currently, Serial port is used for debugging. Anyways, RX2 and TX2 are not used anywhere for now.
    
    Serial.begin(HardwareConfig::SERIAL_BAUD, SERIAL_8N1, Pins::RX0_PIN, Pins::TX0_PIN);
    Serial1.begin(HardwareConfig::SERIAL_BAUD, SERIAL_8N1, Pins::RX1_PIN, Pins::TX1_PIN); 
    Serial2.begin(HardwareConfig::SERIAL_BAUD, SERIAL_8N1, Pins::RX2_PIN, Pins::TX2_PIN);
    */

    Serial1.begin(HardwareConfig::SERIAL_BAUD_RATE, SERIAL_8N1, Pins::RX0_PIN, Pins::TX0_PIN); 
    Serial2.begin(HardwareConfig::SERIAL_BAUD_RATE, SERIAL_8N1, Pins::RX1_PIN, Pins::TX1_PIN);

    return;
}