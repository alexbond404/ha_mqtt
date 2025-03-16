#ifndef __HA_CONFIG_BASE_H
#define __HA_CONFIG_BASE_H

#include "ha_types.h"
#include "cJSON.h"


ha_config_handle_t ha_base_config_init(char *name, config_type_e type);
const char* ha_base_config_get_device_type_str(ha_config_handle_t ha_config);
const char* ha_base_config_get_device_name_str(ha_config_handle_t ha_config);
const char* ha_base_config_get_device_name_norm_str(ha_config_handle_t ha_config);
bool ha_base_config_has_device_class(ha_config_handle_t ha_config);
const char* ha_base_config_get_device_class_str(ha_config_handle_t ha_config);
void ha_base_config_get_private_fields(ha_config_handle_t ha_config, cJSON *obj);
cJSON* ha_base_config_get_value_norm(ha_config_handle_t ha_config);
void ha_base_config_append_value_norm(ha_config_handle_t ha_config, cJSON *obj);
bool ha_base_config_has_command_cb(ha_config_handle_t ha_config);
bool ha_base_config_has_value(ha_config_handle_t ha_config);

#endif //__HA_CONFIG_BASE_H