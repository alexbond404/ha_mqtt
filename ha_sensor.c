// https://www.home-assistant.io/integrations/sensor.mqtt/

#include "ha_sensor.h"

#include <stdlib.h>
#include <string.h>
#include "ha_base_config.h"


typedef struct
{
    sensor_class_e cls;
    char units[10];
    float value;
} ha_sensor_t;


ha_config_handle_t ha_sensor_init(char *name, sensor_class_e cls, float value)
{
    ha_config_handle_t config_base = ha_base_config_init(name, SENSOR);
    ha_sensor_t *config = (ha_sensor_t*)malloc(sizeof(ha_sensor_t));

    config->cls = cls;
    config->units[0] = 0x00;
    config->value = value;
    config_base->config_spec = config;

    return config_base;
}

void ha_sensor_set_value(ha_config_handle_t ha_config, float value)
{
    ha_sensor_t *config = (ha_sensor_t*)ha_config->config_spec;
    config->value = value;
}

const char* ha_sensor_get_device_class_str(ha_config_handle_t ha_config)
{
    ha_sensor_t *config = (ha_sensor_t*)ha_config->config_spec;
    if (config->cls == SENSOR_NONE)
    {
        return NULL;
    }
    return sensor_class_str[(int)config->cls - 1];
}

void ha_sensor_get_private_fields(ha_config_handle_t ha_config, cJSON *obj)
{
    ha_sensor_t *config = (ha_sensor_t*)ha_config->config_spec;
    if (strlen(config->units))
    {
        cJSON_AddItemToObject(obj, "unit_of_measurement", cJSON_CreateString(config->units));
    }
}

cJSON* ha_sensor_get_value_norm(ha_config_handle_t ha_config)
{
    ha_sensor_t *config = (ha_sensor_t*)ha_config->config_spec;

    return cJSON_CreateNumber(config->value);
}

bool ha_sensor_set_units_of_measurement(ha_config_handle_t ha_config, char *units)
{
    ha_sensor_t *config = (ha_sensor_t*)ha_config->config_spec;
    if (strlen(units) + 1 <= sizeof(config->units))
    {
        strcpy(config->units, units);
        return true;
    }
    return false;
}
