#ifndef SETTINGS_TAB_H
#define SETTINGS_TAB_H

#include "lvgl.h"

void settings_tab_init(lv_obj_t *tab); // Make this function accessible globally
lv_obj_t *settings_tab_get_list(void); // Expose the function to retrieve the settings list

#endif // SETTINGS_TAB_H