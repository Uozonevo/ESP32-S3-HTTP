#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <sdkconfig.h>
#include <esp_wifi.h>
#include <esp_http_server.h> // added for HTTP web server
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
            .authmode = WIFI_AUTH_WPA2_PSK,
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

// Configure HTTP server 
httpd_handle_t start_webserver(){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // After calling http_start(), server is initialized
    if (httpd_start(&server, &config) == ESP_OK){
        ESP_LOGI(TAG, "Server started successfully, registering URI handlers...");
        return server;
    }
    
    ESP_LOGI(TAG, "Failed to start server");
    return NULL;
}

// Request handler function
esp_err_t my_uri_handler(httpd_req_t* req){
    const char *resp_str = "<h1>Hello World</h1>";
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Configure URI structure defineing the properties being registered
static const httpd_uri_t http_uri = {
    .uri = "/",             // Address where source can be found
    .method = HTTP_GET,     // HTTP method (HTTP_GET, HTTP_POST, ...)
    .handler = my_uri_handler,        // Function to process request
    .user_ctx = NULL,       // Additional user data for context
};

void app_main(void){
    wifi_init_softAP();
    // Start server calling
    httpd_handle_t server = start_webserver();
    // Configure URI management
    httpd_register_uri_handler(server, &http_uri);

}