/*******************************************************************************************
 * Program to watch for zwave events
 * By Martin Assarsson, ALLEATO
 * 20121203 Initial 
 *******************************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdarg.h>
#include <malloc.h>
#include <unistd.h>
#include <linux/limits.h>
#include <fcntl.h>

#include "program.h"

int __proc_argc = 0;
char* __proc_argv[40];
char cmdline_buff[ARG_MAX];

void environment_read() __attribute__ ((constructor (PRIO_ENVREAD)));

void environment_read()
{
//	dprintf(1, "envread - environment read\n");
	int cmdline = open("/proc/self/cmdline", O_RDONLY);
	if (cmdline == -1) perror("Opening /proc/self/cmdline\n");
	size_t size = read ( cmdline, cmdline_buff, ARG_MAX );

	size_t bytecnt = 0;
	__proc_argc = 1;

	__proc_argv[0] = cmdline_buff;

	for ( bytecnt = 0; bytecnt < size; bytecnt++ )
	{
		if ( cmdline_buff[bytecnt] == '\0' ) 
		{
			__proc_argv[__proc_argc++] = &cmdline_buff[bytecnt+1];
			if (cmdline_buff[bytecnt +1] == '\0' ) break;
		}
	}
	__proc_argv[--__proc_argc] = NULL;

	close(cmdline);
}
