#include "ha_device.h"

#include <string.h>
#include "esp_log.h"
#include "cJSON.h"

#include "ha_base_config.h"


static const char *TAG = "ha_dev";

static void ha_device_update_ha(ha_device_handle_t ha_dev);


static char* toLower(char *str)
{
    for (char *p = str; *p; p++)
    {
        *p = tolower(*p);
    }
    return str;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ha_device_handle_t dev = (ha_device_handle_t)handler_args;
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:
        {
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

            char topic[HA_TOPIC_PREFIX_MAX_SIZE+7];
            snprintf(topic, sizeof(topic), "%s/status", dev->ha_topic_prefix);
            esp_mqtt_client_subscribe(client, topic, 0);

            if (!dev->is_registered)
            {
                ha_device_update_ha(dev);
                dev->is_registered = true;
            }

            dev->mqtt_connected = true;
        }
        break;

        case MQTT_EVENT_DISCONNECTED:
        {
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            dev->mqtt_connected = false;
        }
        break;

        case MQTT_EVENT_DATA:
        {
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");

            char topic[HA_PREFIX_MAX_SIZE+1+HA_NAME_MAX_SIZE+1+3+1+2];
            if (event->topic_len+1 < sizeof(topic))
            {
                memcpy(topic, event->topic, event->topic_len);
                topic[event->topic_len] = 0x00;

                char dev_name[HA_NAME_MAX_SIZE];
                if (sscanf(topic, dev->command_topic_format, dev_name) == 1)
                {
                    ha_config_list_entry_t *it;
                    LIST_FOREACH(it, &dev->configs, list_entry)
                    {
                        if (strcmp(dev_name, ha_base_config_get_device_name_norm_str(it->config)) == 0)
                        {
                            if (ha_base_config_has_on_change_cb(it->config))
                            {
                                ha_base_config_call_on_change_cb(it->config, event->data, event->data_len);
                            }
                            break;
                        }
                    }
                }
            }
        }
        break;

        default:
        {
            ESP_LOGI(TAG, "MQTT event id: %d", event_id);
        }
        break;
    }
}

static void ha_device_update_ha(ha_device_handle_t ha_dev)
{
    // invalidate current data
    char topic[HA_PREFIX_MAX_SIZE+1+CONFIG_DEVICE_TYPE_MAX_SIZE+1+HA_NAME_MAX_SIZE+1+HA_NAME_MAX_SIZE+1+6];
    snprintf(topic, sizeof(topic), "%s/%s", ha_dev->device_unique_prefix, ha_dev->device_name);
    esp_mqtt_client_publish(ha_dev->mqtt, topic, NULL, 0, 0, 0);

    // remove previous configs
    ha_config_list_entry_t *it;
    LIST_FOREACH(it, &ha_dev->configs, list_entry)
    {
        snprintf(topic, sizeof(topic), "%s/%s/%s/%s/config", ha_dev->ha_topic_prefix, ha_base_config_get_device_type_str(it->config),
                 ha_dev->device_name, ha_base_config_get_device_name_norm_str(it->config));

        esp_mqtt_client_publish(ha_dev->mqtt, topic, NULL, 0, 0, 0);
    }

    // adding new configs
    LIST_FOREACH(it, &ha_dev->configs, list_entry)
    {
        char tmp_str[HA_PREFIX_MAX_SIZE+HA_NAME_MAX_SIZE+1+HA_NAME_MAX_SIZE];

        cJSON *root = cJSON_CreateObject();

        // add device part
        cJSON *device = cJSON_CreateObject();
        cJSON *identifiers = cJSON_CreateArray();
        snprintf(tmp_str, sizeof(tmp_str), "%s_%s", ha_dev->device_unique_prefix, ha_dev->device_name);
        cJSON_AddItemToArray(identifiers, cJSON_CreateString(toLower(tmp_str)));
        cJSON_AddItemToObject(device, "identifiers", identifiers);
        cJSON_AddItemToObject(device, "name", cJSON_CreateString(ha_dev->device_name));
        if (ha_dev->manufacturer)
        {
            cJSON_AddItemToObject(device, "manufacturer", cJSON_CreateString(ha_dev->manufacturer));
        }
        if (ha_dev->model)
        {
            cJSON_AddItemToObject(device, "model", cJSON_CreateString(ha_dev->model));
        }
        cJSON_AddItemToObject(root, "device", device);

        cJSON_AddItemToObject(root, "enabled_by_default", cJSON_CreateBool(true));

        snprintf(topic, sizeof(topic), "%s/%s", ha_dev->device_unique_prefix, ha_dev->device_name);
        cJSON_AddItemToObject(root, "json_attributes_topic", cJSON_CreateString(topic));

        snprintf(tmp_str, sizeof(tmp_str), "%s %s", ha_dev->device_name, ha_base_config_get_device_name_str(it->config));
        cJSON_AddItemToObject(root, "name", cJSON_CreateString(tmp_str));

        cJSON_AddItemToObject(root, "state_topic", cJSON_CreateString(topic));

        snprintf(tmp_str, sizeof(tmp_str), "%s_%s_%s", ha_dev->device_name, ha_base_config_get_device_name_str(it->config), ha_dev->device_unique_prefix);
        cJSON_AddItemToObject(root, "unique_id", cJSON_CreateString(toLower(tmp_str)));

        if (ha_base_config_has_value(it->config))
        {
            snprintf(tmp_str, sizeof(tmp_str), "{{ value_json.%s }}", ha_base_config_get_device_name_norm_str(it->config));
            cJSON_AddItemToObject(root, "value_template", cJSON_CreateString(toLower(tmp_str)));
        }

        cJSON_AddItemToObject(root, "platform", cJSON_CreateString("mqtt"));

        // TODO: add sw_version

        if (ha_base_config_has_device_class(it->config))
        {
            cJSON_AddItemToObject(root, "device_class", cJSON_CreateString(ha_base_config_get_device_class_str(it->config)));
        }

        if (ha_base_config_has_on_change_cb(it->config))
        {
            snprintf(topic, sizeof(topic), ha_dev->command_topic_format, ha_base_config_get_device_name_norm_str(it->config));
            cJSON_AddItemToObject(root, "command_topic", cJSON_CreateString(topic));
            esp_mqtt_client_subscribe(ha_dev->mqtt, topic, 0);
        }

        // add private config part
        ha_base_config_get_private_fields(it->config, root);

        // publish config
        char *payload = cJSON_PrintUnformatted(root);
        cJSON_Delete(root);

        snprintf(topic, sizeof(topic), "%s/%s/%s/%s/config", ha_dev->ha_topic_prefix, ha_base_config_get_device_type_str(it->config),
                 ha_dev->device_name, ha_base_config_get_device_name_norm_str(it->config));
        esp_mqtt_client_publish(ha_dev->mqtt, topic, payload, strlen(payload), 0, 0);

        free(payload);
    }
}

