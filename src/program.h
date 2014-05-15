/*********************************************************************************
 *
 *
 ********************************************************************************/

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

extern int exitcode;

extern char* arguments;

extern int __proc_argc;
extern char* __proc_argv[40];

// Priority constants for constructors
#define PRIO_ENVREAD 200
#define PRIO_ENVCHECK 1000
#define PRIO_DAEMONIZE 2000
#define PRIO_FORK 3000
#define PRIO_MQTTINIT 3500
#define PRIO_MQTTRESTART 3600
#define PRIO_MQTTSTART 4000
#define PRIO_INIT_DATA1 4500
#define PRIO_INIT_DATA 5000
#define PRIO_INIT 6000
#define PRIO_THREADING 7000

// Priority constants for destructors
#define PRIO_END 1000

#endif /* __PROGRAM_H__ */

