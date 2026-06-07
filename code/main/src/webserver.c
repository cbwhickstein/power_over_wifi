#include <webserver.h>

#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <stdatomic.h>

#include <wifi_config.h>
#include <pages.h>
#include <main.h>

static const char *TAG = "HTTP_SERVER";
static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;
int disconnect_counter = 0;

// HTTP index request handler
esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_send(req, index_page_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// HTTP gpio request handler
esp_err_t gpio_handler(httpd_req_t *req)
{
    httpd_resp_send(req, gpio_page_html, HTTPD_RESP_USE_STRLEN);

    if (xSemaphoreTake(gpio_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        pow_should_trigger = 1;
        xSemaphoreGive(gpio_mutex);
    }

    return ESP_OK;
}

// HTTP disconnect Counter
esp_err_t disconnect_handler(httpd_req_t *req) 
{
    char buf[32];    
    int val = atomic_load(&disconnect_counter);    
    int len = snprintf(buf, sizeof(buf), "%d", val);

    // set content type (text/plain or application/json)    
    httpd_resp_set_type(req, "text/plain");    
    httpd_resp_set_status(req, "200 OK");    
    httpd_resp_send(req, buf, len);    
    return ESP_OK;
}

// Start HTTP server
httpd_handle_t start_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t index_page = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = index_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &index_page);

        httpd_uri_t pow_page = {
            .uri = "/pow_on",
            .method = HTTP_GET,
            .handler = gpio_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &pow_page);

        httpd_uri_t disconnected_page = {
            .uri = "/disconnected",
            .method = HTTP_GET,
            .handler = disconnect_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &disconnected_page);
    }
    else {
        ESP_LOGE(TAG, "HTTP server failed, RESTARTING");
        esp_restart();
    }
    return server;
}

// Wi-Fi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {        
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);        
        ESP_LOGI(TAG, "Disconnected, reconnecting...");      
        disconnect_counter++;  
        esp_wifi_connect(); // auto-reconnect    
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        start_server();
        ESP_LOGI(TAG, "Server started at http://<ESP32_IP>");
    }
}

// Initialize Wi-Fi
void wifi_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

void webserver_main(void)
{
    ESP_LOGI(TAG, "Webserver Thread Started!");
    wifi_init();
    vTaskDelete(NULL);
}