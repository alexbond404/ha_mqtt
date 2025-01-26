#include "ha_base_config.h"

#include <ctype.h>
#include <string.h>

#include "ha_binary_sensor.h"
#include "ha_sensor.h"


static const char forbidden_symbols[] = " .";

extern const char* ha_binary_sensor_get_device_class_str(ha_config_handle_t ha_config);
extern const char* ha_button_get_device_class_str(ha_config_handle_t ha_config);
extern const char* ha_number_get_device_class_str(ha_config_handle_t ha_config);
extern const char* ha_select_get_device_class_str(ha_config_handle_t ha_config);
extern const char* ha_sensor_get_device_class_str(ha_config_handle_t ha_config);
extern const char* ha_switch_get_device_class_str(ha_config_handle_t ha_config);
extern void ha_binary_sensor_get_private_fields(ha_config_handle_t ha_config, cJSON *obj);
extern void ha_number_get_private_fields(ha_config_handle_t ha_config, cJSON *obj);
extern void ha_select_get_private_fields(ha_config_handle_t ha_config, cJSON *obj);
extern void ha_sensor_get_private_fields(ha_config_handle_t ha_config, cJSON *obj);
extern void ha_switch_get_private_fields(ha_config_handle_t ha_config, cJSON *obj);
extern cJSON* ha_binary_sensor_get_value_norm(ha_config_handle_t ha_config);
extern cJSON* ha_number_get_value_norm(ha_config_handle_t ha_config);
extern cJSON* ha_select_get_value_norm(ha_config_handle_t ha_config);
extern cJSON* ha_sensor_get_value_norm(ha_config_handle_t ha_config);
extern cJSON* ha_switch_get_value_norm(ha_config_handle_t ha_config);


ha_config_handle_t ha_base_config_init(char *name, config_type_e type)
{
    ha_config_handle_t config = (ha_config_handle_t)malloc(sizeof(ha_config_t));
    strcpy(config->name, name);
    for (int i = 0; i < HA_NAME_MAX_SIZE; i++)
    {
        if (name[i] == 0x00)
        {
            config->name_norm[i] = 0x00;
            break;
        }

        if (strchr(forbidden_symbols, name[i]) != NULL)
        {
            config->name_norm[i] = '_';
        }
        else
        {
            config->name_norm[i] = tolower(name[i]);
        }
    }
    config->type = type;
    config->on_change_cb = NULL;
    config->has_value = true;

    return config;
}

const char* ha_base_config_get_device_name_str(ha_config_handle_t ha_config)
{
    return ha_config->name;
}

const char* ha_base_config_get_device_type_str(ha_config_handle_t ha_config)
{
    return config_type_str[ha_config->type];
}

const char* ha_base_config_get_device_name_norm_str(ha_config_handle_t ha_config)
{
    return ha_config->name_norm;
}

bool ha_base_config_has_device_class(ha_config_handle_t ha_config)
{
    return (ha_base_config_get_device_class_str(ha_config) != NULL);
}

const char* ha_base_config_get_device_class_str(ha_config_handle_t ha_config)
{
    switch (ha_config->type)
    {
        case BINARY_SENSOR:
            return ha_binary_sensor_get_device_class_str(ha_config);
        case BUTTON:
            return ha_button_get_device_class_str(ha_config);
        case NUMBER:
            return ha_number_get_device_class_str(ha_config);
        case SELECT:
            return ha_select_get_device_class_str(ha_config);
        case SENSOR:
            return ha_sensor_get_device_class_str(ha_config);
        case SWITCH:
            return ha_switch_get_device_class_str(ha_config);

        default:
            return NULL;
    }
}

void ha_base_config_get_private_fields(ha_config_handle_t ha_config, cJSON *obj)
{
    switch (ha_config->type)
    {
        case BINARY_SENSOR:
            ha_binary_sensor_get_private_fields(ha_config, obj);
            break;
        case NUMBER:
            ha_number_get_private_fields(ha_config, obj);
            break;
        case SELECT:
            ha_select_get_private_fields(ha_config, obj);
            break;
        case SENSOR:
            ha_sensor_get_private_fields(ha_config, obj);
            break;
        case SWITCH:
            ha_switch_get_private_fields(ha_config, obj);
            break;

        default:
            break;
    }
}

cJSON* ha_base_config_get_value_norm(ha_config_handle_t ha_config)
{
    switch (ha_config->type)
    {
        case BINARY_SENSOR:
            return ha_binary_sensor_get_value_norm(ha_config);
        case NUMBER:
            return ha_number_get_value_norm(ha_config);
        case SELECT:
            return ha_select_get_value_norm(ha_config);
        case SENSOR:
            return ha_sensor_get_value_norm(ha_config);
        case SWITCH:
            return ha_switch_get_value_norm(ha_config);

        default:
            return NULL;
    }
}

bool ha_base_config_has_on_change_cb(ha_config_handle_t ha_config)
{
    return (ha_config->on_change_cb != NULL);
}

void ha_base_config_call_on_change_cb(ha_config_handle_t ha_config, char *data, int data_len)
{
    return ha_config->on_change_cb(ha_config, data, data_len);
}

bool ha_base_config_has_value(ha_config_handle_t ha_config)
{
    return ha_config->has_value;
}
