#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <esp_log.h>

#include "include/webserver.h"
#include "include/gpio_controller.h"
#include <main.h>

#include <driver/ledc.h>


#define SERVO_GPIO 27
#define LEDC_TIMER  LEDC_TIMER_0
#define LEDC_MODE   LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_DUTY_RES   13
#define LEDC_FREQUENCY  50

void init(void)
{
    gpio_mutex = xSemaphoreCreateMutex();

    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t ch_conf = {
        .gpio_num = SERVO_GPIO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0
    };
    ledc_channel_config(&ch_conf);
}

static uint32_t pulse_us_to_duty(uint32_t pulse_us) {
    const uint32_t period_us = 1000000 / LEDC_FREQUENCY;
    const uint32_t max_duty = (1 << LEDC_DUTY_RES) - 1;
    return (uint32_t) (((uint64_t)pulse_us * max_duty) / period_us);
}

void one_swing(void) {
    for (int us = 500; us <= 2000; us += 50) {
            uint32_t duty = pulse_us_to_duty(us);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(20)); // allow servo to move
        }        
    vTaskDelay(pdMS_TO_TICKS(100));
    for (int us = 2000; us >= 500; us -= 50) {
        uint32_t duty = pulse_us_to_duty(us);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);            
        vTaskDelay(pdMS_TO_TICKS(20));        
    }        
}

void monitor_task(void *arg)
{
    while(1) {
        if (xSemaphoreTake(gpio_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (pow_should_trigger == 1) {
                one_swing();
                pow_should_trigger = 0;
            } 
            xSemaphoreGive(gpio_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

// Main application
void app_main(void)
{
    init();

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    wifi_init();
    xTaskCreate((TaskFunction_t)monitor_task, "monitor", 2048, NULL, 5, NULL);
    //xTaskCreate((TaskFunction_t)gpio_main, "pc_power", 3072, NULL, 5, NULL);
}
