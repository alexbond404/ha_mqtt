#ifndef __HA_BUTTON_H
#define __HA_BUTTON_H

#include "ha_types.h"


typedef void (*on_button_click_cb_t)(ha_config_handle_t config);

ha_config_handle_t ha_button_init(char *name, button_class_e cls, on_button_click_cb_t on_change_cb);

#endif //__HA_BUTTON_H