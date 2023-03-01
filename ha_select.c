#include "ha_select.h"

#include <stdlib.h>
#include <string.h>
#include "ha_base_config.h"


typedef struct
{
    uint8_t value;
    const char **options;
    uint8_t options_len;
    on_select_change_cb_t on_change_cb;
} ha_select_t;

static void ha_sensor_on_change_cb(void *config, char *data, uint16_t data_len);


ha_config_handle_t ha_select_init(char *name, const char **options, uint8_t options_len, uint8_t value, on_select_change_cb_t on_change_cb)
{
    ha_config_handle_t config_base = ha_base_config_init(name, SELECT);
    ha_select_t *config = (ha_select_t*)malloc(sizeof(ha_select_t));

    config->value = value;
    config->options = options;
    config->options_len = options_len;
    if (on_change_cb)
    {
        config->on_change_cb = on_change_cb;
        config_base->on_change_cb = ha_sensor_on_change_cb;
    }
    config_base->config_spec = config;

    return config_base;
}

void ha_select_set_value(ha_config_handle_t ha_config, uint8_t value)
{
    ha_select_t *config = (ha_select_t*)ha_config->config_spec;
    config->value = value;
}

uint8_t ha_select_get_value(ha_config_handle_t ha_config)
{
    ha_select_t *config = (ha_select_t*)ha_config->config_spec;
    return config->value;
}

const char* ha_select_get_device_class_str(ha_config_handle_t ha_config)
{
    return NULL;
}

void ha_select_get_private_fields(ha_config_handle_t ha_config, cJSON *obj)
{
    ha_select_t *config = (ha_select_t*)ha_config->config_spec;

    cJSON_AddItemToObject(obj, "options", cJSON_CreateStringArray(config->options, config->options_len));
}

cJSON* ha_select_get_value_norm(ha_config_handle_t ha_config)
{
    ha_select_t *config = (ha_select_t*)ha_config->config_spec;

    return cJSON_CreateString(config->options[config->value]);
}

static void ha_sensor_on_change_cb(void *config, char *data, uint16_t data_len)
{
    ha_config_handle_t ha_config = (ha_config_handle_t)config;
    ha_select_t *conf = (ha_select_t*)ha_config->config_spec;

    for (uint8_t i = 0; i < conf->options_len; i++)
    {
        if (data_len == strlen(conf->options[i]))
        {
            if (strncmp(data, conf->options[i], data_len) == 0)
            {
                if (conf->on_change_cb(ha_config, i))
                {
                    conf->value = i;
                }
            }
        }
    }
}
