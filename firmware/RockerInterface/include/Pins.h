// Pins.h
#ifndef PINS_H
#define PINS_H

#include <Arduino.h>
#include <cstdint>

namespace Pins
{
    // Temperature pins
    constexpr uint8_t TMP_A_PIN    = GPIO_NUM_35;
    constexpr uint8_t TMP_B_PIN    = GPIO_NUM_34;

    // Ethernet pins
    constexpr uint8_t WIZ_INT_PIN  = GPIO_NUM_39;
    constexpr uint8_t WIZ_RST_PIN  = GPIO_NUM_27;
    constexpr uint8_t CS_PIN       = GPIO_NUM_5;
    constexpr uint8_t SCK_PIN      = GPIO_NUM_18;
    constexpr uint8_t MISO_PIN     = GPIO_NUM_19;
    constexpr uint8_t MOSI_PIN     = GPIO_NUM_23;

    // Servo feedback
    constexpr uint8_t SV_CFB_B     = GPIO_NUM_15;
    constexpr uint8_t SV_CFB_A     = GPIO_NUM_2;
    constexpr uint8_t SV_VFB_A     = GPIO_NUM_0;
    constexpr uint8_t SV_VFB_B     = GPIO_NUM_4;

    // Servo control
    constexpr uint8_t SV_A_PIN     = GPIO_NUM_33;
    constexpr uint8_t SV_B_PIN     = GPIO_NUM_32;

    // Fans
    constexpr uint8_t FAN_A_PIN    = GPIO_NUM_14;
    constexpr uint8_t FAN_B_PIN    = GPIO_NUM_12;
    
    // Neopixel
    constexpr uint8_t NE_OUT_PIN   = GPIO_NUM_7;

    // Gyroscope
    constexpr uint8_t GYRO_INT_PIN = GPIO_NUM_36;
    
    // Gyroscope & I2C
    constexpr uint8_t SDA_PIN      = GPIO_NUM_21;
    constexpr uint8_t SCL_PIN      = GPIO_NUM_22;
    
    // CAN (handled by CAN module)
    constexpr uint8_t CAN_RX_PIN   = GPIO_NUM_25;
    constexpr uint8_t CAN_TX_PIN   = GPIO_NUM_26;

    // UART_1 Handled in Hardware::initHardware()
    constexpr uint8_t RX1_PIN      = GPIO_NUM_9;
    constexpr uint8_t TX1_PIN      = GPIO_NUM_10;
    
    // UART_0 Handled in Hardware::initHardware()
    constexpr uint8_t TX0_PIN      = GPIO_NUM_1;
    constexpr uint8_t RX0_PIN      = GPIO_NUM_3;
    
    // VSENSE
    constexpr uint8_t VSENSE_PIN   = GPIO_NUM_13;
    
    // Unconnected pins 
    constexpr uint8_t RX2_PIN      = GPIO_NUM_16;
    constexpr uint8_t TX2_PIN      = GPIO_NUM_17;
    constexpr uint8_t NE_IN_PIN    = GPIO_NUM_8;
    constexpr uint8_t CLK_PIN      = GPIO_NUM_6;
    
    inline void init_pins()
    {
        pinMode(TMP_A_PIN, INPUT);
        pinMode(TMP_B_PIN, INPUT);

        pinMode(FAN_A_PIN, OUTPUT);
        pinMode(FAN_B_PIN, OUTPUT);

        pinMode(VSENSE_PIN, INPUT); // ?
    }
}

#endif