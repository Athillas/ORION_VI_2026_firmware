#include <Arduino.h>
#include <cstdint>

#pragma once

namespace Pins
{
    // Temperature pins
    const uint8_t TMP_A_PIN    = GPIO_NUM_35;
    const uint8_t TMP_B_PIN    = GPIO_NUM_34;

    // Ethernet pins
    const uint8_t WIZ_INT_PIN  = GPIO_NUM_39;
    const uint8_t WIZ_RST_PIN  = GPIO_NUM_27;
    const uint8_t CS_PIN       = GPIO_NUM_5;
    const uint8_t SCK_PIN      = GPIO_NUM_18;
    const uint8_t MISO_PIN     = GPIO_NUM_19;

    // Servo feedback
    const uint8_t SV_CFB_B     = GPIO_NUM_15;
    const uint8_t SV_CFB_A     = GPIO_NUM_2;
    const uint8_t SV_VFB_A     = GPIO_NUM_0;
    const uint8_t SV_VFB_B     = GPIO_NUM_4;

    // Servo control
    const uint8_t SV_A_PIN     = GPIO_NUM_33;
    const uint8_t SV_B_PIN     = GPIO_NUM_32;

    // Fans
    const uint8_t FAN_A_PIN    = GPIO_NUM_14;
    const uint8_t FAN_B_PIN    = GPIO_NUM_12;
    
    // Neopixel
    const uint8_t NE_OUT_PIN   = GPIO_NUM_7;

    // Gyroscope
    const uint8_t GYRO_INT_PIN = GPIO_NUM_36;
    
    // Gyroscope & I2C
    const uint8_t SDA_PIN      = GPIO_NUM_21;
    const uint8_t SCL_PIN      = GPIO_NUM_22;
    
    // CAN
    const uint8_t CAN_RX_PIN   = GPIO_NUM_25;
    const uint8_t CAN_TX_PIN   = GPIO_NUM_26;

    
    // UART_1
    const uint8_t RX1_PIN      = GPIO_NUM_9;
    const uint8_t TX1_PIN      = GPIO_NUM_10;
    
    // UART_0
    const uint8_t TX0_PIN      = GPIO_NUM_1;
    const uint8_t RX0_PIN      = GPIO_NUM_3;
    
    const uint8_t MOSI_PIN     = GPIO_NUM_23;
    
    // VSENSE
    const uint8_t VSENSE_PIN   = GPIO_NUM_13;
    
    // Unconnected pins 
    const uint8_t RX2_PIN      = GPIO_NUM_16;
    const uint8_t TX2_PIN      = GPIO_NUM_17;
    const uint8_t NE_IN_PIN    = GPIO_NUM_8;
    const uint8_t CLK_PIN      = GPIO_NUM_6;
    
    inline void init_pins()
    {
        pinMode(TMP_A_PIN, INPUT);
        pinMode(TMP_B_PIN, INPUT);

        pinMode(FAN_A_PIN, OUTPUT);
        pinMode(FAN_B_PIN, OUTPUT);
    }
}