#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <esp_log.h>

#include "include/webserver.h"
#include "include/gpio_controller.h"
#include <main.h>


void init(void) {
    gpio_mutex = xSemaphoreCreateMutex();
}

// Main application
void app_main(void) {
    init();
    nvs_flash_init();
    xTaskCreate((TaskFunction_t) webserver_main, "webserver", 20480, NULL, 1, NULL);
    xTaskCreate((TaskFunction_t) gpio_main, "pc_power", 20480, NULL, 1, NULL);
}
