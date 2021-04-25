#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <lwip/opt.h>
#include <lwip/debug.h>
#include <lwip/def.h> /* provide ntohs, htons */

#define BSD 199103

#define WITH_POSIX 1

#define PACKAGE_NAME "libcoap"
#define PACKAGE_VERSION "master-b425b15-dtls-25863042"
#define PACKAGE_STRING PACKAGE_NAME PACKAGE_VERSION

/* it's just provided by libc. i hope we don't get too many of those, as
 * actually we'd need autotools again to find out what environment we're
 * building in */
#define HAVE_STRNLEN 1

#define HAVE_MALLOC 1

#define HAVE_LIMITS_H
#define HAVE_NETINET_IN_H
#define HAVE_SYS_SOCKET_H
#define WITH_MBEDDTLS
#define HAVE_ASSERT_H
#define HAVE_UNISTD_H

#define COAP_RESOURCES_NOHASH
#define _POSIX_TIMERS 0
#define HAVE_SYS_UNISTD_H 0

#define COAP_NO_TRACE
#endif /* _CONFIG_H_ */
