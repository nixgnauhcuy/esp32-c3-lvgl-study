#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "app_data.h"

static const char *TAG = "app_data";

#define NAME_SPACE      "app_data"
#define MAGIC_HEAD      0xAA

static app_param_t g_app_param = {0};

static const app_param_t g_default_app_param = {
    .magic = MAGIC_HEAD,
    .language = LANGUAGE_CN,
    .brightness = 100,
};

static esp_err_t app_data_check(app_param_t *param)
{
    esp_err_t ret;
    ESP_GOTO_ON_FALSE(param->magic == MAGIC_HEAD, ESP_ERR_INVALID_ARG, reset, TAG, "magic incorrect");
    ESP_GOTO_ON_FALSE(param->language < LANGUAGE_MAX, ESP_ERR_INVALID_ARG, reset, TAG, "language incorrect");
    ESP_GOTO_ON_FALSE(param->brightness < 100, ESP_ERR_INVALID_ARG, reset, TAG, "brightness incorrect");
    return ret;
reset:
    ESP_LOGW(TAG, "Set to default");
    memcpy(&g_app_param, &g_default_app_param, sizeof(app_param_t));
    return ret;
}

esp_err_t app_data_read(const char *key)
{
    nvs_handle_t my_handle = 0;
    esp_err_t ret = nvs_open(NAME_SPACE, NVS_READONLY, &my_handle);
    if (ESP_ERR_NVS_NOT_FOUND == ret) {
        ESP_LOGW(TAG, "Not found, Set to default");
        memcpy(&g_app_param, &g_default_app_param, sizeof(app_param_t));
        app_data_write(key, g_app_param);
        return ESP_OK;
    }

    ESP_GOTO_ON_FALSE(ESP_OK == ret, ret, err, TAG, "nvs open failed (0x%x)", ret);

    size_t len = sizeof(app_param_t);
    ret = nvs_get_blob(my_handle, key, &g_app_param, &len);
    ESP_GOTO_ON_FALSE(ESP_OK == ret, ret, err, TAG, "can't read param");
    nvs_close(my_handle);

    app_data_check(&g_app_param);
    return ret;
err:
    if (my_handle) {
        nvs_close(my_handle);
    }
    return ret;
}

esp_err_t app_data_write(const char *key, app_param_t param)
{
    app_data_check(&param);
    g_app_param = param;
    nvs_handle_t my_handle = {0};
    esp_err_t err = nvs_open(NAME_SPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        err = nvs_set_blob(my_handle, key, &g_app_param, sizeof(app_param_t));
        err |= nvs_commit(my_handle);
        nvs_close(my_handle);
    }
    return ESP_OK == err ? ESP_OK : ESP_FAIL;
}

app_param_t app_data_get_param(const char *key)
{
    return g_app_param;
}

esp_err_t app_data_init(void)
{
    /* Initialize NVS. */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(app_data_read(APP_PARAM_KEY));

    return err;
}
