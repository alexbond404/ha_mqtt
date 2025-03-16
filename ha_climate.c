// https://www.home-assistant.io/integrations/climate.mqtt/

#include "ha_climate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include "cJSON.h"

#include "ha_base_config.h"


typedef struct
{
    climate_settings_t settings;
    uint8_t curr_hum;
    int8_t curr_temp;
    int8_t temp;
    uint8_t mode;
    uint8_t fan_mode;
} ha_climate_t;

static const char *curr_hum_state_topic_var = "current_humidity_topic";
static const char *curr_temp_state_topic_var = "current_temperature_topic";
static const char *temp_state_topic_var = "temperature_state_topic";
static const char *mode_state_topic_var = "mode_state_topic";
static const char *fan_mode_state_topic_var = "fan_mode_state_topic";

static bool ha_climate_temperature_topic_cb(void *config, char *data, uint16_t data_len);
static bool ha_climate_mode_topic_cb(void *config, char *data, uint16_t data_len);
static bool ha_climate_fan_mode_topic_cb(void *config, char *data, uint16_t data_len);


static uint8_t get_set_bit_count(uint8_t value)
{
    uint8_t res = 0;
    while (value)
    {
        if (value & 0x1)
        {
            res++;
        }
        value >>= 1;
    }
    return res;
}

static void add_entry(ha_topic_cb_list_t *list, const char *var_name, const char *topic, topic_cb_t topic_cb)
{
    ha_topic_cb_list_entry_t *record = (ha_topic_cb_list_entry_t*)malloc(sizeof(ha_topic_cb_list_entry_t));
    strcpy(record->var_name, var_name);
    strcpy(record->topic, topic);
    record->topic_cb = topic_cb;

    ha_topic_cb_list_entry_t *head = SLIST_FIRST(list);
    if (head == NULL)
    {
        SLIST_INSERT_HEAD(list, record, list_entry);
    }
    else
    {
        SLIST_INSERT_AFTER(head, record, list_entry);
    }
}

ha_config_handle_t ha_climate_init(char *name, climate_settings_t *settings)
{
    ha_config_handle_t config_base = ha_base_config_init(name, CLIMATE);
    ha_climate_t *config = (ha_climate_t*)malloc(sizeof(ha_climate_t));

    config->settings = *settings;
    config_base->state_topic_vars_cnt = get_set_bit_count(settings->features.value);
    config_base->state_topic_vars_arr = (char**)malloc(sizeof(char*) * config_base->state_topic_vars_cnt);
    uint8_t feature_pos = 0;
    if (settings->features.curr_hum)
    {
        config->curr_hum = 0;
        config_base->state_topic_vars_arr[feature_pos] = (char*)curr_hum_state_topic_var;
        feature_pos++;
    }

    if (settings->features.curr_temp)
    {
        config->curr_temp = 0;
        config_base->state_topic_vars_arr[feature_pos] = (char*)curr_temp_state_topic_var;
        feature_pos++;
    }

    if (settings->features.temp)
    {
        config->temp = settings->temp_min;
        if (settings->on_temp_change_cb)
        {
            add_entry(&config_base->topic_cb_list, "temperature_command_topic", "on_temp_change", ha_climate_temperature_topic_cb);
        }
        config_base->state_topic_vars_arr[feature_pos] = (char*)temp_state_topic_var;
        feature_pos++;
    }
    if (settings->features.mode)
    {
        config->mode = 0;
        if (settings->on_mode_change_cb)
        {
            add_entry(&config_base->topic_cb_list, "mode_command_topic", "on_mode_change", ha_climate_mode_topic_cb);
        }
        config_base->state_topic_vars_arr[feature_pos] = (char*)mode_state_topic_var;
        feature_pos++;
    }
    if (settings->features.fan_mode)
    {
        config->fan_mode = 0;
        if (settings->on_fan_mode_change_cb)
        {
            add_entry(&config_base->topic_cb_list, "fan_mode_command_topic", "on_fan_mode_change", ha_climate_fan_mode_topic_cb);
        }
        config_base->state_topic_vars_arr[feature_pos] = (char*)fan_mode_state_topic_var;
        feature_pos++;
    }

    config_base->config_spec = config;

    return config_base;
}

