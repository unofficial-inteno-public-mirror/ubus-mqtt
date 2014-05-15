#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <syslog.h>

void applog(int priority, const char *format, ...);
void apploghex(int prio, const char* pref, const unsigned char* data, size_t len);

#endif /* __LOGGER_H__ */