static void ha_device_publish_data(ha_device_handle_t ha_dev)
{
    if (ha_dev->mqtt_connected && ha_dev->is_registered)
    {
        char topic[HA_PREFIX_MAX_SIZE+1+HA_NAME_MAX_SIZE];
        snprintf(topic, sizeof(topic), "%s/%s", ha_dev->device_unique_prefix, ha_dev->device_name);

//        ESP_LOGI(TAG, "data publish: %s\n%s", topic, ha_dev->current_data);
        esp_mqtt_client_publish(ha_dev->mqtt, topic, ha_dev->current_data, strlen(ha_dev->current_data), 0, 0);
    }
}


ha_device_handle_t ha_device_init(ha_device_config_t *config)
{
    ha_device_handle_t dev = (ha_device_handle_t)malloc(sizeof(ha_device_t));

    strcpy(dev->device_name, config->device_name);
    if (config->ha_topic_prefix != NULL)
    {
        strcpy(dev->ha_topic_prefix, config->ha_topic_prefix);
    }
    else
    {
        strcpy(dev->ha_topic_prefix, HA_TOPIC_PREFIX_DEFAULT);
    }

    strcpy(dev->device_unique_prefix, config->device_unique_prefix);

    if (config->manufacturer != NULL)
    {
        strcpy(dev->manufacturer, config->manufacturer);
    }

    if (config->model != NULL)
    {
        strcpy(dev->model, config->model);
    }

    LIST_INIT(&dev->configs);

    esp_mqtt_client_config_t mqtt_cfg = {
        .host = config->mqtt_server,
        .username = config->mqtt_username,
        .password = config->mqtt_password,
    };

    dev->mqtt = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(dev->mqtt, ESP_EVENT_ANY_ID, mqtt_event_handler, dev);

    dev->mqtt_connected = false;
    dev->is_registered = false;

    snprintf(dev->command_topic_format, sizeof(dev->command_topic_format), "%s/%s/set/%%s", dev->device_unique_prefix, dev->device_name);

    dev->current_data = NULL;

    return dev;
}

int ha_device_add_config(ha_device_handle_t ha_dev, ha_config_handle_t config)
{
    ha_config_list_entry_t *record = (ha_config_list_entry_t*)malloc(sizeof(ha_config_list_entry_t));
    record->config = config;

    ha_config_list_entry_t *head = LIST_FIRST(&ha_dev->configs);
    if (head == NULL)
    {
        LIST_INSERT_HEAD(&ha_dev->configs, record, list_entry);
    }
    else
    {
        LIST_INSERT_BEFORE(head, record, list_entry);
    }

    return 0;
}

int ha_device_start(ha_device_handle_t ha_dev)
{
    esp_mqtt_client_start(ha_dev->mqtt);
    return 0;
}

int ha_device_stop(ha_device_handle_t ha_dev)
{
    esp_mqtt_client_disconnect(ha_dev->mqtt);
    return 0;
}

int ha_device_commit(ha_device_handle_t ha_dev)
{
    cJSON *root = cJSON_CreateObject();

    ha_config_list_entry_t *it;
    LIST_FOREACH(it, &ha_dev->configs, list_entry)
    {
        if (ha_base_config_has_value(it->config))
        {
            cJSON_AddItemToObject(root, ha_base_config_get_device_name_norm_str(it->config), ha_base_config_get_value_norm(it->config));
        }
    }
    if (ha_dev->current_data)
    {
        free(ha_dev->current_data);
    }
    ha_dev->current_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    ha_device_publish_data(ha_dev);

    return 0;
}
