#ifndef __HA_SENSOR_H
#define __HA_SENSOR_H

#include "ha_types.h"


ha_config_handle_t ha_sensor_init(char *name, sensor_class_e cls, float value);
void ha_sensor_set_value(ha_config_handle_t ha_config, float value);
bool ha_sensor_set_units_of_measurement(ha_config_handle_t ha_config, char *units);
void ha_sensor_set_value_precision(ha_config_handle_t ha_config, int8_t precision);

#endif //__HA_SENSOR_H
