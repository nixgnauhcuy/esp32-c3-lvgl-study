#include "lvgl.h"
#include <stdio.h>
#include "esp_system.h"
#ifdef ESP_IDF_VERSION
#include "esp_log.h"
#endif
#include "app_data.h"

#include "lv_example_pub.h"
#include "lv_example_image.h"

#define SETTING_NUM 3

static app_param_t param;
static unsigned char cur_btn_idx = 0;
static lv_obj_t *cont;
static time_out_count time_100ms;

static bool setting_main_layer_enter_cb(void *layer);
static bool setting_main_layer_exit_cb(void *layer);
static void setting_main_layer_timer_cb(lv_timer_t *tmr);

lv_layer_t setting_main_Layer = {
    .lv_obj_name    = "setting_main_Layer",
    .lv_obj_parent  = NULL,
    .lv_obj_layer   = NULL,
    .lv_show_layer  = NULL,
    .enter_cb       = setting_main_layer_enter_cb,
    .exit_cb        = setting_main_layer_exit_cb,
    .timer_cb       = setting_main_layer_timer_cb,
};

static void * next_menu_Layer[SETTING_NUM] = {
    &setting_light_Layer,
    &setting_language_Layer,
    &setting_about_Layer,
};

static const char *labels[SETTING_NUM][2] = {
    {"#FFFFFF Brightness#", "#FFFFFF 调整亮度#"},
    {"#FFFFFF Language#", "#FFFFFF 语言#"},
    {"#FFFFFF About#", "#FFFFFF 关于#"},
};

static const void *icons[SETTING_NUM] = {
    &light_icon,
    &language_icon,
    &about_icon,
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
                cur_btn_idx++;
                if (cur_btn_idx >= lv_obj_get_child_cnt(cont)) {
                    cur_btn_idx = 0;
                }
            } else {
                if (cur_btn_idx > 0) {
                    cur_btn_idx--;
                }
            }
            lv_obj_scroll_to_view(lv_obj_get_child(cont, cur_btn_idx), LV_ANIM_ON);
        }

    } else if (LV_EVENT_SHORT_CLICKED == code) {
        lv_indev_wait_release(lv_indev_get_next(NULL));
        ui_remove_all_objs_from_encoder_group();
        lv_func_goto_layer(next_menu_Layer[cur_btn_idx]);
    } else if (LV_EVENT_LONG_PRESSED == code) {
        lv_indev_wait_release(lv_indev_get_next(NULL));
        ui_remove_all_objs_from_encoder_group();
        lv_func_goto_layer(&main_Layer);
    }
}

static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    int32_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        int32_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        int32_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        }
        else {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);

        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

void ui_setting_main_init(lv_obj_t *parent)
{
    lv_obj_t *page = parent;

    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);

    cont = lv_obj_create(page);
    lv_obj_set_size(cont, 240, 240);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(cont, true, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    for(uint8_t i = 0; i < SETTING_NUM; i++) {
        lv_obj_t *row_cont = lv_obj_create(cont);
        lv_obj_set_size(row_cont, LV_PCT(120), 50);
        lv_obj_set_flex_flow(row_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_border_width(row_cont, 0, 0);

        lv_obj_t *icon = lv_img_create(row_cont);
        lv_img_set_src(icon, icons[i]);

        lv_obj_t *label = lv_label_create(row_cont);
        lv_obj_set_style_text_font(label, &font_OPPOSans_24, 0);
        lv_label_set_recolor(label, true);
        lv_label_set_text(label, labels[i][param.language]);
    }

    /*Update the buttons position manually for first*/
    lv_event_send(cont, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    lv_obj_scroll_to_view(lv_obj_get_child(cont, cur_btn_idx), LV_ANIM_ON);

    ui_remove_all_objs_from_encoder_group();//roll will add event default.
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    ui_add_obj_to_encoder_group(page);
}

static bool setting_main_layer_enter_cb(void *layer)
{
    bool ret = false;
    lv_layer_t *create_layer = layer;

    if (NULL == create_layer->lv_obj_layer) {
        ret = true;
        create_layer->lv_obj_layer = lv_obj_create(lv_scr_act());
        lv_obj_remove_style_all(create_layer->lv_obj_layer);
        lv_obj_set_size(create_layer->lv_obj_layer, LV_HOR_RES, LV_VER_RES);

        param = app_data_get_param(APP_PARAM_KEY);

        ui_setting_main_init(create_layer->lv_obj_layer);
        set_time_out(&time_100ms, 100);
    }

    return ret;
}

static bool setting_main_layer_exit_cb(void *layer)
{
    return true;
}

static void setting_main_layer_timer_cb(lv_timer_t *tmr)
{
    feed_clock_time();
}
