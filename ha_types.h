#ifndef __HA_TYPES_H
#define __HA_TYPES_H

#include <stdint.h>
#include <stdbool.h>


#define HA_NAME_MAX_SIZE                32
#define HA_PREFIX_MAX_SIZE              32
#define HA_TOPIC_PREFIX_MAX_SIZE        16

#define HA_TOPIC_PREFIX_DEFAULT         "homeassistant"

#define HA_DEVICE_MANUFACTURER_MAX_SIZE 64
#define HA_DEVICE_MODEL_MAX_SIZE        64

#define SIZEOF(type)                    (sizeof(type)/sizeof(type[0]))

#define CONFIG_DEVICE_TYPE_MAX_SIZE     16
typedef enum
{
    BINARY_SENSOR = 0,
    NUMBER,
    SELECT,
    SENSOR,
    SWITCH,
} config_type_e;
extern const char *config_type_str[];


typedef enum
{
    BINARY_SENSOR_NONE = 0,
    BINARY_SENSOR_PRESENCE,
} binary_sensor_class_e;
extern const char *binary_sensor_class_str[];


typedef enum
{
    SENSOR_NONE = 0,
} sensor_class_e;
extern const char *sensor_class_str[];


typedef void (*on_change_cb_t)(void *config, char *data, uint16_t data_len);

typedef struct 
{
    char name[HA_NAME_MAX_SIZE];
    char name_norm[HA_NAME_MAX_SIZE];
    config_type_e type;
    on_change_cb_t on_change_cb;

    void *config_spec;
} ha_config_t;
typedef ha_config_t* ha_config_handle_t;


#endif //__HA_TYPES_H