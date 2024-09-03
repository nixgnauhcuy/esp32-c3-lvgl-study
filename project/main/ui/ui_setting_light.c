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

static uint8_t brightness = 100;
static time_out_count time_100ms;
static lv_obj_t * bar;
static app_param_t param;

static void set_value(void * bar, int32_t v);
static bool setting_light_layer_enter_cb(void *layer);
static bool setting_light_layer_exit_cb(void *layer);
static void setting_light_layer_timer_cb(lv_timer_t *tmr);

lv_layer_t setting_light_Layer = {
    .lv_obj_name    = "setting_light_Layer",
    .lv_obj_parent  = NULL,
    .lv_obj_layer   = NULL,
    .lv_show_layer  = NULL,
    .enter_cb       = setting_light_layer_enter_cb,
    .exit_cb        = setting_light_layer_exit_cb,
    .timer_cb       = setting_light_layer_timer_cb,
};

static void menu_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    } else if (LV_EVENT_KEY == code) {
        if (is_time_out(&time_100ms)) {
            uint32_t key = lv_event_get_key(e);
            if (LV_KEY_RIGHT == key) {
                brightness += 10;
                if (brightness > MAX_VALUE) {
                    brightness = MAX_VALUE;
                }
            } else {
                brightness -= 10;
                if (brightness < MIN_VALUE) {
                    brightness = MIN_VALUE;
                }
            }
            set_value(bar, brightness);
            bsp_display_brightness_set(brightness);
        }
    } else if (LV_EVENT_LONG_PRESSED == code) {
        lv_indev_wait_release(lv_indev_get_next(NULL));
        ui_remove_all_objs_from_encoder_group();
        lv_func_goto_layer(&setting_main_Layer);
    }
}

static void set_value(void * bar, int32_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_OFF);
}

static void ui_setting_light_bar_event_cb(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part != LV_PART_INDICATOR) return;

    lv_obj_t * obj = lv_event_get_target(e);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = LV_FONT_DEFAULT;

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", (int)lv_bar_get_value(obj));

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
                    label_dsc.flag);

    lv_area_t txt_area;
    /*If the indicator is long enough put the text inside on the right*/
    if(lv_area_get_width(dsc->draw_area) > txt_size.x + 20) {
        txt_area.x2 = dsc->draw_area->x2 - 5;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
    /*If the indicator is still short put the text out of it on the right*/
    else {
        txt_area.x1 = dsc->draw_area->x2 + 5;
        txt_area.x2 = txt_area.x1 + txt_size.x - 1;
        label_dsc.color = lv_color_black();
    }

    txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;

    lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
}

void ui_setting_light_init(lv_obj_t *parent)
{
    lv_obj_t *page = parent;
    bool is_english = (LANGUAGE_EN == param.language);

    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title_label = lv_label_create(page);
    lv_obj_set_style_text_font(title_label, &font_OPPOSans_24, 0);
    lv_label_set_text(title_label, is_english?"Brightness":"亮度调整");
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 25);
    
    bar = lv_bar_create(page);
    lv_obj_add_event_cb(bar, ui_setting_light_bar_event_cb, LV_EVENT_DRAW_PART_END, NULL);
    lv_obj_set_size(bar, 200, 30);
    lv_obj_center(bar);

    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    ui_add_obj_to_encoder_group(page);
}

static bool setting_light_layer_enter_cb(void *layer)
{
    bool ret = false;
    lv_layer_t *create_layer = layer;

    if (NULL == create_layer->lv_obj_layer) {
        ret = true;
        create_layer->lv_obj_layer = lv_obj_create(lv_scr_act());
        lv_obj_remove_style_all(create_layer->lv_obj_layer);
        lv_obj_set_size(create_layer->lv_obj_layer, LV_HOR_RES, LV_VER_RES);

        param = app_data_get_param(APP_PARAM_KEY);
        brightness = param.brightness;
        ui_setting_light_init(create_layer->lv_obj_layer);
        set_value(bar, brightness);
        set_time_out(&time_100ms, 100);
    }

    return ret;
}

static bool setting_light_layer_exit_cb(void *layer)
{
    param.brightness = brightness;
    app_data_write(APP_PARAM_KEY, param);
    return true;
}

static void setting_light_layer_timer_cb(lv_timer_t *tmr)
{
    return;
}
