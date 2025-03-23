#ifndef __HA_DEVICE_H
#define __HA_DEVICE_H

#include "ha_types.h"
#include <sys/queue.h>
#include "mqtt_client.h"


typedef struct
{
    char *mqtt_server;
    char *mqtt_username;
    char *mqtt_password;
    char *device_name;
    char *device_unique_prefix;
    char *ha_topic_prefix;
    char *manufacturer;
    char *model;
//    char *sw_version;
} ha_device_config_t;

typedef struct ha_config_list_entry_s
{
    ha_config_handle_t config;
    SLIST_ENTRY(ha_config_list_entry_s) list_entry;
} ha_config_list_entry_t;

typedef struct
{
    char device_name[HA_NAME_MAX_SIZE];
    char device_unique_prefix[HA_PREFIX_MAX_SIZE];
    char ha_topic_prefix[HA_TOPIC_PREFIX_MAX_SIZE];
    char manufacturer[HA_DEVICE_MANUFACTURER_MAX_SIZE];
    char model[HA_DEVICE_MODEL_MAX_SIZE];

    SLIST_HEAD(ha_config_list, ha_config_list_entry_s) configs;

    esp_mqtt_client_handle_t mqtt;
    bool mqtt_connected;
    bool is_registered;

    char command_topic_format[HA_PREFIX_MAX_SIZE+1+HA_NAME_MAX_SIZE+1+3+1+2];

    char *current_data;
} ha_device_t;

typedef ha_device_t* ha_device_handle_t;


ha_device_handle_t ha_device_init(ha_device_config_t *config);
int ha_device_add_config(ha_device_handle_t ha_dev, ha_config_handle_t config);
int ha_device_start(ha_device_handle_t ha_dev);
int ha_device_stop(ha_device_handle_t ha_dev);
int ha_device_commit(ha_device_handle_t ha_dev);
bool ha_device_is_registered(ha_device_handle_t ha_dev);

#endif //__HA_DEVICE_H