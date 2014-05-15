#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([ubus-mqtt],[0.1.0],[bug@alleato.se])
AC_CONFIG_SRCDIR([src/ubus-mqtt.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_CXX

# Checks for libraries.
AC_SEARCH_LIBS([pow],[m],,[AC_MSG_ERROR(["Missing libm library"])])
AC_SEARCH_LIBS([RSA_sign],[crypto],,[AC_MSG_ERROR(["Missing crypto library"])])
AC_SEARCH_LIBS([SSL_get_session],[ssl],,[AC_MSG_ERROR(["Missing ssl library"])])
AC_SEARCH_LIBS([pthread_create],[pthread],,[AC_MSG_ERROR(["Missing pthread library"])])
AC_SEARCH_LIBS([mosquitto_connect],[mosquitto],,[AC_MSG_ERROR(["Missing mosquitto development library"])])
AC_SEARCH_LIBS([json_pack],[jansson],,[AC_MSG_ERROR(["Missing jansson library"])])
AC_SEARCH_LIBS([ubus_connect],[ubus],,[AC_MSG_ERROR(["Missing ubus library"])])
AC_SEARCH_LIBS([uloop_init],[ubox],,[AC_MSG_ERROR(["Missing ubox library"])])
AC_SEARCH_LIBS([blobmsg_add_object],[blobmsg_json],,[AC_MSG_ERROR(["Missing blobmsg_json library"])])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h malloc.h stdint.h stdlib.h string.h sys/ioctl.h syslog.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([bzero strdup strerror])

AC_CONFIG_FILES([Makefile
		 src/Makefile])
AC_OUTPUT
AC_MSG_RESULT([
	$PACKAGE $VERSION
	===============

	prefix:                 ${prefix}
	exec_prefix:            ${exec_prefix}
	sysconfdir:             ${sysconfdir}
	libdir:                 ${libdir}
	includedir:             ${includedir}
 
	compiler:               ${CC}
	cflags:                 ${CFLAGS}
	ldflags:                ${LDFLAGS}
])
