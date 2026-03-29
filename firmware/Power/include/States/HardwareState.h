// HardwareState.h
#ifndef HARDWARE_STATE_H
#define HARDWARE_STATE_H

// included from ../shared_libs/
// ---
#include <ACS712XX.h>
#include <Adafruit_NeoPixel.h>
// ---

#include <DS18B20.h>

struct HardwareState
{
    // Amper-meters
    ACS712XX WL;
    ACS712XX WR;
    ACS712XX RAM;
    ACS712XX ELEK;
    ACS712XX SCI;
    ACS712XX INNE;

    // Thermometer
    DS18B20 T1;

    // NeoPixel
    Adafruit_NeoPixel NeoPixelConnector;

    HardwareState() : 
        WL(ACS712_30A, Pins::ADC_WL_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS),
        WR(ACS712_30A, Pins::ADC_WR_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS),
        RAM(ACS712_30A, Pins::ADC_RAM_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS),
        ELEK(ACS712_30A, Pins::ADC_ELEK_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS),
        SCI(ACS712_30A, Pins::ADC_SCI_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS),
        INNE(ACS712_30A, Pins::ADC_INNE_PIN, HardwareConfig::ADC_REF, HardwareConfig::ADC_FS),
        T1(Pins::TEMP_PIN)
    {
        NeoPixelConnector.setPin(Pins::NEOPIXEL_PIN);
    }
};

#endif