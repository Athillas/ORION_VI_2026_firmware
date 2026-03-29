// HardwareConfig.h
#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <cstdint>

namespace HardwareConfig
{
    constexpr uint8_t ADC_REF       = 1;
    constexpr uint16_t ADC_FS       = 4095;
    constexpr uint32_t BAUD_RATE    = 500E3; // 500kbps, change later 
}

#endif