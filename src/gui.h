#pragma once

#include "lvgl.h"
#include "ui_menu.h"

void gui_init(void);
void gui_task(void);
void gui_do_card_switch(void);

#define UI_GOTO_SCREEN(scr) do { \
    lv_scr_load(scr); \
    lv_group_focus_obj(scr); \
} while (0);

static inline lv_obj_t* ui_scr_create(void) {
    lv_obj_t * obj = lv_obj_create(NULL);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_group_add_obj(lv_group_get_default(), obj);
    return obj;
}

/* create a navigatable UI menu container, so that the item (label) inside can be selected and clicked */
static inline lv_obj_t* ui_menu_cont_create_nav(lv_obj_t *parent) {
    lv_obj_t* cont = ui_menu_cont_create(parent);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_group_add_obj(lv_group_get_default(), cont);
    return cont;
}