void ha_climate_get_private_fields(ha_config_handle_t ha_config, cJSON *obj)
{
    ha_climate_t *config = (ha_climate_t*)ha_config->config_spec;
    const uint8_t value_template_size = HA_VALUE_TEMPLATE_MAX_SIZE + HA_NAME_MAX_SIZE;
    char value_template[value_template_size];

    if (config->settings.features.curr_temp || config->settings.features.temp)
    {
        cJSON_AddItemToObject(obj, "temperature_unit", cJSON_CreateString(config->settings.temp_unit));
    }

    if (config->settings.features.curr_hum)
    {
        snprintf(value_template, value_template_size, "{{ value_json.%s_curr_hum }}", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddItemToObject(obj, "current_humidity_template", cJSON_CreateString(value_template));
    }
    
    if (config->settings.features.curr_temp)
    {
        snprintf(value_template, value_template_size, "{{ value_json.%s_curr_temp }}", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddItemToObject(obj, "current_temperature_template", cJSON_CreateString(value_template));
    }

    if (config->settings.features.temp)
    {
        cJSON_AddItemToObject(obj, "min_temp", cJSON_CreateNumber(config->settings.temp_min));
        cJSON_AddItemToObject(obj, "max_temp", cJSON_CreateNumber(config->settings.temp_max));

        snprintf(value_template, value_template_size, "{{ value_json.%s_temp }}", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddItemToObject(obj, "temperature_state_template", cJSON_CreateString(value_template));
    }

    if (config->settings.features.mode)
    {
        cJSON_AddItemToObject(obj, "modes", cJSON_CreateStringArray(config->settings.modes, config->settings.modes_cnt));

        snprintf(value_template, value_template_size, "{{ value_json.%s_mode }}", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddItemToObject(obj, "mode_state_template", cJSON_CreateString(value_template));
    }

    if (config->settings.features.fan_mode)
    {
        cJSON_AddItemToObject(obj, "fan_modes", cJSON_CreateStringArray(config->settings.fan_modes, config->settings.fan_modes_cnt));

        snprintf(value_template, value_template_size, "{{ value_json.%s_fan_mode }}", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddItemToObject(obj, "fan_mode_state_template", cJSON_CreateString(value_template));
    }
}

void ha_climate_append_value_norm(ha_config_handle_t ha_config, cJSON *obj)
{
    ha_climate_t *config = (ha_climate_t*)ha_config->config_spec;
    const uint8_t value_template_size = HA_VALUE_TEMPLATE_MAX_SIZE + HA_NAME_MAX_SIZE;
    char value_template[value_template_size];

    if (config->settings.features.curr_hum)
    {
        snprintf(value_template, value_template_size, "%s_curr_hum", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddNumberToObject(obj, value_template, config->curr_hum);
    }

    if (config->settings.features.curr_temp)
    {
        snprintf(value_template, value_template_size, "%s_curr_temp", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddNumberToObject(obj, value_template, config->curr_temp);
    }

    if (config->settings.features.temp)
    {
        snprintf(value_template, value_template_size, "%s_temp", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddNumberToObject(obj, value_template, config->temp);
    }

    if (config->settings.features.mode)
    {
        snprintf(value_template, value_template_size, "%s_mode", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddStringToObject(obj, value_template, config->settings.modes[config->mode]);
    }

    if (config->settings.features.fan_mode)
    {
        snprintf(value_template, value_template_size, "%s_fan_mode", ha_base_config_get_device_name_norm_str(ha_config));
        cJSON_AddStringToObject(obj, value_template, config->settings.fan_modes[config->fan_mode]);
    }
}

void ha_climate_set_curr_hum_value(ha_config_handle_t ha_config, uint8_t value)
{
    ha_climate_t *config = (ha_climate_t*)ha_config->config_spec;
    config->curr_hum = value;
}

void ha_climate_set_curr_temp_value(ha_config_handle_t ha_config, int8_t value)
{
    ha_climate_t *config = (ha_climate_t*)ha_config->config_spec;
    config->curr_temp = value;
}

void ha_climate_set_temp_value(ha_config_handle_t ha_config, int8_t value)
{
    ha_climate_t *config = (ha_climate_t*)ha_config->config_spec;
    config->temp = value;
}

void ha_climate_set_mode_value(ha_config_handle_t ha_config, uint8_t value)
{
    ha_climate_t *config = (ha_climate_t*)ha_config->config_spec;
    config->mode = value;
}

void ha_climate_set_fan_mode_value(ha_config_handle_t ha_config, uint8_t value)
{
    ha_climate_t *config = (ha_climate_t*)ha_config->config_spec;
    config->fan_mode = value;
}

static bool ha_climate_temperature_topic_cb(void *config, char *data, uint16_t data_len)
{
    ha_config_handle_t ha_config = (ha_config_handle_t)config;
    ha_climate_t *conf = (ha_climate_t*)ha_config->config_spec;
    char number[16];

    memcpy(number, data, data_len);
    number[data_len] = 0x00;

    float new_value = atof(number);
    if (conf->settings.temp_min <= new_value &&
        new_value <= conf->settings.temp_max)
    {
        if (conf->settings.on_temp_change_cb(ha_config, (int8_t)new_value))
        {
            conf->temp = new_value;
            return true;
        }
    }

    return false;
}

static bool ha_climate_mode_topic_cb(void *config, char *data, uint16_t data_len)
{
    ha_config_handle_t ha_config = (ha_config_handle_t)config;
    ha_climate_t *conf = (ha_climate_t*)ha_config->config_spec;

    for (uint8_t i = 0; i < conf->settings.modes_cnt; i++)
    {
        size_t mode_len = strlen(conf->settings.modes[i]);
        if (mode_len == data_len &&
            0 == memcmp(data, conf->settings.modes[i], mode_len))
            {
                if (conf->settings.on_mode_change_cb(ha_config, i))
                {
                    conf->mode = i;
                    return true;
                }
            }
    }

    return false;
}

static bool ha_climate_fan_mode_topic_cb(void *config, char *data, uint16_t data_len)
{
    ha_config_handle_t ha_config = (ha_config_handle_t)config;
    ha_climate_t *conf = (ha_climate_t*)ha_config->config_spec;

    for (uint8_t i = 0; i < conf->settings.fan_modes_cnt; i++)
    {
        size_t fan_mode_len = strlen(conf->settings.fan_modes[i]);
        if (fan_mode_len == data_len &&
            0 == memcmp(data, conf->settings.fan_modes[i], fan_mode_len))
            {
                if (conf->settings.on_fan_mode_change_cb(ha_config, i))
                {
                    conf->fan_mode = i;
                    return true;
                }
            }
    }

    return false;
}
