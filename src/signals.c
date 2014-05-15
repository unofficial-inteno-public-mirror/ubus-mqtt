/*************************************************************
 * Signals section available to any program
 * By Martin Assarsson, ALLEATO
 * 20121204 Initial
 * ***********************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "program.h"

static void signals_init(void) __attribute__ ((constructor (PRIO_INIT)));

static struct sigaction action;

static void sighup_handler(int signum)
{
	switch (signum)
	{
	case SIGINT:
	case SIGHUP:
	case SIGTERM:
//	case SIGCHLD:
		exit(0);
	default:
		break;
	}
}

static void signals_init(void)
{
	action.sa_handler = sighup_handler;
	sigemptyset (&action.sa_mask);
	action.sa_flags = 0;

	sigaction (SIGINT, &action, NULL);
	sigaction (SIGHUP, &action, NULL);
	sigaction (SIGTERM, &action, NULL);
//	sigaction (SIGCHLD, &action, NULL);
}
