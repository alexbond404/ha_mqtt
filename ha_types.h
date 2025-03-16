#ifndef __HA_TYPES_H
#define __HA_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/queue.h>


#define HA_NAME_MAX_SIZE                32
#define HA_PREFIX_MAX_SIZE              32
#define HA_TOPIC_PREFIX_MAX_SIZE        16
#define HA_TOPIC_MAX_SIZE               32
#define HA_VARIABLE_NAME_MAX_SIZE       32
#define HA_VALUE_TEMPLATE_MAX_SIZE      32

#define HA_TOPIC_PREFIX_DEFAULT         "homeassistant"

#define HA_DEVICE_MANUFACTURER_MAX_SIZE 64
#define HA_DEVICE_MODEL_MAX_SIZE        64

#define SIZEOF(type)                    (sizeof(type)/sizeof(type[0]))

#define CONFIG_DEVICE_TYPE_MAX_SIZE     16
typedef enum
{
    BINARY_SENSOR = 0,
    BUTTON,
    CLIMATE,
    NUMBER,
    SELECT,
    SENSOR,
    SWITCH,
} config_type_e;
extern const char *config_type_str[];


typedef enum
{
    DEV_CLASS_NONE = 0,
    DEV_CLASS_DOOR,
    DEV_CLASS_LOCK,
    DEV_CLASS_OCCUPANCY,
    DEV_CLASS_PRESENCE,
    DEV_CLASS_SAFETY,
    DEV_CLASS_TEMPERATURE,
} dev_class_e;
extern const char *dev_class_str[];


typedef enum
{
    BUTTON_NONE = 0,
    BUTTON_RESTART,
    BUTTON_UPDATE
} button_class_e;
extern const char *button_class_str[];


typedef enum
{
    SENSOR_NONE = 0,
    SENSOR_AQI,
    SENSOR_CARBON_DIOXIDE,
    SENSOR_HUMIDITY,
    SENSOR_ILLUMINANCE,
    SENSOR_PRESSURE,
    SENSOR_SIGNAL_STRENGTH,
    SENSOR_TEMPERATURE,
} sensor_class_e;
extern const char *sensor_class_str[];


typedef enum
{
    NUMBER_MODE_AUTO = 0,
    NUMBER_MODE_BOX,
    NUMBER_MODE_SLIDER
} number_mode_e;
extern const char *number_mode_str[];


typedef bool (*topic_cb_t)(void *config, char *data, uint16_t data_len);

typedef struct ha_topic_cb_list_entry_s
{
    char var_name[HA_VARIABLE_NAME_MAX_SIZE];
    char topic[HA_TOPIC_MAX_SIZE];
    topic_cb_t topic_cb;
    SLIST_ENTRY(ha_topic_cb_list_entry_s) list_entry;
} ha_topic_cb_list_entry_t;

typedef SLIST_HEAD(config_topic_list, ha_topic_cb_list_entry_s) ha_topic_cb_list_t;

typedef struct
{
    char name[HA_NAME_MAX_SIZE];
    char name_norm[HA_NAME_MAX_SIZE];
    config_type_e type;
    char **state_topic_vars_arr;
    uint8_t state_topic_vars_cnt;
    ha_topic_cb_list_t topic_cb_list;
    bool has_value;

    void *config_spec;
} ha_config_t;
typedef ha_config_t* ha_config_handle_t;


#endif //__HA_TYPES_H