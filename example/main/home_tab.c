#include "lvgl.h"

void home_tab_init(lv_obj_t *tab)
{
    lv_obj_t *label = lv_label_create(tab);
    lv_label_set_text(label, "Home");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    // Create a loader arc
    lv_obj_t *arc = lv_arc_create(tab);
    lv_obj_set_size(arc, 100, 100);
    lv_obj_align(arc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_rotation(arc, 270); // Start from the top

    // Create an animation to rotate the arc
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_arc_set_value);
    lv_anim_set_time(&a, 30000); // 30 seconds
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_start(&a);
}
