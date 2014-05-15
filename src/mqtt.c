#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mosquitto.h>
#include <jansson.h>
#include "list.h"
#include "misc.h"
#include "mqtt.h"
#include "program.h"

static void mosq_init(void) __attribute__ ((constructor (PRIO_MQTTINIT)));
static void mosq_cleanup(void) __attribute__ ((destructor (PRIO_MQTTINIT)));

struct subscription {
	list_head_t head;
	char* sub;
	int qos;
};

struct bind {
	list_head_t head;
	char* topic;
	void (*func)(const struct mosquitto_message* msg);
};

static struct mosquitto* _mosq;
static int _is_connected;
static char* _addr;
static int _port;
static int _mid;

static list_t _subs;
static list_t _binds;

static int mqtt_match(const char* sub, const char* topic);
static void on_message(struct mosquitto* mq, void* obj, const struct mosquitto_message* msg);
static void on_disconnect(struct mosquitto* mq, void* obj, int rc);
static void on_connect(struct mosquitto* mq, void* obj, int rc);
static void on_log(struct mosquitto* mq, void* obj, int lvl, const char* msg);

static int local_mqtt_match(const char* sub, const char* topic);

void mqtt_configure(const char* id, int clean_session)
{
/*	if(_mosq == NULL){
		_mosq = mosquitto_new(id, clean_session, NULL);
		list_init(&_subs);
		list_init(&_binds);
	}
	else*/
	mosquitto_reinitialise(_mosq, id, clean_session, NULL);
	mosquitto_connect_callback_set( _mosq, on_connect  );
	mosquitto_disconnect_callback_set( _mosq, on_disconnect );
	mosquitto_message_callback_set( _mosq, on_message);
	mosquitto_log_callback_set( _mosq, on_log );
}

void mqtt_bind(const char* topic, void (*func)(const struct mosquitto_message* msg))
{
	struct bind* b = (struct bind*) malloc(sizeof(struct bind));
	b->topic = strdup(topic);
	b->func = func;
	list_pushb(&_binds, &b->head);
}

void mqtt_unbind(const char* topic, void (*func)(const struct mosquitto_message* msg))
{
	struct bind* b;
	list_for_each(b, &_binds, head) {
		if(b->func == func)
			if(!strcmp(topic, b->topic))
				break;
	}
	list_remh(&_subs, &b->head);
	free(b->topic);
	free(b);
}

void mqtt_addsub(const char* sub, int qos)
{
	struct subscription* _sub = (struct subscription*)malloc(sizeof(struct subscription));
	_sub->sub = strdup(sub);
	_sub->qos = qos;
	list_pushb(&_subs, &_sub->head);
	if(_is_connected) {
		mosquitto_subscribe(_mosq, &_mid, sub, qos);
//		dprintf("mqtt: subscribed to %s\n", sub);
	}
}

void mqtt_start(const char* addr, int port)
{
	_addr = strdup(addr);
	_port = port;
	mosquitto_connect(_mosq, addr, port, 90);
	mosquitto_loop_start(_mosq);
}

void mqtt_loop(const char* addr, int port)
{
	_addr = strdup(addr);
	_port = port;
	mosquitto_connect(_mosq, addr, port, 90);
	int err = 0;
	while( (err = mosquitto_loop(_mosq, -1, 1)) == 0 ) {
	}
//	dprintf("mqtt: loop error\n");
}

void mqtt_publish(const char* topic, int payload_len, const void* payload, int qos, int retain)
{
	mosquitto_publish(_mosq, &_mid, topic, payload_len, payload, qos, retain);
}

void mqtt_pubstr(const char* topic, const char* str, int qos, int retain)
{
	mosquitto_publish(_mosq, &_mid, topic, strlen(str), str, qos, retain);
}

void mqtt_pubjson(const char* topic, const char* fmt, ...)
{
	json_t* obj;
	char* payload;
	
	va_list ap;
	va_start(ap, fmt);
	obj = json_vpack_ex(NULL, 0, fmt, ap);
	va_end(ap);
	
	payload = json_dumps(obj, 0);
	json_decref(obj);
	mqtt_pubstr(topic, payload, 2, 0);
	free(payload);
	return;
}

int mqtt_is_connected()
{
	return _is_connected;
}

static int mqtt_match(const char* sub, const char* topic)
{
	bool res;
	int err = mosquitto_topic_matches_sub(sub, topic, &res);
//	dprintf("mqtt_match(%s, %s) -> %d\n", sub, topic, res);
	return res;
}

