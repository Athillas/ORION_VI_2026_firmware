// HardwareState.h
#ifndef HARDWARE_STATE_H
#define HARDWARE_STATE_H

// included from ../shared_libs/
// ---
#include <ACS712.h>
#include <Adafruit_NeoPixel.h>
// ---
#include <HX711.h>
#include <L298NX2.h>

#include "Configs/HardwareConfig.h"
#include "Pins.h"

struct HardwareState
{
    L298NX2 motorDrive;

    // ACS712_NNN ???
    ACS712 bldcMotorAmpM;
    ACS712 compressorAmpM;

    HX711 weightAmplifier;

    Adafruit_NeoPixel NeoPixelConnector;

    HardwareState() :
    motorDrive(
        Pins::INMOTOR1_PIN,
        Pins::INMOTOR2_PIN,
        Pins::INMOTOR3_PIN,
        Pins::INMOTOR4_PIN
    ),
    bldcMotorAmpM(
        Pins::VESP321_PIN, HardwareConfig::AMPM_VOLTS,
        HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE
    ),
    compressorAmpM(
        Pins::VESP322_PIN, HardwareConfig::AMPM_VOLTS,
        HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE
    )
    {
        weightAmplifier.begin(Pins::DT_PIN, Pins::SCK2_PIN);
        NeoPixelConnector.setPin(Pins::NEOPIX_SIGNAL_PIN);
    }
};

#endif