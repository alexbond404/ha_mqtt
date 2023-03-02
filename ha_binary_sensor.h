#ifndef __HA_BINARY_SENSOR_H
#define __HA_BINARY_SENSOR_H

#include "ha_types.h"


ha_config_handle_t ha_binary_sensor_init(char *name, dev_class_e cls, bool value);
void ha_binary_sensor_set_value(ha_config_handle_t ha_config, bool value);

#endif //__HA_BINARY_SENSOR_H