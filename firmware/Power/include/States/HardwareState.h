// HardwareState.h
#ifndef HARDWARE_STATE_H
#define HARDWARE_STATE_H

#include <cstdint>

struct HardwareState
{
    float TEMP;
    uint16_t BAT_1_ADC;
    uint16_t BAT_2_ADC;
    uint16_t BAT_3_ADC;
    uint16_t BAT_4_ADC;
    uint16_t ADC_WL;
    uint16_t ADC_WR;
    uint16_t ADC_RAM;
    uint16_t ADC_ELEK;
    uint16_t ADC_SCI;
    uint16_t ADC_INNE;
};

#endif