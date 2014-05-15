/*******************************************************************************************
 * Program to watch for and generate mqtt timer events
 * By Martin Assarsson, ALLEATO
 * 20130820 Initial 
 *******************************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdarg.h>
#include <malloc.h>

#include "config.h"
#include "program.h"
#include "misc.h"

int exitcode = 0;
char* arguments = "D:N:H:p:dhvl:";

static void program_environment(void) __attribute__ ((constructor (PRIO_ENVCHECK)));
static void program_end(void) __attribute__ ((destructor (PRIO_END)));

static bool program_help = false;
static bool program_version = false;

static void program_end() 
{
	if ( program_version )
	{
		printf (PACKAGE_NAME " daemon version " PACKAGE_VERSION " built " __DATE__ " " __TIME__ "\n");
	}

	if ( program_help || exitcode )
	{
		printf ("\nUsage: mqtimerd [OPTION]\n\n"
			" Options:\n"
			"  -H, --host           MQTT host name, default localhost\n"
			"  -p, --port           MQTT port number, default 1883\n"
			"  -d, --daemon         Daemonize\n"
			"  -h, --help           Print this help\n"
			"  -v, --version        Print version information\n"
			"  -l, --log=LEVEL      Loglevel wanted (0-7)\n");
	}
}

static void program_environment(void) 
{
	int c;
	static struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"version", 0, 0, 'v'},
		{0, 0, 0, 0}
	};

	opterr = 0;
	optind = 0;

	while (1)
	{
		int option_index = 0;

		c = getopt_long ( __proc_argc, __proc_argv, arguments, long_options, &option_index);

		if (c == -1) break;

		switch (c) {
		case 'h':
			program_version = true;
			program_help = true;
			exit ( 0 );
		case 'v':
			program_version = true;
			exit ( 0 );
		default:
			break;
		}
	}
}

int main(int argc, char **argv, char **envp) 
{
	program_help = false;
	while (1)
	{
		sleep(10);
	}
}
