/********************************************************
 * Filename: mqtt_msg_dispatcher.h
 * Author: daedalus
 * Email: 
 * Description: 
 *
 *******************************************************/

#ifndef __ZWAVE_HANDLER_APPLICATION_COMMAND_H__
#define __ZWAVE_HANDLER_APPLICATION_COMMAND_H__

#include <mosquitto.h>
#include <stdint.h>

//typedef void (*mqtt_cmd_fn)(uint8_t node, uint8_t endp, const char* topic, json_t* obj);
typedef void (*mqtt_msg_fn)(size_t topic_cnt, const char** topic, json_t* obj);

void register_command_class(const char* name);
void register_mqtt_set(const char* cls, const char* cmd, mqtt_msg_fn function);
void register_mqtt_event(const char* cls, const char* cmd, mqtt_msg_fn function);
void register_mqtt_req(const char* cls, const char* cmd, mqtt_msg_fn function);

#endif /* __ZWAVE_HANDLER_APPLICATION_COMMAND_H__ */

