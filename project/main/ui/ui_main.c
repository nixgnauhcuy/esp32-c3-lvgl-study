#include "lvgl.h"
#include <stdio.h>
#include "esp_system.h"
#ifdef ESP_IDF_VERSION
#include "esp_log.h"
#endif

#include "lv_example_pub.h"
#include "lv_example_image.h"

static bool main_layer_enter_cb(void *layer);
static bool main_layer_exit_cb(void *layer);
static void main_layer_timer_cb(lv_timer_t *tmr);

lv_layer_t main_Layer = {
    .lv_obj_name    = "main_Layer",
    .lv_obj_parent  = NULL,
    .lv_obj_layer   = NULL,
    .lv_show_layer  = NULL,
    .enter_cb       = main_layer_enter_cb,
    .exit_cb        = main_layer_exit_cb,
    .timer_cb       = main_layer_timer_cb,
};

static void * next_menu_Layer[] = {
    &today_menu_Layer,
    &setting_menu_Layer,
};

static time_out_count time_100ms;

static void main_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    } else if (LV_EVENT_KEY == code) {
        uint32_t key = lv_event_get_key(e);
        if (is_time_out(&time_100ms)) {
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
        }
        feed_clock_time();

    } else if (LV_EVENT_CLICKED == code) {
        feed_clock_time();
    } else if (LV_EVENT_LONG_PRESSED == code) {
    }
}

void ui_main_init(lv_obj_t *parent)
{
    static lv_obj_t *img_background = NULL;
    lv_obj_t *page = parent;

    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);

    img_background = lv_img_create(page);
    lv_img_set_src(img_background, &dial_0);

    lv_obj_add_event_cb(page, main_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, main_event_cb, LV_EVENT_KEY, NULL);
    ui_add_obj_to_encoder_group(page);
}

static bool main_layer_enter_cb(void *layer)
{
    bool ret = false;
    lv_layer_t *create_layer = layer;

    if (NULL == create_layer->lv_obj_layer) {
        ret = true;
        create_layer->lv_obj_layer = lv_obj_create(lv_scr_act());
        lv_obj_remove_style_all(create_layer->lv_obj_layer);
        lv_obj_set_size(create_layer->lv_obj_layer, LV_HOR_RES, LV_VER_RES);

        ui_main_init(create_layer->lv_obj_layer);
    }
    set_time_out(&time_100ms, 100);
    feed_clock_time();

    return ret;
}

static bool main_layer_exit_cb(void *layer)
{
    return true;
}

static void main_layer_timer_cb(lv_timer_t *tmr)
{
    return;
}
