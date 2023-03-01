#ifndef __HA_NUMBER_H
#define __HA_NUMBER_H

#include "ha_types.h"


typedef bool (*on_number_change_cb_t)(ha_config_handle_t config, float new_value);

ha_config_handle_t ha_number_init(char *name, float min, float max, float step, float value, on_number_change_cb_t on_change_cb);
void ha_number_set_value(ha_config_handle_t ha_config, float value);
float ha_number_get_value(ha_config_handle_t ha_config);

#endif //__HA_NUMBER_H