#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include "esp_wifi_types.h"

esp_err_t wifi_manager_init(void);
esp_err_t wifi_manager_scan(void);
esp_err_t wifi_manager_connect(const char *ssid, const char *password);
esp_err_t wifi_manager_deinit(void);

// New functions
uint16_t wifi_manager_get_ap_count(void);
wifi_ap_record_t* wifi_manager_get_ap_list(void);
const char* wifi_manager_get_auth_mode_str(wifi_auth_mode_t auth_mode);
void wifi_manager_print_scan_results(void);

// Add new declarations
void wifi_manager_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data);
bool wifi_manager_is_scan_complete(void);

#endif // WIFI_MANAGER_H
