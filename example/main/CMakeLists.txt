idf_component_register(SRCS main.c home_tab.c info_tab.c settings_tab.c wifi_settings.c wifi_manager.c
    INCLUDE_DIRS .
    REQUIRES esp_lcd driver esp_wifi nvs_flash)
