#ifndef __HA_SWITCH_H
#define __HA_SWITCH_H

#include "ha_types.h"


typedef bool (*on_switch_change_cb_t)(ha_config_handle_t config, bool new_value);

ha_config_handle_t ha_switch_init(char *name, bool value, on_switch_change_cb_t on_change_cb);
void ha_switch_set_value(ha_config_handle_t ha_config, bool value);
bool ha_switch_get_value(ha_config_handle_t ha_config);

#endif //__HA_SWITCH_H
