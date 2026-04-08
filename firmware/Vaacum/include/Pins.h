#include <Arduino.h>
#include <stdint.h>

#pragma once

namespace Pins
{
    // BLDC engine pins
    constexpr uint8_t TEMP_BLDC_PIN      = GPIO_NUM_36; // +
    constexpr uint8_t DIR_PIN            = GPIO_NUM_32; // +
    constexpr uint8_t BRAKE_PIN          = GPIO_NUM_33; // +
    constexpr uint8_t PWM_BLDC_PIN       = GPIO_NUM_35; // +- awaiting scheme...
    constexpr uint8_t PWM_SERVO_PIN      = GPIO_NUM_2;  // +- awaiting scheme...

    // L298N DC Motor Driver Module
    constexpr uint8_t INMOTOR1_PIN       = GPIO_NUM_27; // +
    constexpr uint8_t INMOTOR2_PIN       = GPIO_NUM_14; // +
    constexpr uint8_t INMOTOR3_PIN       = GPIO_NUM_12; // +
    constexpr uint8_t INMOTOR4_PIN       = GPIO_NUM_13; // +

    // Kompresor ARK-2 5mm / Elektro zawur ARK 2,5mm
    constexpr uint8_t GPIO_ON_OFF1_PIN   = GPIO_NUM_25;
    constexpr uint8_t GPIO_ON_OFF2_PIN   = GPIO_NUM_26;

    // Linear current sensors (ACS712)
    constexpr uint8_t VESP321_PIN        = GPIO_NUM_39; // +
    constexpr uint8_t VESP322_PIN        = GPIO_NUM_34; // +

    // Keyboard
    constexpr uint8_t KLAW_SIGNAL_PIN    = GPIO_NUM_4; // +

    // HX711_REDBOARD_2
    constexpr uint8_t DT_PIN             = GPIO_NUM_16; // +
    constexpr uint8_t SCK2_PIN           = GPIO_NUM_17; // +

    // NeoPixel
    constexpr uint8_t NEOPIX_SIGNAL_PIN  = GPIO_NUM_15; // +-, number of pixels unknown!!!
    
    // WIZnet W5500 !!!MORE INFO NEEDED!!!
    constexpr uint8_t MISO_PIN           = GPIO_NUM_19; // -
    constexpr uint8_t CS_PIN             = GPIO_NUM_5;  // -
    constexpr uint8_t SCK_PIN            = GPIO_NUM_18; // -
    constexpr uint8_t MOSI_PIN           = GPIO_NUM_23; // -

    // 7x1 GPIO
    constexpr uint8_t NET_PORT1_PIN      = GPIO_NUM_0;  // -
    constexpr uint8_t NET_PORT2_PIN      = GPIO_NUM_21; // -
    constexpr uint8_t NET_PORT3_PIN      = GPIO_NUM_3;  // -
    constexpr uint8_t NET_PORT4_PIN      = GPIO_NUM_1;  // -
    constexpr uint8_t NET_PORT5_PIN      = GPIO_NUM_22; // -

    inline void init_pins()
    {
        pinMode(TEMP_BLDC_PIN, INPUT);
        pinMode(DIR_PIN, OUTPUT);
        pinMode(BRAKE_PIN, OUTPUT);
        pinMode(PWM_BLDC_PIN, INPUT);
        pinMode(PWM_SERVO_PIN, INPUT);

        pinMode(GPIO_ON_OFF1_PIN, OUTPUT);
        pinMode(GPIO_ON_OFF2_PIN, OUTPUT);

        pinMode(KLAW_SIGNAL_PIN, INPUT_PULLUP);

        // UNSURE!!!!
        pinMode(NET_PORT1_PIN, INPUT);
        pinMode(NET_PORT2_PIN, INPUT_PULLUP);
        pinMode(NET_PORT3_PIN, INPUT);
        pinMode(NET_PORT4_PIN, OUTPUT);
        pinMode(NET_PORT5_PIN, INPUT_PULLUP);
    }
}