/*******************************************************************************************
 * Logger 
 * By Martin Assarsson, ALLEATO
 * 20130108 Initial 
 *******************************************************************************************/

#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>

#include "config.h"
#include "logger.h"
#include "program.h"

static void logger_environment(void) __attribute__ ((constructor (PRIO_ENVCHECK)));
static void logger_init(void) __attribute__ ((constructor (PRIO_INIT)));
static void logger_end(void) __attribute__ ((destructor (PRIO_END)));

static int logasdaemon = 0;
static int syslog_init = 0;
static int loglevel = LOG_ERR;

static pthread_mutex_t print_mtx = PTHREAD_MUTEX_INITIALIZER;

/******************************************************************************************
 * 
 *****************************************************************************************/ 
void applog(int priority, const char *format, ...)
{
	static char* priorities[] = { "LOG_EMERG", "LOG_ALERT", "LOG_CRIT", "LOG_ERR", "LOG_WARNING", "LOG_NOTICE", "LOG_INFO", "LOG_DEBUG" };
	char message[512];
	
	if ( priority <= loglevel )
	{
		// ------
		va_list ap;
		va_start(ap, format);
		vsnprintf(message, 512, format, ap);
		va_end(ap);
		message[511] = 0;
		// ------

		if (syslog_init)
		{
			pthread_mutex_lock(&print_mtx);
			syslog ( priority, message );
			pthread_mutex_unlock(&print_mtx);
		} else {
			pthread_mutex_lock(&print_mtx);
			printf ( "%-11s %s\n", priorities[priority], message );
			pthread_mutex_unlock(&print_mtx);
		}
	}
}

void apploghex(int prio, const char* pref, const unsigned char* data, size_t len)
{
	/*
	static char* priorities[] = { "LOG_EMERG", "LOG_ALERT", "LOG_CRIT", "LOG_ERR", "LOG_WARNING", "LOG_NOTICE", "LOG_INFO", "LOG_DEBUG" };
	if(prio <= loglevel) {
		char* msg = malloc(strlen(pref) + (len*3) + 100);
		char* ptr = msg;
		size_t i;
//		va_list ap;
//		va_start(ap, len);
//		ptr += vsprintf(msg, pref, ap);
//		va_end(ap);

		ptr += sprintf(ptr, "%s", pref);
		for(i = 0; i < len-1; i++) {
			ptr += sprintf(ptr, "%.2X:", data[i]);
		}
		sprintf(ptr, "%.2X", data[i]);
		
		if(syslog_init) {
			pthread_mutex_lock(&print_mtx);
			syslog(prio, msg);
			pthread_mutex_unlock(&print_mtx);
		}else {
			pthread_mutex_lock(&print_mtx);
			printf("%-11s %s\n", priorities[prio], msg);
			pthread_mutex_unlock(&print_mtx);
		}
		
		free(msg);
	}
	*/
}

/******************************************************************************************
 * Initializtion and cleanup functions
 *****************************************************************************************/
static void logger_environment(void)
{
//	dprintf(1, "environment - logger\n");
        int c;
        static struct option long_options[] = {
                {"daemon", 0, 0, 'd'},
                {"log", 1, 0, 'l'},
                {0, 0, 0, 0}
        };

        opterr = 0;
        optind = 1;

	while (1)
	{
		int option_index = 0;

		c = getopt_long( __proc_argc, __proc_argv, arguments, long_options, &option_index);

		if (c == -1) break;

		switch (c) {
			case 'd':
				logasdaemon = 1;
				break;
			case 'l':
				if (optarg) {
					char level[4];
					strncpy( level, optarg, 3);
					sscanf ( level, "%i", &loglevel );
					if (loglevel > LOG_DEBUG) loglevel = LOG_DEBUG;
					if (loglevel < LOG_EMERG) loglevel = LOG_EMERG;
				} else {
					printf ( "Please provide level\n" );
					exitcode = -1;
					exit ( exitcode );
				}
				break;
			default:
				break;
		}
	}
}

static void logger_init(void)
{
//	dprintf(1, "init - logger\n");
	if ( logasdaemon )
	{
		// Print pid also as application might be started more than once.
		openlog ( PACKAGE_NAME, LOG_PID, LOG_USER );
		syslog_init = 1;
	}
}

static void logger_end(void)
{
	if ( logasdaemon )
	{
		closelog(); // Not necesary but neat.
	}
}


