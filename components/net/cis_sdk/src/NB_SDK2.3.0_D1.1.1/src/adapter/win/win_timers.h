
#ifndef _TIMERS_WIN_H_
#define _TIMERS_WIN_H_




#ifndef ENOTSUP
#define ENOTSUP         129 /* This is the value in VC 2010. */
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT       138 /* This is the value in VC 2010. */
#endif


/**
 * @defgroup clock POSIX Time Routines
 */

#ifndef __clockid_t_defined
typedef int clockid_t;
#define __clockid_t_defined     0x01
#endif  //__clockid_t_defined

#ifndef HAVE_STRUCT_TIMESPEC
#define HAVE_STRUCT_TIMESPEC	0x01
struct timespec {
	long tv_sec;
	long tv_nsec;
};
#endif /* HAVE_STRUCT_TIMESPEC */

/* We have POSIX timers.  */
#ifndef _POSIX_TIMERS
#define _POSIX_TIMERS           200809L
#endif

/* The monotonic clock might be available.  */
#ifndef _POSIX_MONOTONIC_CLOCK
#define _POSIX_MONOTONIC_CLOCK  0x00
#endif

/* The CPU-time clocks are available.  */
#ifndef _POSIX_CPUTIME
#define _POSIX_CPUTIME          200809L
#endif

/* The Clock support in threads are available.  */
#ifndef _POSIX_THREAD_CPUTIME
#define _POSIX_THREAD_CPUTIME   200809L
#endif

#ifndef TIMER_ABSTIME
#define TIMER_ABSTIME				0x01
#endif

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME              0x00
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC             0x01
#endif

#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID    0x02
#endif

#ifndef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID     0x03
#endif

#include <errno.h>
#include <cis_def.h>
//#include <stdint.h>

struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};


int gettimeofday(struct timeval *tv, struct timezone *tz);

#ifdef __cplusplus
extern "C" {
#endif

int clock_getres(clockid_t clock_id, struct timespec *res);
int clock_gettime(clockid_t clock_id, struct timespec *tp);
int clock_settime(clockid_t clock_id, const struct timespec *tp);
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain);
int _nanosleep(const struct timespec *request, struct timespec *remain);

#ifdef __cplusplus
};
#endif

#endif //_TIMERS_WIN_H_