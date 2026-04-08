// HardwareState.h
#ifndef HARDWARE_STATE_H
#define HARDWARE_STATE_H

// included from ../shared_libs/
// ---
#include <ACS712.h>
#include <Adafruit_NeoPixel.h>
// ---

#include <MPU6050.h>

#include "Pins.h"
#include "Configs/HardwareConfig.h"

struct HardwareState
{
    // Servo amper-meters
    ACS712 SERVO_AMPMETER_A;
    ACS712 SERVO_AMPMETER_B;

    // NeoPixel
    Adafruit_NeoPixel NeoPixelConnector;

    // Gyroscope
    MPU6050 GYRO;

    // Thermometer feedback (!!!requires clarification!!! float or int?)
    float TEMP_A;
    float TEMP_B;

    // Servo feedback (!!!requires clarification!!! float or int?)
    float SERVO_VFB_A;
    float SERVO_VFB_B;

    // Servo control (!!!requires clarification!!! how is servo controlled?)
    float SERVO_A_CTRL;
    float SERVO_B_CTRL;

    // !!!requires clarification!!!
    float VSENSE;

    // Fan control
    uint8_t FAN_A_STATE : 1;
    uint8_t FAN_B_STATE : 1;

    // Gyroscope interrupt
    uint8_t GYRO_INT_STATE : 1;

    HardwareState() : 
        SERVO_AMPMETER_A(Pins::SV_CFB_A, HardwareConfig::AMPM_VOLTS, HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE),
        SERVO_AMPMETER_B(Pins::SV_CFB_B, HardwareConfig::AMPM_VOLTS, HardwareConfig::AMPM_MAX_ADC, HardwareConfig::AMPM_MV_PER_AMPERE),
        GYRO(),
        SERVO_VFB_A(0.0f), SERVO_VFB_B(0.0f), TEMP_A(0.0f), TEMP_B(0.0f), FAN_A_STATE(0), FAN_B_STATE(0),
        GYRO_INT_STATE(0)
    {
        Wire.begin(Pins::SDA_PIN, Pins::SCL_PIN);
        NeoPixelConnector.setPin(Pins::NE_OUT_PIN);
    }
};

#endif