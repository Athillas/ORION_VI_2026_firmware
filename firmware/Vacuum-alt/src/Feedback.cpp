// Feedback.cpp

#include <ACS712.h>
#include <HX711.h>

#include "Feedback.h"
#include "Pins.h"

namespace
{
    struct TaskParams
    {
        char *feedback_message;
        AppState *app_state;
        SemaphoreHandle_t *serial_mutex;
        SemaphoreHandle_t *feedback_message_mutex;
        SemaphoreHandle_t *app_state_mutex;
    };

    ACS712 bldc_ammeter(Pins::VESP321_PIN, Feedback::AMMETER_VOLTS, Feedback::AMMETER_MAX_ADC, Feedback::AMMETER_MV_PER_AMPERE);
    ACS712 compressor_valve_ammeter(Pins::VESP322_PIN, Feedback::AMMETER_VOLTS, Feedback::AMMETER_MAX_ADC, Feedback::AMMETER_MV_PER_AMPERE);
    HX711 load_cell;
    
    struct TaskParams ftp{nullptr, nullptr, nullptr, nullptr, nullptr};
}

void feedback_task(void *parameters);

void Feedback::init(char (&feedback_message)[NetworkConfig::MAX_JSON_PAYLOAD], AppState &as, SemaphoreHandle_t &serial_mutex, SemaphoreHandle_t &feedback_message_mutex, SemaphoreHandle_t &app_state_mutex)
{
    if(xSemaphoreTake(serial_mutex, portMAX_DELAY))
    {
        Serial.println("[FEEDBACK] Initializing...");
        ftp.serial_mutex = &serial_mutex;
        xSemaphoreGive(serial_mutex);
    }

    ftp.feedback_message_mutex = &feedback_message_mutex;
    ftp.app_state_mutex = &app_state_mutex;
    
    if(xSemaphoreTake(feedback_message_mutex, portMAX_DELAY))
    {
        snprintf(ftp.feedback_message, NetworkConfig::MAX_JSON_PAYLOAD, feedback_message);
        xSemaphoreGive(feedback_message_mutex);
    }
    
    if(xSemaphoreTake(app_state_mutex, portMAX_DELAY))
    {
        ftp.app_state = &as;
        xSemaphoreGive(app_state_mutex);
    }

    load_cell.begin(Pins::DT_PIN, Pins::SCK2_PIN);
    // load_cell.set_scale();
    // load_cell.tare();

    if(xSemaphoreTake(serial_mutex, portMAX_DELAY))
    {
        if(xTaskCreate(
            feedback_task,
            "FEEDBACK",
            1024,
            NULL,
            1,
            NULL
        ) != pdTRUE)
        {
            
            Serial.println("[FEEDBACK] Failed to create the feedback task!");
        }
        else
        {
            Serial.println("[FEEDBACK] Feedback task created successfully. Initialization complete.");
        }
        
        xSemaphoreGive(serial_mutex);
    }
}

void feedback_task(void *parameters)
{
    TickType_t last_wake_time = xTaskGetTickCount();
    AppState app_state_;

    float bldc_ammeter_value_ = 0.0f, compressor_valve_ammeter_value_ = 0.0f, load_cell_value_ = 0.0f;
    uint32_t bldc_temperature_value_ = 0;
    char feedback_message_[NetworkConfig::MAX_JSON_PAYLOAD]{0};

    for(;;)
    {
        bldc_ammeter_value_ = bldc_ammeter.mA_DC();
        compressor_valve_ammeter_value_ = compressor_valve_ammeter.mA_DC();

        if (load_cell.wait_ready_timeout(1000))
        {
            load_cell_value_ = load_cell.get_units(10);
        }

        bldc_temperature_value_ = analogRead(Pins::TEMP_BLDC_PIN);

        if(xSemaphoreTake(*ftp.app_state_mutex, portMAX_DELAY))
        {
            app_state_ = *ftp.app_state;
            xSemaphoreGive(*ftp.app_state_mutex);
        }

        /*
            [
                0: bldc_ammeter_value_              (float)
                1: compressor_valve_ammeter_value_  (float)
                2: load_cell_value_                 (float)
                3: bldc_temperature_value_          (uint16_t)
                4: VacuumCommand                    (uint8_t)
                5: VacuumState                      (uint8_t)
                6: AppErrorState                    (uint8_t)
                7: isReady                          (uint8_t)
            ]
        */
        uint16_t len = snprintf(
            feedback_message_,
            sizeof(feedback_message_),
            "[%.2f,%.2f,%.2f,%u,%u,%u,%u,%u]",
            bldc_ammeter_value_,
            compressor_valve_ammeter_value_,
            load_cell_value_,
            bldc_temperature_value_,
            app_state_.vacuum_command,
            app_state_.vacuum_state,
            app_state_.error_state,
            app_state_.isReady
        );

        if(len < 0)
        {
            if(xSemaphoreTake(*ftp.serial_mutex, portMAX_DELAY))
            {
                Serial.println("[FEEDBACK] Failed to serialize the feedback_message_ buffer!");
                xSemaphoreGive(*ftp.serial_mutex);
            }
        }
        else
        {
            if(xSemaphoreTake(*ftp.feedback_message_mutex, portMAX_DELAY))
            {
                memcpy(ftp.feedback_message, feedback_message_, len + 1);
                xSemaphoreGive(*ftp.feedback_message_mutex);
            }
        }

        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(Feedback::FEEDBACK_TASK_TIMEOUT));
    }
}