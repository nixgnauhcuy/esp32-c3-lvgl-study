/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#ifndef LV_EXAMPLE_PUB_H
#define LV_EXAMPLE_PUB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "esp_err.h"
#include "esp_log.h"

#include "lv_schedule_basic.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

/** Major version number (X.x.x) */
#define DEMO_VERSION_MAJOR 0
/** Minor version number (x.X.x) */
#define DEMO_VERSION_MINOR 1
/** Patch version number (x.x.X) */
#define DEMO_VERSION_PATCH 2

#define TIME_ENTER_CLOCK_1MIN    (60*1000)

#define COLOUR_BLACK            0x000000
#define COLOUR_WHITE            0xFFFFFF
#define COLOUR_YELLOW           0xE9BD85
#define COLOUR_GREY_1F          0x1F1F1F
#define COLOUR_GREY_2F          0x2F2F2F

#define COLOUR_GREY_BF          0xBFBFBF
#define COLOUR_GREY_8F          0x8F8F8F
#define COLOUR_GREY_4F          0x4F4F4F

extern lv_layer_t boot_Layer;
extern lv_layer_t main_Layer;
extern lv_layer_t today_menu_Layer;
extern lv_layer_t setting_menu_Layer;
extern lv_layer_t setting_main_Layer;
extern lv_layer_t setting_light_Layer;
extern lv_layer_t setting_language_Layer;
extern lv_layer_t setting_about_Layer;
extern lv_layer_t clock_screen_Layer;

extern void ui_obj_to_encoder_init(void);

extern void ui_add_obj_to_encoder_group(lv_obj_t *obj);

extern void ui_remove_all_objs_from_encoder_group(void);

#endif /*LV_EXAMPLE_PUB_H*/
