#include "ha_switch.h"

#include <stdlib.h>
#include <string.h>
#include "ha_base_config.h"


typedef struct
{
    bool value;
    on_switch_change_cb_t on_change_cb;
} ha_switch_t;

typedef struct
{
    const char *str;
    const bool value;
} state_value_t;

static const char state_on[] = "on";
static const char state_off[] = "off";
static const state_value_t states[] = {{.str = state_on, .value = true},
                                       {.str = state_off, .value = false}};

static void ha_switch_on_change_cb(void *config, char *data, uint16_t data_len);


ha_config_handle_t ha_switch_init(char *name, bool value, on_switch_change_cb_t on_change_cb)
{
    ha_config_handle_t config_base = ha_base_config_init(name, SWITCH);
    ha_switch_t *config = (ha_switch_t*)malloc(sizeof(ha_switch_t));

    config->value = value;
    if (on_change_cb)
    {
        config->on_change_cb = on_change_cb;
        config_base->on_change_cb = ha_switch_on_change_cb;
    }
    config_base->config_spec = config;

    return config_base;
}

void ha_switch_set_value(ha_config_handle_t ha_config, bool value)
{
    ha_switch_t *config = (ha_switch_t*)ha_config->config_spec;
    config->value = value;
}

bool ha_switch_get_value(ha_config_handle_t ha_config)
{
    ha_switch_t *config = (ha_switch_t*)ha_config->config_spec;
    return config->value;
}

const char* ha_switch_get_device_class_str(ha_config_handle_t ha_config)
{
    return NULL;
}

void ha_switch_get_private_fields(ha_config_handle_t ha_config, cJSON *obj)
{
    cJSON_AddItemToObject(obj, "payload_off", cJSON_CreateString(state_off));
    cJSON_AddItemToObject(obj, "payload_on", cJSON_CreateString(state_on));
}

cJSON* ha_switch_get_value_norm(ha_config_handle_t ha_config)
{
    ha_switch_t *config = (ha_switch_t*)ha_config->config_spec;

    if (config->value)
    {
        return cJSON_CreateString(state_on);
    }
    else
    {
        return cJSON_CreateString(state_off);
    }
}

static void ha_switch_on_change_cb(void *config, char *data, uint16_t data_len)
{
    ha_config_handle_t ha_config = (ha_config_handle_t)config;
    ha_switch_t *conf = (ha_switch_t*)ha_config->config_spec;

    for (uint8_t i = 0; i < SIZEOF(states); i++)
    {
        if (data_len == strlen(states[i].str))
        {
            if (strncmp(data, states[i].str, data_len) == 0)
            {
                if (conf->on_change_cb(ha_config, states[i].value))
                {
                    conf->value = states[i].value;
                }
            }
        }
    }
}
