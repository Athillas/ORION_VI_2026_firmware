// HardwareState.h
#ifndef HARDWARE_STATE_H
#define HARDWARE_STATE_H


#include <ACS712.h>
#include <Adafruit_NeoPixel.h>
#include <DS18B20.h>

#include "Pins.h"
#include "Configs/HardwareConfig.h"

struct HardwareState
{
    // Amper-meters
    ACS712 WL;
    ACS712 WR;
    ACS712 RAM;
    ACS712 ELEK;
    ACS712 SCI;
    ACS712 INNE;

    // Thermometer
    DS18B20 T1;

    // NeoPixel
    Adafruit_NeoPixel NeoPixelConnector;

    HardwareState() : 
        WL(Pins::ADC_WL_PIN, HardwareConfig::AMPM_VOLTS, HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE),
        WR(Pins::ADC_WR_PIN, HardwareConfig::AMPM_VOLTS, HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE),
        RAM(Pins::ADC_RAM_PIN, HardwareConfig::AMPM_VOLTS, HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE),
        ELEK(Pins::ADC_ELEK_PIN, HardwareConfig::AMPM_VOLTS, HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE),
        SCI(Pins::ADC_SCI_PIN, HardwareConfig::AMPM_VOLTS, HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE),
        INNE(Pins::ADC_INNE_PIN, HardwareConfig::AMPM_VOLTS, HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE),
        T1(Pins::TEMP_PIN)
    {
        NeoPixelConnector.setPin(Pins::NEOPIXEL_PIN);
    }
};

#endif