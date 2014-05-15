#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <mosquitto.h>
//#include <jansson.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include "misc.h"
#include "program.h"

static void init(void) __attribute__ ((constructor (PRIO_INIT)));
static void cleanup(void) __attribute__ ((destructor (PRIO_INIT)));

static void receive_event(struct ubus_context *ctx, struct ubus_event_handler *ev,
		              const char *type, struct blob_attr *msg)
{
	char *str;
	char topic[256];

	sprintf(topic, "evt/ubus/%s",type);
	str = blobmsg_format_json(msg, true);
	mqtt_pubstr(topic, str, 2, 0);
	free(str);
}

static pthread_t ubus_pth = 0;

static void* ubus_main(void* arg)
{
	dprintf(1, "ubus - main.start\n");

	static struct ubus_context *ctx;
	static struct ubus_event_handler listener;
	const char *event;
	int ret = 0;

	ctx = ubus_connect(NULL);

	memset(&listener, 0, sizeof(listener));
	listener.cb = receive_event;

	ret = ubus_register_event_handler(ctx, &listener, "*");

	uloop_init();
	ubus_add_uloop(ctx);
	uloop_run();
	uloop_done();

	dprintf(1, "ubus - main.end\n");
}

static void init()
{
	dprintf(1, "ubus - init.start\n");
	pthread_create ( &ubus_pth, NULL, &ubus_main, NULL );
	dprintf(1, "ubus - init.end\n");
}

static void cleanup()
{
}
