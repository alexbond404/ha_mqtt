#ifndef __HA_CLIMATE_H
#define __HA_CLIMATE_H

#include "ha_types.h"

typedef bool (*on_temperature_change_cb_t)(ha_config_handle_t config, int8_t new_value);
typedef bool (*on_mode_change_cb_t)(ha_config_handle_t config, uint8_t new_mode);
typedef bool (*on_fan_mode_change_cb_t)(ha_config_handle_t config, uint8_t new_mode);

typedef struct
{
    union
    {
        struct
        {
            uint8_t curr_hum :1;
            uint8_t curr_temp :1;
            uint8_t temp :1;
            uint8_t mode :1;
            uint8_t fan_mode :1;
//            uint8_t swing_mode :1;
        };
        uint8_t value;
    } features;

    char temp_unit[2];

    // temperature feature
    int8_t temp_min;
    int8_t temp_max;
    on_temperature_change_cb_t on_temp_change_cb;

    // mode feature
    const char **modes;
    uint8_t modes_cnt;
    on_mode_change_cb_t on_mode_change_cb;

    // fan mode feature
    const char **fan_modes;
    uint8_t fan_modes_cnt;
    on_fan_mode_change_cb_t on_fan_mode_change_cb;

    // swing mode feature
    /// TODO
} climate_settings_t;

ha_config_handle_t ha_climate_init(char *name, climate_settings_t *settings);
void ha_climate_set_curr_hum_value(ha_config_handle_t ha_config, uint8_t value);
void ha_climate_set_curr_temp_value(ha_config_handle_t ha_config, int8_t value);
void ha_climate_set_temp_value(ha_config_handle_t ha_config, int8_t value);
void ha_climate_set_mode_value(ha_config_handle_t ha_config, uint8_t value);
void ha_climate_set_fan_mode_value(ha_config_handle_t ha_config, uint8_t value);


#endif //__HA_CLIMATE_H
