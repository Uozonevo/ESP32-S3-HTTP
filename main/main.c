#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <sdkconfig.h>
#include <esp_wifi.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#define ESP_WIFI_SSID       "ESP_tut"
#define ESP_WIFI_PASS       "hello world"
#define ESP_WIFI_CHANNEL    1
#define MAX_STA_CONN        2

/**
 * Initializing IP stack and Event Loop
 * To start the soft-AP
 * 1. Include esp_wifi.h and string.h
 * 2. Initialize the IP stack (via esp_netif_init and 
 *      esp_netif_create_default_wifi_ap)
 * 3. Start the default event loop
 * 4. Create and register an event handler function to process
 *      Wi-Fi events
 */

static const char *TAG = "example";

// Register event handlers for soft AP
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data){
    printf("Event nr: %ld!\n", event_id);
}

void wifi_init_softAP(){
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    // Always start with this
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&cfg);
    
    esp_event_handler_instance_register(WIFI_EVENT, 
        ESP_EVENT_ANY_ID, 
        &wifi_event_handler, 
        NULL, 
        NULL
    );

    wifi_config_t config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .password = ESP_WIFI_PASS,
            .channel = ESP_WIFI_CHANNEL,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WAPI_PSK,
            .pmf_cfg = {
                .required = true,
            },

        },
    };

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &config);
    esp_wifi_start();

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ESP_WIFI_SSID, ESP_WIFI_PASS, ESP_WIFI_CHANNEL);
}

void app_main(void){
    wifi_init_softAP();
}