/*
 * resource_capability_switch_main_0.c
 *
 *  Created on: May 6, 2018
 *      Author: Changmin
 */

#include <stdio.h>
#include <stdbool.h>
#include "st_things.h"
#include "log.h"

// For Switch Resource On/Off Value //

#define PROP_STR_LEN_MAX 32

static const char* PROP_POWER = "power";
static const char* PROP_SWITCH_ON = "on";
static const char* PROP_SWITCH_OFF = "off";
static char g_switch[PROP_STR_LEN_MAX+1] = "off";

// For UART Control //

#include <unistd.h>
#include <glib.h>
#include <peripheral_io.h>


bool handle_get_request_on_resource_capability_switch_main_0(st_things_get_request_message_s* req_msg, st_things_representation_s* resp_rep)
{
    if (req_msg->has_property_key(req_msg, PROP_POWER)) {
    	resp_rep->set_str_value(resp_rep, PROP_POWER, g_switch);
    }
    return true;
}

bool handle_set_request_on_resource_capability_switch_main_0(st_things_set_request_message_s* req_msg, st_things_representation_s* resp_rep)
{
	peripheral_uart_h uart_h = NULL;
	int port = 0;

    peripheral_uart_open(port, &uart_h);

    peripheral_uart_set_baud_rate(uart_h, PERIPHERAL_UART_BAUD_RATE_115200);
    peripheral_uart_set_byte_size(uart_h, PERIPHERAL_UART_BYTE_SIZE_8BIT);
    peripheral_uart_set_parity(uart_h, PERIPHERAL_UART_PARITY_NONE);
    peripheral_uart_set_stop_bits(uart_h, PERIPHERAL_UART_STOP_BITS_1BIT);
    peripheral_uart_set_flow_control(uart_h, PERIPHERAL_UART_SOFTWARE_FLOW_CONTROL_NONE, PERIPHERAL_UART_HARDWARE_FLOW_CONTROL_NONE);

    char *str_value = NULL;
	req_msg->rep->get_str_value(req_msg->rep, PROP_POWER, &str_value);

	if ((0 != strncmp(str_value, PROP_SWITCH_ON, strlen(PROP_SWITCH_ON)))
		&& (0 != strncmp(str_value, PROP_SWITCH_OFF, strlen(PROP_SWITCH_OFF)))) {
		ERR("Need On/OFF value");
		free(str_value);

		return false;
	}

    if (0 != strncmp(str_value, g_switch, strlen(g_switch))) {
        strncpy(g_switch, str_value, PROP_STR_LEN_MAX);

        if (0 == strncmp(g_switch, PROP_SWITCH_ON, strlen(PROP_SWITCH_ON))){
        	uint8_t data[] = "POWER:ON";
        	uint32_t length = sizeof(data) / sizeof(data[0]);
        	peripheral_uart_write(uart_h, data, length);

        }
        else{
        	uint8_t data[] = "POWER:OFF";
        	uint32_t length = sizeof(data) / sizeof(data[0]);
        	peripheral_uart_write(uart_h, data, length);
        }
    }

    peripheral_uart_close(uart_h);

    resp_rep->set_str_value(resp_rep, PROP_POWER, g_switch);
	st_things_notify_observers(req_msg->resource_uri);
	free(str_value);

    return true;
}
