#include "settings_tab.h"
#include <string.h>
#include "esp_log.h"
#include "wifi_settings.h"

static const char *TAG = "SettingsTab";

static lv_obj_t *settings_list = NULL;
static lv_obj_t *current_view = NULL;

/* Forward declarations */
static void settings_event_handler(lv_event_t *e);
static void create_view(const char *title);
static void back_event_handler(lv_event_t *e);

void settings_tab_init(lv_obj_t *tab)
{
    settings_list = lv_list_create(tab);
    lv_obj_set_size(settings_list, LV_HOR_RES, 400);
    lv_obj_center(settings_list);

    lv_obj_t *btn;

    lv_list_add_text(settings_list, "Connectivity");
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_WIFI, "Wi-Fi");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_BLUETOOTH, "Bluetooth");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_USB, "USB");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_GPS, "AP Mode");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);

    lv_list_add_text(settings_list, "Power");
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_BATTERY_FULL, "Battery");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);

    lv_list_add_text(settings_list, "SD Card");
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_FILE, "New");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_DIRECTORY, "Open");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_SAVE, "Save");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_CLOSE, "Delete");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(settings_list, LV_SYMBOL_EDIT, "Export");
    lv_obj_add_event_cb(btn, settings_event_handler, LV_EVENT_CLICKED, NULL);
}

static void settings_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        const char *btn_text = lv_list_get_btn_text(settings_list, obj);
        ESP_LOGI(TAG, "Clicked: %s", btn_text);

        if (strcmp(btn_text, "Wi-Fi") == 0)
        {
            lv_obj_add_flag(settings_list, LV_OBJ_FLAG_HIDDEN);
            wifi_settings_create(lv_scr_act()); // Call Wi-Fi settings view
        }
        else if (strcmp(btn_text, "Bluetooth") == 0)
        {
            lv_obj_add_flag(settings_list, LV_OBJ_FLAG_HIDDEN);
            // Call Bluetooth settings creation function here
            ESP_LOGI(TAG, "Bluetooth settings selected");
            create_view(btn_text);
        }
        else if (strcmp(btn_text, "USB") == 0)
        {
            ESP_LOGI(TAG, "USB settings selected");
            create_view(btn_text);
        }
        else if (strcmp(btn_text, "Battery") == 0)
        {
            ESP_LOGI(TAG, "Battery settings selected");
            create_view(btn_text);
        }
        else
        {
            create_view(btn_text);
        }
    }
}

static void create_view(const char *title)
{
    if (current_view)
    {
        lv_obj_del(current_view);
    }

    current_view = lv_obj_create(lv_scr_act());
    lv_obj_set_size(current_view, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(current_view, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(current_view);
    lv_label_set_text(label, title);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *back_btn = lv_btn_create(current_view);
    lv_obj_set_size(back_btn, 20, 20);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "<");
    lv_obj_center(back_label);

    lv_obj_add_event_cb(back_btn, back_event_handler, LV_EVENT_CLICKED, NULL);
}

static void back_event_handler(lv_event_t *e)
{
    if (current_view)
    {
        lv_obj_del(current_view);
        current_view = NULL;
    }
    lv_obj_clear_flag(settings_list, LV_OBJ_FLAG_HIDDEN);
}

lv_obj_t *settings_tab_get_list(void)
{
    return settings_list; // Return the settings list object
}
