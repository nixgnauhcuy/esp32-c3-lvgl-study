#pragma once

#include "esp_err.h"

#define APP_PARAM_KEY "app_param"

typedef enum {
    LANGUAGE_EN = 0,
    LANGUAGE_CN,
    LANGUAGE_MAX,
} APP_PARAM_LANGUAGE;

typedef struct {
    uint8_t magic;
    uint8_t language;
    uint8_t brightness;
    uint8_t reserve[1021];
} app_param_t;

esp_err_t app_data_read(const char *key);
esp_err_t app_data_write(const char *key, app_param_t param);
app_param_t app_data_get_param(const char *key);
esp_err_t app_data_init(void);