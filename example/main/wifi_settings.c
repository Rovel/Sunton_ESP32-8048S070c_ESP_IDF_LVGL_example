#include "wifi_settings.h"
#include "wifi_manager.h"
#include "esp_log.h"
#include "settings_tab.h"
#include "esp_wifi.h" // Include for wifi_ap_record_t and Wi-Fi functions

// Declare the missing functions if they are not part of the included headers
// extern uint16_t wifi_manager_get_ap_count();
// extern wifi_ap_record_t *wifi_manager_get_ap_list();

static const char *TAG = "WiFiSettings";

static lv_obj_t *current_view = NULL;
static lv_obj_t *wifi_list = NULL;
static lv_obj_t *wifi_switch = NULL;
static lv_obj_t *loader_label = NULL;

static void wifi_scan_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        const char *ssid = lv_list_get_btn_text(wifi_list, obj);
        ESP_LOGI(TAG, "Selected SSID: %s", ssid);

        // Connect to the selected Wi-Fi network (use a placeholder password for now)
        wifi_manager_connect(ssid, "your_password");
    }
}

static void wifi_switch_event_handler(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);
    if (lv_obj_has_state(sw, LV_STATE_CHECKED)) {
        // Show loader
        loader_label = lv_label_create(current_view);
        lv_label_set_text(loader_label, "Enabling WiFi and scanning...");
        lv_obj_align(loader_label, LV_ALIGN_CENTER, 0, 0);

        // Init and scan
        if (wifi_manager_init() == ESP_OK) {
            esp_err_t ret = wifi_manager_scan();
            if (ret == ESP_OK) {
                lv_label_set_text(loader_label, "Scan done.");
            } else {
                lv_label_set_text_fmt(loader_label, "Scan failed: %d", ret);
            }
        } else {
            lv_label_set_text(loader_label, "WiFi init failed.");
        }
        // Populate list after scanning
        uint16_t ap_count = wifi_manager_get_ap_count();
        wifi_ap_record_t *ap_list = wifi_manager_get_ap_list();
        for (int i = 0; i < ap_count; i++) {
            lv_obj_t *btn = lv_list_add_btn(wifi_list,
                                            LV_SYMBOL_WIFI,
                                            (const char *)ap_list[i].ssid);
            lv_obj_add_event_cb(btn, wifi_scan_event_handler, LV_EVENT_CLICKED, NULL);
        }
        // Remove loader
        lv_obj_del(loader_label);
    } else {
        // Turn Wi-Fi off
        wifi_manager_deinit();
        // Clear AP list
        lv_obj_clean(wifi_list);
    }
}

void wifi_settings_create(lv_obj_t *parent)
{
    ESP_LOGI(TAG, "Creating Wi-Fi settings view...");

    current_view = lv_obj_create(lv_scr_act());
    lv_obj_set_size(current_view, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(current_view, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(current_view);
    lv_label_set_text(label, "Wi-Fi Settings");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *back_btn = lv_btn_create(current_view);
    lv_obj_set_size(back_btn, 20, 20);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "<");
    lv_obj_center(back_label);

    // Create Wi-Fi switch
    wifi_switch = lv_switch_create(current_view);
    lv_obj_align_to(wifi_switch, current_view, LV_ALIGN_OUT_TOP_RIGHT, -30, 50);
    lv_obj_add_event_cb(wifi_switch, wifi_switch_event_handler, LV_EVENT_VALUE_CHANGED, NULL);


    lv_obj_add_event_cb(back_btn, wifi_settings_back_event_handler, LV_EVENT_CLICKED, NULL);


    wifi_list = lv_list_create(current_view);
    lv_obj_set_size(wifi_list, 750, 400);
    lv_obj_align(wifi_list, LV_ALIGN_BOTTOM_MID, 0, 0);


    
}

void wifi_settings_back_event_handler(lv_event_t *e)
{
    if (current_view != NULL) {
        lv_obj_del(current_view); // Clear the Wi-Fi settings view
        // lv_obj_del(back_btn); // Clear the back button}
    }
    lv_obj_clear_flag(settings_tab_get_list(), LV_OBJ_FLAG_HIDDEN); // Show the settings list
}
