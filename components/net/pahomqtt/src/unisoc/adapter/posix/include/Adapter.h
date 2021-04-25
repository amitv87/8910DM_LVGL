#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "string.h"
#include "osi_api.h"
#include "osi_log.h"
#include "paho_config.h"

#if !defined(__PAHO_ADAPTER_H__)
#define __PAHO_ADAPTER_H__

int process_compilation_warning(int rc);

//Portint Time
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME              0x00
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC             0x01
#endif
int usleep(useconds_t usec);
int clock_gettime(clockid_t _clk_id, struct timespec *_tp);
long unisoc_difftime(time_t _time2, time_t _time1);

//Porting Thread
#define PTHREAD_MUTEX_INITIALIZER ((pthread_mutex_t) 0xFFFFFFFF)
#define PTHREAD_COND_INITIALIZER ((pthread_cond_t) 0xFFFFFFFF)

//typedef unsigned int pthread_t;	//already declaration
//typedef void pthread_attr_t;		//already declaration
#define PTHREAD_CREATE_DETACHED 0
#define pthread_attr_init(a)
#define pthread_attr_setdetachstate(a, b)
#define pthread_attr_destroy(a)
int pthread_create(pthread_t *_taskid, pthread_attr_t *_pattr, void *_pTaskEntry, void *_arg);
int pthread_cancel(pthread_t _taskid);
pthread_t pthread_self(void);

//Porting Mutex
//typedef unsigned int pthread_mutex_t;	//already declaration
//typedef struct {
//  int type;
//} pthread_mutexattr_t;				//already declaration
#define pthread_mutexattr_init(a)
#define pthread_mutexattr_settype(a, b)
int pthread_mutex_init(pthread_mutex_t * _mutex, pthread_mutexattr_t * _attr);
int pthread_mutex_lock(pthread_mutex_t * _mutex);
int pthread_mutex_trylock(pthread_mutex_t * _mutex);
int pthread_mutex_unlock(pthread_mutex_t * _mutex);
int pthread_mutex_destroy(pthread_mutex_t * _mutex);

//Porting Sem
typedef unsigned int sem_t;
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_wait(sem_t *_sem);
int sem_timedwait(sem_t *_sem, const struct timespec * _abstime);
int sem_trywait(sem_t *_sem);
int sem_post(sem_t *_sem);
int sem_destroy(sem_t *_sem);

//Porting Cond
//typedef unsigned int pthread_cond_t;	//already declaration
//typedef struct {
//  int      is_initialized;
//  clock_t  clock;
//#if defined(_POSIX_THREAD_PROCESS_SHARED)
//  int      process_shared;
//#endif
//} pthread_condattr_t;					//already declaration
#define pthread_condattr_init(a)
int pthread_cond_init(pthread_cond_t * _cond, pthread_condattr_t * _attr);
int pthread_cond_wait(pthread_cond_t * _cond, pthread_mutex_t *_mutex);
int pthread_cond_timedwait(pthread_cond_t * _cond, pthread_mutex_t *_mutex, struct timespec * _abstime);
int pthread_cond_signal(pthread_cond_t * _cond);
int pthread_cond_destroy(pthread_cond_t * _cond);

#endif


