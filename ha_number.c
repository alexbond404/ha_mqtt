#include "ha_number.h"

#include <stdlib.h>
#include <string.h>
#include "ha_base_config.h"


typedef struct
{
    float value;
    number_settings_t settings;
} ha_number_t;

static void ha_number_on_change_cb(void *config, char *data, uint16_t data_len);


ha_config_handle_t ha_number_init(char *name, float value, number_settings_t *settings)
{
    ha_config_handle_t config_base = ha_base_config_init(name, NUMBER);
    ha_number_t *config = (ha_number_t*)malloc(sizeof(ha_number_t));

    config->value = value;
    config->settings = *settings;
    if (settings->on_change_cb)
    {
        config_base->on_change_cb = ha_number_on_change_cb;
    }
    config_base->config_spec = config;

    return config_base;
}

void ha_number_set_value(ha_config_handle_t ha_config, float value)
{
    ha_number_t *config = (ha_number_t*)ha_config->config_spec;
    config->value = value;
}

float ha_number_get_value(ha_config_handle_t ha_config)
{
    ha_number_t *config = (ha_number_t*)ha_config->config_spec;
    return config->value;
}

const char* ha_number_get_device_class_str(ha_config_handle_t ha_config)
{
    ha_number_t *config = (ha_number_t*)ha_config->config_spec;
    if (config->settings.cls == DEV_CLASS_NONE)
    {
        return NULL;
    }
    return dev_class_str[(int)config->settings.cls - 1];
}

void ha_number_get_private_fields(ha_config_handle_t ha_config, cJSON *obj)
{
    ha_number_t *config = (ha_number_t*)ha_config->config_spec;

    cJSON_AddNumberToObject(obj, "min", config->settings.min);
    cJSON_AddNumberToObject(obj, "max", config->settings.max);
    cJSON_AddNumberToObject(obj, "step", config->settings.step);
    cJSON_AddStringToObject(obj, "mode", number_mode_str[config->settings.mode]);
}

cJSON* ha_number_get_value_norm(ha_config_handle_t ha_config)
{
    ha_number_t *config = (ha_number_t*)ha_config->config_spec;

    return cJSON_CreateNumber(config->value);
}

static void ha_number_on_change_cb(void *config, char *data, uint16_t data_len)
{
    ha_config_handle_t ha_config = (ha_config_handle_t)config;
    ha_number_t *conf = (ha_number_t*)ha_config->config_spec;
    char number[16];

    memcpy(number, data, data_len);
    number[data_len] = 0x00;

    float new_value = atof(number);
    if ((conf->settings.min <= new_value) &&
        (conf->settings.max >= new_value) &&
        (conf->settings.on_change_cb(ha_config, new_value)))
    {
        conf->value = new_value;
    }
}