static void on_message(struct mosquitto* mq, void* obj, const struct mosquitto_message* msg)
{
	struct bind* b;
	list_for_each(b, &_binds, head) {
//		printf("matching %s with %s\n", b->topic, msg->topic);
		if(local_mqtt_match(b->topic, msg->topic))
			b->func(msg);
	}
}

static void on_log(struct mosquitto* mq, void* obj, int lvl, const char* msg)
{
}

static void on_disconnect(struct mosquitto* mq, void* what, int rc)
{
//	printf("mqtt: disconnected from %s\n", _addr);
	_is_connected = 0;
	switch(rc) {
		default:
			break;
	}
}

static void on_connect(struct mosquitto* mq, void* what, int rc)
{
	switch(rc) {
		case 0: 
			{
//				dprintf("mqtt: connected to %s\n", _addr);
				struct subscription* sub;
				_is_connected = 1;
				list_for_each(sub, &_subs, head) {
					mosquitto_subscribe(_mosq, &_mid, sub->sub, sub->qos);
//					dprintf("mqtt: subscribed to %s\n", sub->sub);
				}
			}
			break;
		default: 
			{
//				dprintf("error connecting to %s\n", _addr);
			}
			break;
	}
}

static void mosq_init()
{
	dprintf(1, "mqttstart - mosq_init.start\n");
	mosquitto_lib_init();
	list_init(&_subs);
	list_init(&_binds);
	_mosq = mosquitto_new(__proc_argv[0], 1, NULL);
	if(!_mosq) {
		printf("error initializing mqtt");
	}
	mosquitto_connect_callback_set( _mosq, on_connect  );
	mosquitto_disconnect_callback_set( _mosq, on_disconnect );
	mosquitto_message_callback_set( _mosq, on_message );
	mosquitto_log_callback_set( _mosq, on_log );
	dprintf(1, "mqttstart - mosq_init.end\n");
}

static void mosq_cleanup()
{
	mosquitto_destroy(_mosq);
	mosquitto_lib_cleanup();
}

/****************************************************************
 *
 ***************************************************************/

/*
void test(const struct mosquitto_message* msg)
{
	printf("HELLO from %s\n", msg->topic);
}

int main(int argc, char** argv)
{
	mqtt_bind("hello/world", test);
	mqtt_bind("hello/sweden", test);
	mqtt_addsub("hello/+", 2);
	mqtt_start("localhost", 1883);
	while(1) {
		sleep(1);
		mqtt_pubstr("omfg", "lol", 2, 0);
	}
}
*/

static int _match_subtopic(const char* sub, const char* top)
{
	if(sub && strcmp(sub, "+") == 0)
		return 1;
	if(sub && strcmp(sub, "#") == 0)
		return 2;
	if(sub && top && strcmp(sub, top) == 0)
		return 3;
	if(!sub && !top)
		return 4;
	return 0;
}

static int local_mqtt_match(const char* sub, const char* topic)
{
/*	
	char* _sub, *_topic;
 	_sub = strdup(sub);
	_topic = strdup(topic);
*/
	char* sub_data;
	size_t sub_ntok;
	char* topic_data;
	size_t topic_ntok;
	
	char** sub_tokens = strsplit(sub, "/", &sub_data, &sub_ntok);
	char** topic_tokens = strsplit(topic, "/", &topic_data, &topic_ntok);
	
	char** tmp_sub_tokens = sub_tokens;
	char** tmp_topic_tokens = topic_tokens;

	for(; 1; sub_tokens++, topic_tokens++){
		switch(_match_subtopic(*sub_tokens, *topic_tokens)) {
			case 0:
//				printf("failed: %s != %s\n", *sub_tokens, *topic_tokens);
				free(tmp_sub_tokens);
				free(tmp_topic_tokens);
				free(sub_data);
				free(topic_data);
				return 0;
				break;
			case 1:
//				printf("wildcard: + == %s\n", *topic_tokens);
				break;
			case 2:
//				printf("wildcard #\n");
				free(tmp_sub_tokens);
				free(tmp_topic_tokens);
				free(sub_data);
				free(topic_data);
				return 1;
				break;
			case 3:
//				printf("%s == %s\n", *sub_tokens, *topic_tokens);
				break;
			case 4:
//				printf("success!\n");
				free(tmp_sub_tokens);
				free(tmp_topic_tokens);
				free(sub_data);
				free(topic_data);
				return 1;
		}
	}
	free(tmp_sub_tokens);
	free(tmp_topic_tokens);
	free(sub_data);
	free(topic_data);

}
