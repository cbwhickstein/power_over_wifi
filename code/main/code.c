#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <esp_log.h>

#include "include/webserver.h"

// Main application
void app_main(void) {
    nvs_flash_init();
    wifi_init();
    

}
