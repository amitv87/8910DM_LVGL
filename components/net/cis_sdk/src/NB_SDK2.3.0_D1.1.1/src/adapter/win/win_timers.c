/*
 * Copyright (c) 2011, Dongsheng Song <songdongsheng@live.cn>
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file clock.c
 * @brief Implementation Code of POSIX Time Routines
 */
#include "win_timers.h"
#include <stdio.h>
#include <errno.h>
#include <winsock2.h>
#include <time.h>


#define INT64_C(x)		((x) + (INT64_MAX - INT64_MAX))
#define UINT64_C(x)		((x) + (UINT64_MAX - UINT64_MAX))


#define  DELTA_EPOCH_IN_100NS   INT64_C(116444736000000000) 
#define  MAX_SLEEP_IN_MS   4294967294UL 
#define  POW10_2   INT64_C(100) 
#define  POW10_3   INT64_C(1000) 
#define  POW10_4   INT64_C(10000) 
#define  POW10_6   INT64_C(1000000) 
#define  POW10_7   INT64_C(10000000) 
#define  POW10_9   INT64_C(1000000000) 



#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#	define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#	define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (tv) {
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres /= 10;  /*convert into microseconds*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS; 
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}
	if (tz) {
		if (!tzflag) {
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}

static int lc_set_errno(int result)
{
	if (result != 0) {
		errno = result;
		return -1;
	}
	return 0;
};


/**
 * Get the resolution of the specified clock clock_id and
 * stores it in the struct struct timespec pointed to by res.
 * @param  clock_id The clock_id argument is the identifier of the particular
 *         clock on which to act. The following clocks are supported:
 * <pre>
 *     CLOCK_REALTIME  System-wide real-time clock. Setting this clock
 *                 requires appropriate privileges.
 *     CLOCK_MONOTONIC Clock that cannot be set and represents monotonic
 *                 time since some unspecified starting point.
 *     CLOCK_PROCESS_CPUTIME_ID High-resolution per-process timer from the CPU.
 *     CLOCK_THREAD_CPUTIME_ID  Thread-specific CPU-time clock.
 * </pre>
 * @param  res The pointer to a struct timespec structure to receive the time
 *         resolution.
 * @return If the function succeeds, the return value is 0.
 *         If the function fails, the return value is -1,
 *         with errno set to indicate the error.
 */
int clock_getres(clockid_t clock_id, struct timespec *res)
{
    switch(clock_id) {
    case CLOCK_MONOTONIC:
        {
            LARGE_INTEGER pf;

            if (QueryPerformanceFrequency(&pf) == 0)
                return lc_set_errno(EINVAL);

            res->tv_sec = 0;
            res->tv_nsec = (int) ((POW10_9 + (pf.QuadPart >> 1)) / pf.QuadPart);
            if (res->tv_nsec < 1)
                res->tv_nsec = 1;

            return 0;
        }

    case CLOCK_REALTIME:
    case CLOCK_PROCESS_CPUTIME_ID:
    case CLOCK_THREAD_CPUTIME_ID:
        {
            DWORD   timeAdjustment, timeIncrement;
            BOOL    isTimeAdjustmentDisabled;

            (void) GetSystemTimeAdjustment(&timeAdjustment, &timeIncrement, &isTimeAdjustmentDisabled);
            res->tv_sec = 0;
            res->tv_nsec = timeIncrement * 100;

            return 0;
        }
    default:
        break;
    }

    return lc_set_errno(EINVAL);
}


/**
 * Get the time of the specified clock clock_id and stores it in the struct
 * struct timespec pointed to by tp.
 * @param  clock_id The clock_id argument is the identifier of the particular
 *         clock on which to act. The following clocks are supported:
 * <pre>
 *     CLOCK_REALTIME  System-wide real-time clock. Setting this clock
 *                 requires appropriate privileges.
 *     CLOCK_MONOTONIC Clock that cannot be set and represents monotonic
 *                 time since some unspecified starting point.
 *     CLOCK_PROCESS_CPUTIME_ID High-resolution per-process timer from the CPU.
 *     CLOCK_THREAD_CPUTIME_ID  Thread-specific CPU-time clock.
 * </pre>
 * @param  tp The pointer to a struct timespec structure to receive the time.
 * @return If the function succeeds, the return value is 0.
 *         If the function fails, the return value is -1,
 *         with errno set to indicate the error.
 */
