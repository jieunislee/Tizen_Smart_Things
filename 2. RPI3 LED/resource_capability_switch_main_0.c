/*
 * resource_capability_switch_main_0.c
 *
 *  Created on: Apr 25, 2018
 *      Author: Changmin
 */

#include <stdio.h>
#include <stdbool.h>
#include "st_things.h"
#include "log.h"

static const char* PROP_POWER = "power";


// For Switch Resource On/Off Value //

#define VALUE_STR_LEN_MAX 32

static const char* VALUE_SWITCH_ON = "on";
static const char* VALUE_SWITCH_OFF = "off";
static char g_switch[VALUE_STR_LEN_MAX+1] = "off";

// For GPIO Control //

#include <unistd.h>
#include <glib.h>
#include <peripheral_io.h>

static int current_state = 0;

peripheral_gpio_h gpio = NULL;
int pin = 25;


bool handle_get_request_on_resource_capability_switch_main_0(st_things_get_request_message_s* req_msg, st_things_representation_s* resp_rep)
{
    DBG("Received a GET request on %s\n", req_msg->resource_uri);

    if (req_msg->has_property_key(req_msg, PROP_POWER)) {
    	resp_rep->set_str_value(resp_rep, PROP_POWER, g_switch);
    }
    return true;
}

bool handle_set_request_on_resource_capability_switch_main_0(st_things_set_request_message_s* req_msg, st_things_representation_s* resp_rep)
{
    DBG("Received a SET request on %s\n", req_msg->resource_uri);

    peripheral_gpio_open(pin, &gpio);
    peripheral_gpio_set_direction(gpio, PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);

    char *str_value = NULL;
	req_msg->rep->get_str_value(req_msg->rep, PROP_POWER, &str_value);


	if ((0 != strncmp(str_value, VALUE_SWITCH_ON, strlen(VALUE_SWITCH_ON)))
		&& (0 != strncmp(str_value, VALUE_SWITCH_OFF, strlen(VALUE_SWITCH_OFF)))) {
		ERR("Need On/OFF value");
		free(str_value);

		return false;
	}


    if (0 != strncmp(str_value, g_switch, strlen(g_switch))) {
        strncpy(g_switch, str_value, VALUE_STR_LEN_MAX);

        if (0 == strncmp(g_switch, VALUE_SWITCH_ON, strlen(VALUE_SWITCH_ON))){
        	DBG("LED ON");
        	current_state = 1;
        }
        else{
        	DBG("LED OFF");
        	current_state = 0;
        }
    }

    peripheral_gpio_write(gpio, current_state);
	DBG("Sent data to GPIO pin %d", pin);

    resp_rep->set_str_value(resp_rep, PROP_POWER, g_switch);
	st_things_notify_observers(req_msg->resource_uri);
	free(str_value);

    return true;
}
