// https://www.home-assistant.io/integrations/button.mqtt/

#include "ha_button.h"

#include <stdlib.h>
#include <string.h>
#include "ha_base_config.h"


typedef struct
{
    button_class_e cls;
    on_button_click_cb_t on_click_cb;
} ha_button_t;

static bool ha_button_on_change_cb(void *config, char *data, uint16_t data_len);


ha_config_handle_t ha_button_init(char *name, button_class_e cls, on_button_click_cb_t on_click_cb)
{
    ha_config_handle_t config_base = ha_base_config_init(name, BUTTON);
    ha_button_t *config = (ha_button_t*)malloc(sizeof(ha_button_t));

    config->cls = cls;
    if (on_click_cb)
    {
        config->on_click_cb = on_click_cb;

        // for button we have only one command topic - just create it here
        ha_topic_cb_list_entry_t *record = (ha_topic_cb_list_entry_t*)malloc(sizeof(ha_topic_cb_list_entry_t));
        strcpy(record->var_name, "command_topic");
        strcpy(record->topic, config_base->name_norm);
        strcat(record->topic, "_click");
        record->topic_cb = ha_button_on_change_cb;

        SLIST_INSERT_HEAD(&config_base->topic_cb_list, record, list_entry);
    }
    config_base->config_spec = config;
    config_base->has_value = false;

    return config_base;
}

const char* ha_button_get_device_class_str(ha_config_handle_t ha_config)
{
    ha_button_t *config = (ha_button_t*)ha_config->config_spec;
    if (config->cls == BUTTON_NONE)
    {
        return NULL;
    }
    return button_class_str[(int)config->cls - 1];
}

static bool ha_button_on_change_cb(void *config, char *data, uint16_t data_len)
{
    ha_config_handle_t ha_config = (ha_config_handle_t)config;
    ha_button_t *conf = (ha_button_t*)ha_config->config_spec;

    conf->on_click_cb(ha_config);

    return false;
}
