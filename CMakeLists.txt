idf_component_register(SRCS "ha_base_config.c" "ha_binary_sensor.c" "ha_button.c" "ha_climate.c" "ha_device.c" "ha_number.c" "ha_select.c" "ha_sensor.c" "ha_switch.c" "ha_types.c"
                       REQUIRES mqtt json
                       INCLUDE_DIRS ".")
