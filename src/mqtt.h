#ifndef __MQTT_H__
#define __MQTT_H__
#include <mosquitto.h>

void mqtt_configure(const char* id, int clean_session);
void mqtt_bind(const char* topic, void (*func)(const struct mosquitto_message* msg));
void mqtt_unbind(const char* topic, void (*func)(const struct mosquitto_message* msg));
void mqtt_addsub(const char* sub, int qos);

void mqtt_start(const char* addr, int port);
void mqtt_loop(const char* addr, int port);

int mqtt_is_connected();

void mqtt_publish(const char* topic, int payload_len, const void* payload, int qos, int retain);
void mqtt_pubstr(const char* topic, const char* str, int qos, int retain);
void mqtt_pubjson(const char* topic, const char* fmt, ...);

#endif /* __MQTT_H__ */

