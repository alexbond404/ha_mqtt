#ifndef __HA_SELECT_H
#define __HA_SELECT_H

#include "ha_types.h"


typedef bool (*on_select_change_cb_t)(ha_config_handle_t config, uint8_t new_value);

ha_config_handle_t ha_select_init(char *name, const char **options, uint8_t options_len, uint8_t value, on_select_change_cb_t on_change_cb);
void ha_select_set_value(ha_config_handle_t ha_config, uint8_t value);
uint8_t ha_select_get_value(ha_config_handle_t ha_config);

#endif //__HA_SELECT_H
