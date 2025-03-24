#include "lvgl.h"

void info_tab_init(lv_obj_t *tab)
{
    lv_obj_t *label = lv_label_create(tab);
    lv_label_set_text(label, "Info!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}