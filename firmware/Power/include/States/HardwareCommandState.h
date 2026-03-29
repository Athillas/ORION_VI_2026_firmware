// HardwareCommandState.h
#ifndef HARDWARE_COMMAND_STATE_H
#define HARDWARE_COMMAND_STATE_H

struct HardwareCommandState
{
    bool S_W_L;
    bool S_W_R;
    bool S_INNE;
    bool S_SCI;
    bool S_ELEK;
    bool S_RAM;
    bool FAN1_ON;
    bool NEOPIXEL;
};

#endif