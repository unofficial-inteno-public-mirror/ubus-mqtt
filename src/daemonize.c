/*************************************************************
 * Daemonizer section available to any program
 * By Martin Assarsson, ALLEATO
 * 20121203 Initial
 * ***********************************************************/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <getopt.h>

#include "program.h"

static void daemon_environment(void) __attribute__ ((constructor (PRIO_ENVCHECK)));
static void daemon_start(void) __attribute__ ((constructor (PRIO_DAEMONIZE)));

static int daemonize = 0;

static void daemon_environment(void)
{
	int c;
//	dprintf(1, "envcheck - daemonize\n");
	static struct option long_options[] = {
		{"daemon", no_argument, 0, 'd'},
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
		case 'd':
			daemonize = true;
			printf("Daemonizing...\n");
			break;
		default:
			break;
		}
	}
}

static void daemon_start(void)
{
	int ppid;
	int tpid;
	ppid = getppid();
	tpid = getpid();

//	dprintf(1, "daemonize - daemonize\n");
//	dprintf(1, "Parent pid %i, my pid %i \n", ppid, tpid);

	if (daemonize)
	{
		if ( ppid == 1 )
		{
			exit ( 0 ); // Allready forked
		}

		// fork process
		pid_t pid = fork();

		if ( pid < 0 ) // Failed to fork
		{
			exit ( -1 );
		}		          
		if ( pid > 0 ) // This is the crating parent
		{
			exit ( 0 ); // Fine, let child run!
		}

		pid = getpid();
//		printf("This is child pid %i\n", pid);

		// Get new process group
		setsid();

		// Close stdin. stdout and stderr and redirect them to /dev/null
		int i;
//		for ( i = getdtablesize(); i >= 0; i--)
//			close(i);
		i = open("/dev/null", O_RDWR);
		dup2(i,0);
		dup2(i,1);
		dup2(i,2);
		close(i);

		// Set file permissions
		umask(027);
	}
}

