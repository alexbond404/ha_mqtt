#include "ha_number.h"

#include <stdlib.h>
#include <string.h>
#include "ha_base_config.h"


typedef struct
{
    float value;
    float min;
    float max;
    float step;
    on_number_change_cb_t on_change_cb;
} ha_number_t;

static void ha_number_on_change_cb(void *config, char *data, uint16_t data_len);


ha_config_handle_t ha_number_init(char *name, float min, float max, float step, float value, on_number_change_cb_t on_change_cb)
{
    ha_config_handle_t config_base = ha_base_config_init(name, NUMBER);
    ha_number_t *config = (ha_number_t*)malloc(sizeof(ha_number_t));

    config->value = value;
    config->min = min;
    config->max = max;
    config->step = step;
    if (on_change_cb)
    {
        config->on_change_cb = on_change_cb;
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
    return NULL;
}

void ha_number_get_private_fields(ha_config_handle_t ha_config, cJSON *obj)
{
    ha_number_t *config = (ha_number_t*)ha_config->config_spec;

    cJSON_AddItemToObject(obj, "min", cJSON_CreateNumber(config->min));
    cJSON_AddItemToObject(obj, "max", cJSON_CreateNumber(config->max));
    cJSON_AddItemToObject(obj, "step", cJSON_CreateNumber(config->step));
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

    conf->value = atof(number);
}
