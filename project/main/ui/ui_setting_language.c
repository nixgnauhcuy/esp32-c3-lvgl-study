#include "lvgl.h"
#include <stdio.h>
#include "esp_system.h"
#ifdef ESP_IDF_VERSION
#include "esp_log.h"
#endif
#include "bsp/esp-bsp.h"
#include "app_data.h"

#include "lv_example_pub.h"
#include "lv_example_image.h"

#define MAX_VALUE 100
#define MIN_VALUE 10

static uint8_t language;
static bool is_english;
lv_obj_t * btn1, * btn2;
static time_out_count time_100ms;
static app_param_t param;

static bool setting_language_layer_enter_cb(void *layer);
static bool setting_language_layer_exit_cb(void *layer);
static void setting_language_layer_timer_cb(lv_timer_t *tmr);

lv_layer_t setting_language_Layer = {
    .lv_obj_name    = "setting_language_Layer",
    .lv_obj_parent  = NULL,
    .lv_obj_layer   = NULL,
    .lv_show_layer  = NULL,
    .enter_cb       = setting_language_layer_enter_cb,
    .exit_cb        = setting_language_layer_exit_cb,
    .timer_cb       = setting_language_layer_timer_cb,
};

static void menu_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    } else if (LV_EVENT_KEY == code) {
        if (is_time_out(&time_100ms)) {
            if (lv_obj_has_state(btn1, LV_STATE_CHECKED) == false) {
                lv_obj_add_state(btn1, LV_STATE_CHECKED);
                lv_obj_clear_state(btn2, LV_STATE_CHECKED);
                language = LANGUAGE_EN;
            } else {
                lv_obj_add_state(btn2, LV_STATE_CHECKED);
                lv_obj_clear_state(btn1, LV_STATE_CHECKED);
                language = LANGUAGE_CN;
            }
        }
    } else if (LV_EVENT_LONG_PRESSED == code) {
        lv_indev_wait_release(lv_indev_get_next(NULL));
        ui_remove_all_objs_from_encoder_group();
        lv_func_goto_layer(&setting_main_Layer);
    }
}

void ui_setting_language_init(lv_obj_t *parent)
{
    lv_obj_t *page = parent;

    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title_label = lv_label_create(page);
    lv_obj_set_style_text_font(title_label, &font_OPPOSans_24, 0);
    lv_label_set_text(title_label, is_english?"Language":"语言");
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 25);

    btn1 = lv_btn_create(page);
    lv_obj_set_size(btn1, 60, 40);
    lv_obj_align(btn1, LV_ALIGN_CENTER, -40, 0);

    lv_obj_t * btn1_label = lv_label_create(btn1);
    lv_obj_set_style_text_font(btn1_label, &font_OPPOSans_20, 0);
    lv_label_set_text(btn1_label, "En");
    lv_obj_center(btn1_label);

    btn2 = lv_btn_create(page);
    lv_obj_set_size(btn2, 60, 40);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 40, 0);
    
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    lv_obj_t * btn2_label = lv_label_create(btn2);
    lv_obj_set_style_text_font(btn2_label, &font_OPPOSans_20, 0);
    lv_label_set_text(btn2_label, "中文");
    lv_obj_center(btn2_label);

    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    if (is_english)
    {
        lv_obj_add_state(btn1, LV_STATE_CHECKED);
        lv_obj_clear_state(btn2, LV_STATE_CHECKED);
    }
    else
    {
        lv_obj_add_state(btn2, LV_STATE_CHECKED);
        lv_obj_clear_state(btn1, LV_STATE_CHECKED);
    }

    ui_remove_all_objs_from_encoder_group();//roll will add event default.
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    ui_add_obj_to_encoder_group(page);
}

static bool setting_language_layer_enter_cb(void *layer)
{
    bool ret = false;
    lv_layer_t *create_layer = layer;

    if (NULL == create_layer->lv_obj_layer) {
        ret = true;
        create_layer->lv_obj_layer = lv_obj_create(lv_scr_act());
        lv_obj_remove_style_all(create_layer->lv_obj_layer);
        lv_obj_set_size(create_layer->lv_obj_layer, LV_HOR_RES, LV_VER_RES);

        param = app_data_get_param(APP_PARAM_KEY);
        language = param.language;
        is_english = (LANGUAGE_EN == language);
        ui_setting_language_init(create_layer->lv_obj_layer);
        set_time_out(&time_100ms, 100);
    }

    return ret;
}

static bool setting_language_layer_exit_cb(void *layer)
{
    param.language = language;
    app_data_write(APP_PARAM_KEY, param);
    return true;
}

static void setting_language_layer_timer_cb(lv_timer_t *tmr)
{
    return;
}
