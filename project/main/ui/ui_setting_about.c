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

static bool setting_about_layer_enter_cb(void *layer);
static bool setting_about_layer_exit_cb(void *layer);
static void setting_about_layer_timer_cb(lv_timer_t *tmr);

lv_layer_t setting_about_Layer = {
    .lv_obj_name    = "setting_about_Layer",
    .lv_obj_parent  = NULL,
    .lv_obj_layer   = NULL,
    .lv_show_layer  = NULL,
    .enter_cb       = setting_about_layer_enter_cb,
    .exit_cb        = setting_about_layer_exit_cb,
    .timer_cb       = setting_about_layer_timer_cb,
};

static void menu_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    } else if (LV_EVENT_LONG_PRESSED == code) {
        lv_indev_wait_release(lv_indev_get_next(NULL));
        ui_remove_all_objs_from_encoder_group();
        lv_func_goto_layer(&setting_main_Layer);
    }
}

void ui_setting_about_init(lv_obj_t *parent)
{
    lv_obj_t *page = parent;
    bool is_english = (LANGUAGE_EN == param.language);
    char *title, *device_name, *version;

    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);

    if (is_english)
    {
        title = "about";
        device_name = "Device Name";
        version = "Version";
    }
    else
    {
        title = "关于";
        device_name = "设备名称";
        version = "版本号";
    }
    
    lv_obj_t *title_label = lv_label_create(page);
    lv_obj_set_style_text_font(title_label, &font_OPPOSans_24, 0);
    lv_label_set_text(title_label, title);
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 25);

    lv_obj_t *label = lv_label_create(page);
    lv_obj_set_style_text_font(label, &font_OPPOSans_20, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x0000FF), 0);
    lv_label_set_text(label, device_name);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 25, 80);

    lv_obj_t *value = lv_label_create(page);
    lv_obj_set_style_text_font(value, &font_OPPOSans_20, 0);
    lv_label_set_text(value, "ESP-C3-WATCH");
    lv_obj_align(value, LV_ALIGN_TOP_LEFT, 25, 110);

    label = lv_label_create(page);
    lv_obj_set_style_text_font(label, &font_OPPOSans_20, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x0000FF), 0);
    lv_label_set_text(label, version);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 25, 150);

    value = lv_label_create(page);
    lv_obj_set_style_text_font(value, &font_OPPOSans_20, 0);
    lv_label_set_text(value, "0.0.0.1.001");
    lv_obj_align(value, LV_ALIGN_TOP_LEFT, 25, 180);

    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    ui_add_obj_to_encoder_group(page);
}

static bool setting_about_layer_enter_cb(void *layer)
{
    bool ret = false;
    lv_layer_t *create_layer = layer;

    if (NULL == create_layer->lv_obj_layer) {
        ret = true;
        create_layer->lv_obj_layer = lv_obj_create(lv_scr_act());
        lv_obj_remove_style_all(create_layer->lv_obj_layer);
        lv_obj_set_size(create_layer->lv_obj_layer, LV_HOR_RES, LV_VER_RES);

        param = app_data_get_param(APP_PARAM_KEY);

        ui_setting_about_init(create_layer->lv_obj_layer);
    }

    return ret;
}

static bool setting_about_layer_exit_cb(void *layer)
{
    return true;
}

static void setting_about_layer_timer_cb(lv_timer_t *tmr)
{
    return;
}
