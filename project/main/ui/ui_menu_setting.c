#include "lvgl.h"
#include <stdio.h>
#include "esp_system.h"
#ifdef ESP_IDF_VERSION
#include "esp_log.h"
#endif
#include "app_data.h"

#include "lv_example_pub.h"
#include "lv_example_image.h"

static app_param_t param;

static bool setting_menu_layer_enter_cb(void *layer);
static bool setting_menu_layer_exit_cb(void *layer);
static void setting_menu_layer_timer_cb(lv_timer_t *tmr);

lv_layer_t setting_menu_Layer = {
    .lv_obj_name    = "setting_menu_Layer",
    .lv_obj_parent  = NULL,
    .lv_obj_layer   = NULL,
    .lv_show_layer  = NULL,
    .enter_cb       = setting_menu_layer_enter_cb,
    .exit_cb        = setting_menu_layer_exit_cb,
    .timer_cb       = setting_menu_layer_timer_cb,
};

static void * next_menu_Layer[] = {
    &main_Layer,
    &today_menu_Layer,
    &setting_main_Layer,
};

static void menu_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    } else if (LV_EVENT_KEY == code) {
        uint32_t key = lv_event_get_key(e);
        uint8_t idx = 0xFF;
        if (LV_KEY_RIGHT == key) {
            idx = 1;
        } else if (LV_KEY_LEFT == key) {
            idx = 0;
        }

        if (idx != 0xFF) {
            lv_group_set_editing(lv_group_get_default(), false);
            ui_remove_all_objs_from_encoder_group();
            lv_func_goto_layer(next_menu_Layer[idx]);
        }
        
        feed_clock_time();
    } else if (LV_EVENT_SHORT_CLICKED == code) {
        feed_clock_time();
        lv_group_set_editing(lv_group_get_default(), false);
        ui_remove_all_objs_from_encoder_group();
        lv_func_goto_layer(next_menu_Layer[2]);
    } else if (LV_EVENT_LONG_PRESSED == code) {
        feed_clock_time();
    }
}

void ui_setting_menu_init(lv_obj_t *parent)
{
    static lv_obj_t *icon = NULL;
    static lv_obj_t *label;
    lv_obj_t *page = parent;

    bool is_english = (LANGUAGE_EN == param.language);

    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);

    icon = lv_img_create(page);
    lv_img_set_src(icon, &setting_icon);
    lv_obj_center(icon);

    label = lv_label_create(page);
    lv_obj_set_style_text_font(label, &font_OPPOSans_24, 0);
    lv_label_set_recolor(label, true);
    lv_label_set_text(label, is_english?"#FFFFFF Setting#":"#FFFFFF 设置#");
    lv_obj_set_width(label, 150);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 25);

    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    ui_add_obj_to_encoder_group(page);
}

static bool setting_menu_layer_enter_cb(void *layer)
{
    bool ret = false;
    lv_layer_t *create_layer = layer;

    if (NULL == create_layer->lv_obj_layer) {
        ret = true;
        create_layer->lv_obj_layer = lv_obj_create(lv_scr_act());
        lv_obj_remove_style_all(create_layer->lv_obj_layer);
        lv_obj_set_size(create_layer->lv_obj_layer, LV_HOR_RES, LV_VER_RES);

        param = app_data_get_param(APP_PARAM_KEY);

        ui_setting_menu_init(create_layer->lv_obj_layer);
    }

    return ret;
}

static bool setting_menu_layer_exit_cb(void *layer)
{
    return true;
}

static void setting_menu_layer_timer_cb(lv_timer_t *tmr)
{
    return;
}
