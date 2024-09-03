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

static uint16_t step = 0;
static uint8_t heart = 0;
static uint16_t kcal = 0;
static lv_obj_t* step_label, *heart_label, *kcal_label;
static lv_obj_t * step_arc, * heart_arc, * kcal_arc;
static time_out_count time_100ms;

static bool today_menu_layer_enter_cb(void *layer);
static bool today_menu_layer_exit_cb(void *layer);
static void today_menu_layer_timer_cb(lv_timer_t *tmr);

lv_layer_t today_menu_Layer = {
    .lv_obj_name    = "today_menu_Layer",
    .lv_obj_parent  = NULL,
    .lv_obj_layer   = NULL,
    .lv_show_layer  = NULL,
    .enter_cb       = today_menu_layer_enter_cb,
    .exit_cb        = today_menu_layer_exit_cb,
    .timer_cb       = today_menu_layer_timer_cb,
};

static void * next_menu_Layer[] = {
    &setting_main_Layer,
    &main_Layer,
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
    } else if (LV_EVENT_LONG_PRESSED == code) {
        feed_clock_time();
    }
}

void ui_today_menu_init(lv_obj_t *parent)
{
    lv_obj_t *page = parent;

    /* Step */
    step_arc = lv_arc_create(page);
    lv_arc_set_angles(step_arc, 60, 300);
    lv_arc_set_bg_angles(step_arc, 60, 300);
    lv_arc_set_range(step_arc, 0, 10000);
    lv_obj_set_style_arc_color(step_arc, lv_color_hex(0x0098ff), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(step_arc, lv_color_hex(0xc8eeff), LV_PART_MAIN);
    lv_obj_remove_style(step_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(step_arc, LV_OBJ_FLAG_CLICKABLE); 
    lv_obj_align(step_arc, LV_ALIGN_LEFT_MID, 20, 0);
    lv_obj_set_size(step_arc, 210, 210);

    step_label = lv_label_create(page);
    lv_obj_align(step_label, LV_ALIGN_CENTER, 30, -25);
    lv_obj_set_style_text_font(step_label, &font_OPPOSans_20, 0);
    lv_label_set_recolor(step_label, true);

    /* Heart */
    heart_arc = lv_arc_create(page);
    lv_arc_set_angles(heart_arc, 60, 300);
    lv_arc_set_bg_angles(heart_arc, 60, 300);
    lv_arc_set_range(heart_arc, 0, 220);
    lv_obj_set_style_arc_color(heart_arc, lv_color_hex(0xff8028), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(heart_arc, lv_color_hex(0xffeed7), LV_PART_MAIN);
    lv_obj_remove_style(heart_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(heart_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(heart_arc, LV_ALIGN_LEFT_MID, 40, 0);
    lv_obj_set_size(heart_arc, 170, 170);

    heart_label = lv_label_create(page);
    lv_obj_align(heart_label, LV_ALIGN_CENTER, 30, 0);
    lv_obj_set_style_text_font(heart_label, &font_OPPOSans_20, 0);
    lv_label_set_recolor(heart_label, true);

    /* Kcal */
    kcal_arc = lv_arc_create(page);
    lv_arc_set_angles(kcal_arc, 60, 300);
    lv_arc_set_bg_angles(kcal_arc, 60, 300);
    lv_arc_set_range(kcal_arc, 0, 10000);
    lv_obj_set_style_arc_color(kcal_arc, lv_color_hex(0xff3c3c), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(kcal_arc, lv_color_hex(0xffcccc), LV_PART_MAIN);
    lv_obj_remove_style(kcal_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(kcal_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(kcal_arc, LV_ALIGN_LEFT_MID, 60, 0);
    lv_obj_set_size(kcal_arc, 130, 130);

    kcal_label = lv_label_create(page);
    lv_obj_align(kcal_label, LV_ALIGN_CENTER, 30, 25);
    lv_obj_set_style_text_font(kcal_label, &font_OPPOSans_20, 0);
    lv_label_set_recolor(kcal_label, true);

    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    ui_add_obj_to_encoder_group(page);
}

static bool today_menu_layer_enter_cb(void *layer)
{
    bool ret = false;
    lv_layer_t *create_layer = layer;

    if (NULL == create_layer->lv_obj_layer) {
        ret = true;
        create_layer->lv_obj_layer = lv_obj_create(lv_scr_act());
        lv_obj_remove_style_all(create_layer->lv_obj_layer);
        lv_obj_set_size(create_layer->lv_obj_layer, LV_HOR_RES, LV_VER_RES);

        param = app_data_get_param(APP_PARAM_KEY);

        ui_today_menu_init(create_layer->lv_obj_layer);
    }
    set_time_out(&time_100ms, 100);
    feed_clock_time();

    return ret;
}

static bool today_menu_layer_exit_cb(void *layer)
{
    return true;
}

static void today_menu_layer_timer_cb(lv_timer_t *tmr)
{
    if (is_time_out(&time_100ms))
    {
        kcal += 5;
        step += 10;
        if (++heart >= 220)
            heart = 0;
        if (kcal >= 10000)
            kcal = 0;
        if (step >= 10000)
            step = 0;

        lv_arc_set_value(step_arc, step);
        lv_arc_set_value(heart_arc, heart);
        lv_arc_set_value(kcal_arc, kcal);

        lv_label_set_text_fmt(step_label, "#0098ff Step:%d#", step);
        lv_label_set_text_fmt(heart_label, "#ff8028 Heart:%d#", heart);
        lv_label_set_text_fmt(kcal_label, "#ff3c3c Kcal:%d#", kcal);
        feed_clock_time();
    }
    return;
}
