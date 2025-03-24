#include "wifi_manager.h"
#include "esp_log.h"
#include <string.h>
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "freertos/event_groups.h"

#define CHANNEL_LIST_SIZE 12
// static uint8_t channel_list[CHANNEL_LIST_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

static const char *TAG = "wifi_manager";
static EventGroupHandle_t wifi_event_group;
static const int SCAN_DONE_BIT = BIT0;

static wifi_ap_record_t ap_list[50]; // Adjust size as needed
static uint16_t ap_count = 0;
static bool s_wifi_inited = false;
static bool s_event_loop_created = false;
static esp_netif_t *s_netif = NULL;

void wifi_manager_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_SCAN_DONE:
                ESP_LOGI(TAG, "Scan completed");
                xEventGroupSetBits(wifi_event_group, SCAN_DONE_BIT);
                break;
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WiFi station started");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "Disconnected from WiFi");
                break;
        }
    }
}

esp_err_t wifi_manager_init(void)
{
    if (s_wifi_inited) {
        ESP_LOGI(TAG, "Wi-Fi already initialized");
        return ESP_OK;
    }
    s_wifi_inited = true;
    ESP_LOGI(TAG, "Initializing Wi-Fi...");

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Create event group
    wifi_event_group = xEventGroupCreate();

    // Initialize TCP/IP adapter
    ESP_ERROR_CHECK(esp_netif_init());

    // Only create the default event loop if we haven't already
    if (!s_event_loop_created) {
        esp_err_t err = esp_event_loop_create_default();
        if (err == ESP_OK) {
            s_event_loop_created = true;
        } else if (err == ESP_ERR_INVALID_STATE) {
            ESP_LOGW(TAG, "Default event loop already created elsewhere");
            s_event_loop_created = false;  // Another part of app created it
        } else {
            return err;
        }
    }

    if (!s_netif) {
        s_netif = esp_netif_create_default_wifi_sta();
    }

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_wifi_set_ps(WIFI_PS_NONE);

    // Register event handler
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &wifi_manager_event_handler,
                                                      NULL,
                                                      NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Setting Wi-Fi country to Brazil");
    wifi_country_t country_conf = {
        .cc = "BR",
        .schan = 1,
        .nchan = 13,
        .max_tx_power = 20, // 20 dBm
        .policy = WIFI_COUNTRY_POLICY_MANUAL
    };
    ESP_ERROR_CHECK(esp_wifi_set_country(&country_conf));
    
    return ESP_OK;
}

esp_err_t wifi_manager_deinit(void)
{
    if (!s_wifi_inited) {
        ESP_LOGI(TAG, "Wi-Fi is not initialized");
        return ESP_OK;
    }
    ESP_LOGI(TAG, "Deinitializing Wi-Fi...");

    // Stop and deinit Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
    s_wifi_inited = false;

    // Destroy netif if we created it
    if (s_netif) {
        esp_netif_destroy(s_netif);
        s_netif = NULL;
    }

    // Delete the default event loop only if we created it
    if (s_event_loop_created) {
        ESP_ERROR_CHECK(esp_event_loop_delete_default());
        s_event_loop_created = false;
    }

    return ESP_OK;
}

const char* wifi_manager_get_auth_mode_str(wifi_auth_mode_t auth_mode)
{
    switch (auth_mode) {
    case WIFI_AUTH_OPEN:
        return "OPEN";
    case WIFI_AUTH_WEP:
        return "WEP";
    case WIFI_AUTH_WPA_PSK:
        return "WPA_PSK";
    case WIFI_AUTH_WPA2_PSK:
        return "WPA2_PSK";
    case WIFI_AUTH_WPA_WPA2_PSK:
        return "WPA_WPA2_PSK";
    case WIFI_AUTH_WPA3_PSK:
        return "WPA3_PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK:
        return "WPA2_WPA3_PSK";
    default:
        return "UNKNOWN";
    }
}

// static void array_2_channel_bitmap(const uint8_t channel_list[], const uint8_t channel_list_size, wifi_scan_config_t *scan_config) {

//     for(uint8_t i = 0; i < channel_list_size; i++) {
//         uint8_t channel = channel_list[i];
//         scan_config->channel_bitmap.ghz_2_channels |= (1 << channel);
//     }
// }

esp_err_t wifi_manager_scan(void)
{
    ESP_LOGI(TAG, "Performing a simple, blocking Wi-Fi scan...");

    // Comment out existing event bits usage:
    // xEventGroupClearBits(wifi_event_group, SCAN_DONE_BIT);

    // Use the most basic scan config
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true,       // Optionally scan hidden networks
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active.min = 100,
        .scan_time.active.max = 200, // Increase scan time
        // Leave other fields as default
    };

    // ESP_LOGI(TAG, "Scan config: active scan %d - %d ms", scan_config.scan_time.active.min, scan_config.scan_time.active.max);

    // Block until scan is finished by passing `true`
    esp_err_t err = esp_wifi_scan_start(&scan_config, true);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start scan: %s", esp_err_to_name(err));
        return err;
    }

    // Now that the scan is complete, retrieve records
    ap_count = 0;
    err = esp_wifi_scan_get_ap_records(&ap_count, ap_list);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get scan results: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Found %d access points:", ap_count);
    wifi_manager_print_scan_results();

    return ESP_OK;
}

void wifi_manager_print_scan_results(void)
{
    for (int i = 0; i < ap_count; i++) {
        ESP_LOGI(TAG, "SSID \t\t%s", (char *)ap_list[i].ssid);
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_list[i].rssi);
        ESP_LOGI(TAG, "Channel \t\t%d", ap_list[i].primary);
        ESP_LOGI(TAG, "Auth Mode \t%s", wifi_manager_get_auth_mode_str(ap_list[i].authmode));
        if (ap_list[i].authmode != WIFI_AUTH_OPEN) {
            ESP_LOGI(TAG, "Pairwise Cipher \t%d", ap_list[i].pairwise_cipher);
            ESP_LOGI(TAG, "Group Cipher \t%d", ap_list[i].group_cipher);
        }
        ESP_LOGI(TAG, "--------------------");
    }
}

uint16_t wifi_manager_get_ap_count()
{
    return ap_count;
}

wifi_ap_record_t *wifi_manager_get_ap_list()
{
    return ap_list;
}

esp_err_t wifi_manager_connect(const char *ssid, const char *password)
{
    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid);

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "Connection initiated to SSID: %s", ssid);
    return ESP_OK;
}
