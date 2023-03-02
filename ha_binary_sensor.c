#include "ha_binary_sensor.h"

#include <stdlib.h>
#include "ha_base_config.h"


typedef struct
{
    dev_class_e cls;
    bool value;
} ha_binary_sensor_t;

static const char state_on[] = "on";
static const char state_off[] = "off";


ha_config_handle_t ha_binary_sensor_init(char *name, dev_class_e cls, bool value)
{
    ha_config_handle_t config_base = ha_base_config_init(name, BINARY_SENSOR);
    ha_binary_sensor_t *config = (ha_binary_sensor_t*)malloc(sizeof(ha_binary_sensor_t));

    config->cls = cls;
    config->value = value;
    config_base->config_spec = config;

    return config_base;
}

void ha_binary_sensor_set_value(ha_config_handle_t ha_config, bool value)
{
    ha_binary_sensor_t *config = (ha_binary_sensor_t*)ha_config->config_spec;
    config->value = value;
}

const char* ha_binary_sensor_get_device_class_str(ha_config_handle_t ha_config)
{
    ha_binary_sensor_t *config = (ha_binary_sensor_t*)ha_config->config_spec;
    if (config->cls == DEV_CLASS_NONE)
    {
        return NULL;
    }
    return dev_class_str[(int)config->cls - 1];
}

void ha_binary_sensor_get_private_fields(ha_config_handle_t ha_config, cJSON *obj)
{
    cJSON_AddItemToObject(obj, "payload_off", cJSON_CreateString(state_off));
    cJSON_AddItemToObject(obj, "payload_on", cJSON_CreateString(state_on));
}

cJSON* ha_binary_sensor_get_value_norm(ha_config_handle_t ha_config)
{
    ha_binary_sensor_t *config = (ha_binary_sensor_t*)ha_config->config_spec;

    if (config->value)
    {
        return cJSON_CreateString(state_on);
    }
    else
    {
        return cJSON_CreateString(state_off);
    }
}