int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    unsigned __int64 t;
    LARGE_INTEGER pf, pc;
    union {
        unsigned __int64 u64;
        FILETIME ft;
    }  ct, et, kt, ut;

    switch(clock_id) {
    case CLOCK_REALTIME:
        {
            GetSystemTimeAsFileTime(&ct.ft);
            t = ct.u64 - DELTA_EPOCH_IN_100NS;
            tp->tv_sec = t / POW10_7;
            tp->tv_nsec = ((int) (t % POW10_7)) * 100;

            return 0;
        }

    case CLOCK_MONOTONIC:
        {
            if (QueryPerformanceFrequency(&pf) == 0)
                return lc_set_errno(EINVAL);

            if (QueryPerformanceCounter(&pc) == 0)
                return lc_set_errno(EINVAL);

            tp->tv_sec = pc.QuadPart / pf.QuadPart;
            tp->tv_nsec = (int) (((pc.QuadPart % pf.QuadPart) * POW10_9 + (pf.QuadPart >> 1)) / pf.QuadPart);
            if (tp->tv_nsec >= POW10_9) {
                tp->tv_sec ++;
                tp->tv_nsec -= POW10_9;
            }

            return 0;
        }

    case CLOCK_PROCESS_CPUTIME_ID:
        {
        if(0 == GetProcessTimes(GetCurrentProcess(), &ct.ft, &et.ft, &kt.ft, &ut.ft))
            return lc_set_errno(EINVAL);
        t = kt.u64 + ut.u64;
        tp->tv_sec = t / POW10_7;
        tp->tv_nsec = ((int) (t % POW10_7)) * 100;

        return 0;
        }

    case CLOCK_THREAD_CPUTIME_ID: 
        {
            if(0 == GetThreadTimes(GetCurrentThread(), &ct.ft, &et.ft, &kt.ft, &ut.ft))
                return lc_set_errno(EINVAL);
            t = kt.u64 + ut.u64;
            tp->tv_sec = t / POW10_7;
            tp->tv_nsec = ((int) (t % POW10_7)) * 100;

            return 0;
        }

    default:
        break;
    }

    return lc_set_errno(EINVAL);
}

/**
 * Sleep for the specified time.
 * @param  clock_id This argument should always be CLOCK_REALTIME (0).
 * @param  flags 0 for relative sleep interval, others for absolute waking up.
 * @param  request The desired sleep interval or absolute waking up time.
 * @param  remain The remain amount of time to sleep.
 *         The current implemention just ignore it.
 * @return If the function succeeds, the return value is 0.
 *         If the function fails, the return value is -1,
 *         with errno set to indicate the error.
 */
int clock_nanosleep(clockid_t clock_id, int flags,
                           const struct timespec *request,
                           struct timespec *remain)
{
    struct timespec tp;

    if (clock_id != CLOCK_REALTIME)
        return lc_set_errno(EINVAL);

    if (flags == 0)
        return _nanosleep(request, remain);

    /* TIMER_ABSTIME = 1 */
    clock_gettime(CLOCK_REALTIME, &tp);

    tp.tv_sec = request->tv_sec - tp.tv_sec;
    tp.tv_nsec = request->tv_nsec - tp.tv_nsec;
    if (tp.tv_nsec < 0) {
        tp.tv_nsec += POW10_9;
        tp.tv_sec --;
    }

    return _nanosleep(&tp, remain);
}

/**
 * Set the time of the specified clock clock_id.
 * @param  clock_id This argument should always be CLOCK_REALTIME (0).
 * @param  tp The requested time.
 * @return If the function succeeds, the return value is 0.
 *         If the function fails, the return value is -1,
 *         with errno set to indicate the error.
 */
int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
    SYSTEMTIME st;

    union {
        unsigned __int64 u64;
        FILETIME ft;
    }  t;

    if (clock_id != CLOCK_REALTIME)
        return lc_set_errno(EINVAL);

    t.u64 = tp->tv_sec * (__int64) POW10_7 + tp->tv_nsec / 100 + DELTA_EPOCH_IN_100NS;
    if (FileTimeToSystemTime(&t.ft, &st) == 0)
        return lc_set_errno(EINVAL);

    if (SetSystemTime(&st) == 0)
        return lc_set_errno(EPERM);

    return 0;
}

/**
 * Sleep for the specified time.
 * @param  request The desired amount of time to sleep.
 * @param  remain The remain amount of time to sleep.
 * @return If the function succeeds, the return value is 0.
 *         If the function fails, the return value is -1,
 *         with errno set to indicate the error.
 */
int _nanosleep(const struct timespec *request, struct timespec *remain)
{
    unsigned long ms, rc = 0;
    unsigned __int64 u64, want, real;

    union {
        unsigned __int64 ns100;
        FILETIME ft;
    }  _start, _end;

    if (request->tv_sec < 0 || request->tv_nsec < 0 || request->tv_nsec >= POW10_9) {
        errno = EINVAL;
        return -1;
    }

    if (remain != NULL) GetSystemTimeAsFileTime(&_start.ft);

    want = u64 = request->tv_sec * POW10_3 + request->tv_nsec / POW10_6;
    while (u64 > 0 && rc == 0) {
        if (u64 >= MAX_SLEEP_IN_MS) ms = MAX_SLEEP_IN_MS;
        else ms = (unsigned long) u64;

        u64 -= ms;
        rc = SleepEx(ms, TRUE);
    }

    if (rc != 0) { /* WAIT_IO_COMPLETION (192) */
        if (remain != NULL) {
            GetSystemTimeAsFileTime(&_end.ft);
            real = (_end.ns100 - _start.ns100) / POW10_4;

            if (real >= want) u64 = 0;
            else u64 = want - real;

            remain->tv_sec = u64 / POW10_3;
            remain->tv_nsec = (long) (u64 % POW10_3) * POW10_6;
        }

		return lc_set_errno( EINTR);
    }

    return 0;
}


