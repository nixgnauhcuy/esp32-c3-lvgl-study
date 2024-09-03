/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "app_data.h"
#include "lv_example_pub.h"
#include "bsp/esp-bsp.h"
#include "lv_example_pub.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Compile time: %s %s", __DATE__, __TIME__);

    app_data_init();
    app_param_t param = app_data_get_param(APP_PARAM_KEY);

    bsp_display_start();

    ui_obj_to_encoder_init();
    lv_create_home(&boot_Layer);
    lv_create_clock(&clock_screen_Layer, TIME_ENTER_CLOCK_1MIN);
    bsp_display_unlock();

    vTaskDelay(pdMS_TO_TICKS(500));

    bsp_display_brightness_set(param.brightness);
}
