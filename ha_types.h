#ifndef __HA_TYPES_H
#define __HA_TYPES_H


#define HA_NAME_MAX_SIZE                32
#define HA_PREFIX_MAX_SIZE              32
#define HA_TOPIC_PREFIX_MAX_SIZE        16

#define HA_TOPIC_PREFIX_DEFAULT         "homeassistant"

#define HA_DEVICE_MANUFACTURER_MAX_SIZE 64
#define HA_DEVICE_MODEL_MAX_SIZE        64

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


typedef struct
{
    char name[HA_NAME_MAX_SIZE];
    char name_norm[HA_NAME_MAX_SIZE];
    config_type_e type;

    void *config_spec;
} ha_config_t;

typedef ha_config_t* ha_config_handle_t;

#endif //__HA_TYPES_H