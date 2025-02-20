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
    webserver_main();
    gpio_main();
}
