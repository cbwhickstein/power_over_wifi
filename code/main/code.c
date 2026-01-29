#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <esp_log.h>

#include "include/webserver.h"
#include "include/gpio_controller.h"
#include <main.h>

void init(void)
{
    gpio_mutex = xSemaphoreCreateMutex();
}

void monitor_task(void *arg)
{
    while (1) {
        size_t heap = esp_get_free_heap_size();

        if (heap < 20000) {
            ESP_LOGE("MON", "Low heap, restarting");
            esp_restart();
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
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
    xTaskCreate((TaskFunction_t)gpio_main, "pc_power", 3072, NULL, 5, NULL);
}
