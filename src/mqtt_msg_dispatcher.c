/********************************************************
 * Filename: zwave_handlers.c
 * Author: daedalus
 * Email: 
 * Description: 
 *
 * Modified: 20130626 Martin Assarsson, ALLEATO
 *           Rewritten to handle commands. 
 *           Can now be recursively called.
 *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <mosquitto.h>
#include <stdarg.h>
#include <jansson.h>

#include "program.h"

#include "mqtt_helper.h"
#include "mqtt.h"
#include "misc.h"
#include "logger.h"
#include "mqtt_msg_dispatcher.h"

#define CMDCLS_LIST_SIZE 30
#define MQTT_CMD_LIST_SIZE 20

static void bind_mqtt() __attribute__ ((constructor (PRIO_MQTTSTART)));

typedef struct {
	const char* cmd;
	mqtt_msg_fn fn;
} mqtt_fn_entry;

typedef struct {
	const char* cls_name;
	mqtt_fn_entry mqtt_evts[MQTT_CMD_LIST_SIZE];
	uint8_t mqtt_evt_cnt;
	mqtt_fn_entry mqtt_sets[MQTT_CMD_LIST_SIZE];
	uint8_t mqtt_set_cnt;
	mqtt_fn_entry mqtt_reqs[MQTT_CMD_LIST_SIZE];
	uint8_t mqtt_req_cnt;
} cls_entry;

static cls_entry cmdcls_list[CMDCLS_LIST_SIZE];
uint8_t cmdcls_cnt = 0; // This number is "used".

void register_command_class(const char* cls_name)
{
	applog(LOG_DEBUG, "Registering command class %s", cls_name);
	if (cmdcls_cnt >= CMDCLS_LIST_SIZE)
	{
		applog(LOG_ERR, "Command list too short, modify CMDCLS_LIST_SIZE");
		return;
	}

	uint8_t icls = cmdcls_cnt;
	cmdcls_cnt++;

	cmdcls_list[icls].cls_name = cls_name;
	cmdcls_list[icls].mqtt_evt_cnt = 0;
	cmdcls_list[icls].mqtt_set_cnt = 0;
	cmdcls_list[icls].mqtt_req_cnt = 0;
}

void register_mqtt_set(const char* cls, const char* cmd, mqtt_msg_fn function)
{
	applog(LOG_DEBUG, "Registering mqtt set %s in class %s", cmd, cls);

	uint8_t icls = 0;
	for (icls = 0; icls < cmdcls_cnt; icls++) {
		if (strcmp(cmdcls_list[icls].cls_name, cls) == 0) {
			break;
		}
	}
	if (icls == cmdcls_cnt) {
		applog(LOG_ERR, "Command class %s not registered", cls);
		return;
	}
	cls_entry *pcls = &(cmdcls_list[icls]);

	uint8_t icmd = 0;
	for (icmd = 0; icmd < pcls->mqtt_set_cnt; icmd++) {
		if (strcmp(pcls->mqtt_sets[icmd].cmd, cmd) == 0) {
			break;
		}
	}
	if (icmd < pcls->mqtt_set_cnt) {
		applog(LOG_ERR, "Set command %s/%s allready registered", cls, cmd);
		return;
	}

	if (icmd >= MQTT_CMD_LIST_SIZE)
	{
		applog(LOG_ERR, "Command list too short, modify MQTT_CMD_LIST_SIZE");
		return;
	}
	pcls->mqtt_set_cnt++;

	pcls->mqtt_sets[icmd].cmd = cmd;
	pcls->mqtt_sets[icmd].fn = function;
}

void register_mqtt_event(const char* cls, const char* cmd, mqtt_msg_fn function)
{
	applog(LOG_DEBUG, "Registering mqtt event %s in class %s", cmd, cls);

	uint8_t icls = 0;
	for (icls = 0; icls < cmdcls_cnt; icls++) {
		if (strcmp(cmdcls_list[icls].cls_name, cls) == 0) {
			break;
		}
	}
	if (icls == cmdcls_cnt) {
		applog(LOG_ERR, "Command class %s not registered", cls);
		return;
	}
	cls_entry *pcls = &(cmdcls_list[icls]);

	uint8_t icmd = 0;
	for (icmd = 0; icmd < pcls->mqtt_evt_cnt; icmd++) {
		if (strcmp(pcls->mqtt_evts[icmd].cmd, cmd) == 0) {
			break;
		}
	}
	if (icmd < pcls->mqtt_evt_cnt) {
		applog(LOG_ERR, "Event command %s/%s allready registered", cls, cmd);
		return;
	}

	if (icmd >= MQTT_CMD_LIST_SIZE)
	{
		applog(LOG_ERR, "Command list too short, modify MQTT_CMD_LIST_SIZE");
		return;
	}
	pcls->mqtt_evt_cnt++;

	pcls->mqtt_evts[icmd].cmd = cmd;
	pcls->mqtt_evts[icmd].fn = function;
}

void register_mqtt_req(const char* cls, const char* cmd, mqtt_msg_fn function)
{
	applog(LOG_DEBUG, "Registering mqtt get %s in class %s", cmd, cls);

	uint8_t icls = 0;
	for (icls = 0; icls < cmdcls_cnt; icls++) {
		if (strcmp(cmdcls_list[icls].cls_name, cls) == 0) {
			break;
		}
	}
	if (icls == cmdcls_cnt) {
		applog(LOG_ERR, "Command class %s not registered", cls);
		return;
	}
	cls_entry *pcls = &(cmdcls_list[icls]);

	uint8_t icmd = 0;
	for (icmd = 0; icmd < pcls->mqtt_req_cnt; icmd++) {
		if (strcmp(pcls->mqtt_reqs[icmd].cmd, cmd) == 0) {
			break;
		}
	}
	if (icmd < pcls->mqtt_req_cnt) {
		applog(LOG_ERR, "Get command %s/%s allready registered", cls, cmd);
		return;
	}

	if (icmd >= MQTT_CMD_LIST_SIZE)
	{
		applog(LOG_ERR, "Command list too short, modify MQTT_CMD_LIST_SIZE");
		return;
	}
	pcls->mqtt_req_cnt++;

	pcls->mqtt_reqs[icmd].cmd = cmd;
	pcls->mqtt_reqs[icmd].fn = function;
}

cls_entry* find_cls_by_name(const char* cls_name)
{
	uint8_t cls_iter = 0;

	for (cls_iter = 0; cls_iter < cmdcls_cnt; cls_iter++)
	{
		if (!strcmp(cmdcls_list[cls_iter].cls_name, cls_name)) return &(cmdcls_list[cls_iter]);
	}
	return NULL;
}

const mqtt_fn_entry* find_mqtt_evt_by_name(const cls_entry* pcls, const char* cmd_name)
{
	uint8_t cmd_iter = 0;

	if (!pcls->mqtt_evt_cnt) return NULL;
	for (cmd_iter = 0; cmd_iter < pcls->mqtt_evt_cnt; cmd_iter++)
	{
		if (pcls->mqtt_evts[cmd_iter].cmd == NULL & cmd_name == NULL) return &(pcls->mqtt_evts[cmd_iter]);
		if (!strcmp(pcls->mqtt_evts[cmd_iter].cmd, cmd_name)) return &(pcls->mqtt_evts[cmd_iter]);
	}
	return NULL;
}

const mqtt_fn_entry* find_mqtt_req_by_name(const cls_entry* pcls, const char* cmd_name)
{
	uint8_t cmd_iter = 0;

	if (!pcls->mqtt_req_cnt) return NULL;
	for (cmd_iter = 0; cmd_iter < pcls->mqtt_req_cnt; cmd_iter++)
	{
		if (pcls->mqtt_reqs[cmd_iter].cmd == NULL & cmd_name == NULL) return &(pcls->mqtt_reqs[cmd_iter]);
		if (!strcmp(pcls->mqtt_reqs[cmd_iter].cmd, cmd_name)) return &(pcls->mqtt_reqs[cmd_iter]);
	}
	return NULL;
}

const mqtt_fn_entry* find_mqtt_set_by_name(const cls_entry* pcls, const char* cmd_name)
{
	uint8_t cmd_iter = 0;

	if (!pcls->mqtt_set_cnt) return NULL;
	for (cmd_iter = 0; cmd_iter < pcls->mqtt_set_cnt; cmd_iter++)
	{
		if (pcls->mqtt_sets[cmd_iter].cmd == NULL & cmd_name == NULL) return &(pcls->mqtt_sets[cmd_iter]);
		if (!strcmp(pcls->mqtt_sets[cmd_iter].cmd, cmd_name)) return &(pcls->mqtt_sets[cmd_iter]);
	}
	return NULL;
}

static void mqtt_event_handler(const struct mosquitto_message* msg)
{
	json_t* msg_obj = json_loads(msg->payload, 0, NULL);
	if(!msg_obj) {
		applog(LOG_INFO, "%s: %s",__func__, "invalid json object");
		return;
	}

	char* tmp;
	size_t cnt;
	char** topic = strsplit(msg->topic, "/", &tmp, &cnt);

	cls_entry* pcls = find_cls_by_name(topic[5]);
	if (pcls == NULL)
	{
		applog(LOG_INFO, "MQTT command class %s not registered", topic[5]);
	} else {
		const mqtt_fn_entry* pcmd = find_mqtt_evt_by_name(pcls, topic[6]);
		if (pcmd == NULL) {
			applog(LOG_INFO, "MQTT evt command %s not registered", topic[6]);
		} else {
			if (pcmd->fn != NULL) {
				applog(LOG_DEBUG, "Running MQTT evt command %s/%s", topic[5], topic[6]);
				pcmd->fn(cnt, (const char**)topic, msg_obj);
			} else {
				applog(LOG_INFO, "MQTT evt command %s/%s not implemented", topic[5], topic[6]);
			}
		}
	}
	free(tmp);
	free(topic);
	json_decref(msg_obj);
}

static void mqtt_set_handler(const struct mosquitto_message* msg)
{
	json_t* msg_obj = json_loads(msg->payload, 0, NULL);
	if(!msg_obj) {
		applog(LOG_INFO, "%s: %s",__func__, "invalid json object");
		return;
	}

	char* tmp;
	size_t cnt;
	char** topic = strsplit(msg->topic, "/", &tmp, &cnt);

	cls_entry* pcls = find_cls_by_name(topic[5]);
	if (pcls == NULL)
	{
		applog(LOG_INFO, "MQTT command class %s not registered", topic[5]);
	} else {
		const mqtt_fn_entry* pcmd = find_mqtt_set_by_name(pcls, topic[6]);
		if (pcmd == NULL) {
			applog(LOG_INFO, "MQTT set command %s not registered", topic[6]);
		} else {
			if (pcmd->fn != NULL) {
				applog(LOG_DEBUG, "Running MQTT set command %s/%s", topic[5], topic[6]);
				pcmd->fn(cnt, (const char**)topic, msg_obj);
			} else {
				applog(LOG_INFO, "MQTT set command %s/%s not implemented", topic[5], topic[6]);
			}
		}
	}
	free(tmp);
	free(topic);
	json_decref(msg_obj);
}

static void mqtt_request_handler(const struct mosquitto_message* msg)
{
	json_t* msg_obj = json_loads(msg->payload, 0, NULL);
	if(!msg_obj) {
		applog(LOG_INFO, "%s: %s",__func__, "invalid json object");
		return;
	}

	char* tmp;
	size_t cnt;
	char** topic = strsplit(msg->topic, "/", &tmp, &cnt);

	cls_entry* pcls = find_cls_by_name(topic[5]);
	if (pcls == NULL)
	{
		applog(LOG_INFO, "MQTT command class %s not registered", topic[5]);
	} else {
		const mqtt_fn_entry* pcmd = find_mqtt_req_by_name(pcls, topic[6]);
		if (pcmd == NULL) {
			applog(LOG_INFO, "MQTT req command %s not registered", topic[6]);
		} else {
			if (pcmd->fn != NULL) {
				applog(LOG_DEBUG, "Running MQTT req command %s/%s", topic[5], topic[6]);
				pcmd->fn(cnt, (const char**)topic, msg_obj);
			} else {
				applog(LOG_INFO, "MQTT req command %s/%s not implemented", topic[5], topic[6]);
			}
		}
	}
	free(tmp);
	free(topic);
	json_decref(msg_obj);
}

static void bind_mqtt()
{
	applog(LOG_DEBUG, "Register listeners");
	mqtt_bind("evt/zwave/+/node/+/#", mqtt_event_handler); // Will handle events from nodes
	mqtt_bind("set/obj/zwave/+/node/+/#", mqtt_set_handler); // Handles set commands for classes and possible subclasses
	mqtt_bind("req/zwave/+/node/+/+", mqtt_request_handler); // Handles requests for classes and implicitly all subclasses
}
