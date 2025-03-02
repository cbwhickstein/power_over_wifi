#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdio.h>
#include <main.h>

#define POW_GPIO 13
#define BUTTON_GPIO 14

#define POW_ON 0
#define POW_OFF 1

SemaphoreHandle_t gpio_mutex;
uint8_t pow_should_trigger = 0;

void init_gpio(void) {
    gpio_reset_pin(POW_GPIO);
    gpio_reset_pin(BUTTON_GPIO);
    
    gpio_set_direction(POW_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);

    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    gpio_set_level(POW_GPIO, POW_OFF);
}

void loop_gpio(void) {
    while(1) {
        // Trigger the Power of the mainboard
        xSemaphoreTake(gpio_mutex, pdMS_TO_TICKS(100));
        if (pow_should_trigger) {
            pow_should_trigger = 0;

            xSemaphoreGive(gpio_mutex);
            gpio_set_level(POW_GPIO, POW_ON);
            vTaskDelay(pdMS_TO_TICKS(1000));
            gpio_set_level(POW_GPIO, POW_OFF);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        else {
            xSemaphoreGive(gpio_mutex);
        }

        // Get the input of the PC Button
        if (gpio_get_level(BUTTON_GPIO) == 0) {
            xSemaphoreTake(gpio_mutex, pdMS_TO_TICKS(100));
            pow_should_trigger = 1;
            xSemaphoreGive(gpio_mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void gpio_main(void) {
    printf("Started PC Power Thread\n");
    init_gpio();
    loop_gpio();
    vTaskDelete(NULL);
}