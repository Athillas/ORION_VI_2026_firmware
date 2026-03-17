#include <Arduino.h>
#include <cstdint>

#pragma once

namespace Config
{
    constexpr uint8_t ADC_REF    = 1;
    constexpr uint16_t ADC_FS    = 4095;  
    constexpr uint32_t BAUD_RATE = 500E3; // 500kbps, change later 
    byte ETHERNET_MAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
}