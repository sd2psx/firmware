#pragma once

#include "lvgl.h"
#include "ui_menu.h"

void gui_init(void);
void gui_task(void);
void gui_do_card_switch(void);

void evt_menu_page(lv_event_t *event);

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

static inline lv_obj_t* ui_menu_subpage_create(lv_obj_t *menu, const char* title) {
    lv_obj_t *page = ui_menu_page_create(menu, title);
    lv_obj_add_flag(page, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_group_add_obj(lv_group_get_default(), page);
    lv_obj_add_event_cb(page, evt_menu_page, LV_EVENT_ALL, page);
    return page;
}
