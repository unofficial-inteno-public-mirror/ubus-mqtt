/********************************************************
 * Filename: zwave_appcmd_configuration.c
 * Author: martin
 * Email: 
 * Description: 
 *
 *******************************************************/
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <endian.h>

#include <mosquitto.h>
#include <jansson.h>

#include "program.h"

#include "ZW_SerialAPI.h"
#include "ZW_basis_api.h"
#include "ZW_controller_api.h"
#include "ZW_classcmd.h"

#include "zwave_cmds.h"
#include "mqtt_helper.h"
#include "zwave_msg.h"
#include "zwave_handlers.h"
#include "zwave_msg_handler.h"
#include "mqtt.h"
#include "misc.h"
#include "logger.h"

#include "zwave_handler_application_command.h"

static void init_handlers() __attribute__ ((constructor (PRIO_INIT_DATA)));

typedef struct report {
	uint8_t parameter;
	uint8_t def_size;
	union {
		uint8_t val8;
		uint16_t val16;
		uint32_t val32;
	} value __attribute__((packed));
} report_t;

static void on_zw_report(const char* topic, uint8_t node, uint8_t endp, size_t len, const uint8_t* msg)
{
	report_t* rep = (report_t*)msg;
	uint32_t value;
	switch ( rep->def_size ) {
		case 1: value = rep->value.val8; break;
		case 2: value = ntohs(rep->value.val16); break;
		case 4: value = ntohl(rep->value.val32); break;
		default: value = INT_MAX;
	}
	json_t* obj = json_pack( "{s:i, s:i, s:i}",
				"parameter", rep->parameter,
				"size", (rep->def_size & 0x07),
				"value", value );
	if(obj){
		char* dump = json_dumps(obj, 0);
		mqtt_pubstr(topic, dump, 2, 0);
		free(dump);
		//mqtt_pubjson(topic, "o", obj);
		json_decref(obj);
	}
}

static void on_zw_bulk_report(const char* topic, uint8_t node, uint8_t endp, size_t len, const uint8_t* msg)
{
	applog(LOG_ERR, "on_bulk_report not implemented in configuration");

	// ToDo: Add code for "on_bulk_report"
}

static void on_mqtt_get(uint8_t node, uint8_t endp, const char* topic, json_t* obj)
{
	int parameter;
	if(json_unpack(obj, "{s:i}", "parameter", &parameter) == -1) {
		applog(LOG_ERR, "configuration/get missing \"parameter\" parameter");
		return;
	}
	uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_GET, (uint8_t)(parameter & 0xFF) };
	func_id_zw_send_data(node, 3, data, ACK_AUTO, 1);
}

static void on_mqtt_set(uint8_t node, uint8_t endp, const char* topic, json_t* obj)
{
	int parameter;
	int size = -1;
	int value = INT_MAX;
	int def = -1;
	if(json_unpack(obj, "{s:i,s?b,s?i,s?i}", "parameter", &parameter, "default", &def, "size", &size, "value", &value) == -1) {
		applog(LOG_ERR, "configuration/set missing \"parameter\" parameter");
		return;
	}
	if ( def != -1 ) {
		uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_SET, (uint8_t)(parameter &0xFF), 0x81, 0x00 };
		func_id_zw_send_data(node, 5, data, ACK_AUTO, 1);
	} else if ( value != INT_MAX && size == 1 ) {
		int8_t myvalue = value;
		uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_SET, (uint8_t)(parameter &0xFF), 0x01, (uint8_t)(myvalue & 0xFF) };
		func_id_zw_send_data(node, 5, data, ACK_AUTO, 1);
	} else if ( value != INT_MAX && size == 2 ) {
		struct {
			int16_t value;
			uint8_t byte[2];
		} arr;
		arr.value = htons(value);
		uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_SET, (uint8_t)(parameter &0xFF), 0x02, arr.byte[0], arr.byte[1] };
		func_id_zw_send_data(node, 6, data, ACK_AUTO, 1);
	} else if ( value != INT_MAX && size == 4 ) {
		struct {
			int32_t value;
			uint8_t byte[4];
		} arr;
		arr.value = htonl(value);
		uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_SET, (uint8_t)(parameter &0xFF), 0x04, arr.byte[0], arr.byte[1], arr.byte[2], arr.byte[3] };
		func_id_zw_send_data(node, 8, data, ACK_AUTO, 1);
	} else {
		applog(LOG_ERR, "configuration/set missing \"default\" parameter or \"value\" and \"size\" parameters");
	}
}

static void init_handlers()
{
	register_command(COMMAND_CLASS_CONFIGURATION, "configuration");
	register_mqtt_command(COMMAND_CLASS_CONFIGURATION, "get", on_mqtt_get);
	register_mqtt_command(COMMAND_CLASS_CONFIGURATION, "set", on_mqtt_set);
	register_zw_command(COMMAND_CLASS_CONFIGURATION, "report", CONFIGURATION_REPORT, on_zw_report);
	register_zw_command(COMMAND_CLASS_CONFIGURATION, "bulk_report", CONFIGURATION_BULK_REPORT_V2, /*on_zw_bulk_report*/ NULL);
}
