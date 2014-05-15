/*******************************************************************************************
 * MQTT starter
 * By Martin Assarsson, ALLEATO
 * 20130110 Initial 
 *******************************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "config.h"
#include "program.h"
#include "misc.h"
#include "mqtt.h"
#include "mqtt_helper.h"
#include "logger.h"

static void mqtt_environment(void) __attribute__ ((constructor (PRIO_ENVCHECK)));
static void mqtt_init(void) __attribute__ ((constructor (PRIO_MQTTSTART)));

static const char* mqtt_host = "localhost";
static int mqtt_port = 1883;

size_t fmt_topic(char* str, const char* name, const char* prefix, const char* fmt, ...)
{
	sprintf(str, "%s/zwave/%s/", prefix, name);
	va_list ap;
	va_start(ap, fmt);
	vsprintf(str+strlen(str), fmt, ap);
	va_end(ap);
	return strlen(str);
}

static void mqtt_environment(void) 
{
	int c;
	static struct option long_options[] = {
		{"host", 0, 0, 'H'},
		{"port", 0, 0, 'p'},
		{0, 0, 0, 0}
	};

	opterr = 0;
	optind = 0;

	while (1)
	{
		int option_index = 0;

		c = getopt_long( __proc_argc, __proc_argv, arguments, long_options, &option_index);

		if (c == -1) break;

		switch (c) {
		case 'p':
			mqtt_port = atoi(optarg);
			break;
		case 'H':
			mqtt_host = optarg;
			break;
		default:
			break;
		}
	}
}

static void mqtt_init(void)
{
	applog(LOG_DEBUG, "starting mqtt subsystem");
	mqtt_addsub("req/obj/zwave/#", 2);
	mqtt_addsub("req/zwave/#", 2);
	mqtt_addsub("set/obj/zwave/#", 2);
	mqtt_addsub("set/zwave/#", 2);
	mqtt_addsub("evt/zwave/#", 2);
	mqtt_start(mqtt_host, mqtt_port);
}
