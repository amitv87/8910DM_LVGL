#include "Adapter.h"

#define PAHOMQTT_PRIORITY (OSI_PRIORITY_NORMAL)
#define PAHOMQTT_STACK_SIZE (1024 * 4)
#define PAHOMQTT_EVENT_QUEUE_SIZE (32)

extern int gettimeofday(struct timeval *tv, void *tz);

//Porting Log
#if 0
void Log(int log_level, int msgno, char *format, ...)
{
    va_list args;
    char buff[1024] = {0};
    va_start(args, format);
    vsnprintf(buff, sizeof(buff) - 1, format, args);
    va_end(args);
	COS_LOGI(log_level, buff);
	return ;
}
#endif

int process_compilation_warning(int rc)
{
	return rc;
}

int usleep(useconds_t usec)
{
	osiThreadSleep(usec / 1000);

	return 0;
}

int clock_gettime(clockid_t _clk_id, struct timespec *_tp)
{
	gettimeofday((struct timeval *)_tp, NULL);
	_tp->tv_nsec = _tp->tv_nsec * 1000;

	return 0;
}

long unisoc_difftime(time_t _time2, time_t _time1)
{
	return ((long)(_time2 - _time1));
}

int pthread_create(pthread_t *_taskid, pthread_attr_t *_pattr, void *_pTaskEntry, void *_arg)
{	
	(*_taskid) = (pthread_t)osiThreadCreate("[Async Mqtt]",
                              				_pTaskEntry,
                              				_arg,
                              				PAHOMQTT_PRIORITY,
                              				PAHOMQTT_STACK_SIZE,
                              				PAHOMQTT_EVENT_QUEUE_SIZE);
	if (0 == (*_taskid))
	{
		return -1;
	}

	return 0;
}

int pthread_cancel(pthread_t _taskid)
{
	//Only delete self.
	osiThreadExit();

	return 0;
}

pthread_t pthread_self(void)
{
	return (pthread_t)osiThreadCurrent();
}

int pthread_mutex_init(pthread_mutex_t * _mutex, pthread_mutexattr_t * _attr)
{
	(*_mutex) = (pthread_mutex_t)osiMutexCreate();

	if (0 == (*_mutex))
	{
		return -1;
	}

	return 0;
}

int pthread_mutex_lock(pthread_mutex_t * _mutex)
{
	osiMutexLock((osiMutex_t *)_mutex);

	return 0;
}

int pthread_mutex_trylock(pthread_mutex_t * _mutex)
{
	return (int)osiMutexTryLock((osiMutex_t *)_mutex, 1);
}

int pthread_mutex_unlock(pthread_mutex_t * _mutex)
{
	osiMutexUnlock((osiMutex_t *)_mutex);

	return 0;
}

int pthread_mutex_destroy(pthread_mutex_t * _mutex)
{
	osiMutexDelete((osiMutex_t *)_mutex);

	return 0;
}

int sem_init(sem_t *_sem, int _pshared, unsigned int _value)
{
	if (0 == _value)
	{
		(*_sem) = (sem_t)osiSemaphoreCreate(1, 0);
		if (0 == (*_sem))
		{
			return -1;
		}
	}
	else
	{
		(*_sem) = (sem_t)osiSemaphoreCreate(_value, 0);
		if (0 == (*_sem))
		{
			return -1;
		}
	}
	
	return 0;
}

int sem_wait(sem_t *_sem)
{
	osiSemaphoreAcquire((osiSemaphore_t *)_sem);

	return 0;
}

int sem_timedwait(sem_t *_sem, const struct timespec * _abstime)
{
	int ret = -1;
	unsigned int iDifferenceValue = 0;
	struct timeval stExpectTime = {0};
	struct timeval stCurrentTime = {0};

	stExpectTime.tv_sec= _abstime->tv_sec;
	stExpectTime.tv_usec= (_abstime->tv_nsec / 1000);
	
	gettimeofday(&stCurrentTime, NULL);

	iDifferenceValue = ((stExpectTime.tv_sec * 1000000 + stExpectTime.tv_usec) 
		- (stCurrentTime.tv_sec * 1000000 + stCurrentTime.tv_usec)) / 1000;
	
	ret = (int)osiSemaphoreTryAcquire((osiSemaphore_t *)_sem, iDifferenceValue);

	return ret;
}

int sem_trywait(sem_t *_sem)
{
	return (int)osiSemaphoreTryAcquire((osiSemaphore_t *)_sem, 1);
}

int sem_post(sem_t *_sem)
{
	osiSemaphoreRelease((osiSemaphore_t *)_sem);

	return 0;
}

int sem_destroy(sem_t *_sem)
{
	osiSemaphoreDelete((osiSemaphore_t *)_sem);

	return 0;
}

int pthread_cond_init(pthread_cond_t * _cond, pthread_condattr_t * _attr)
{
	(*_cond) = (pthread_cond_t)osiSemaphoreCreate(1, 0);
	if (0 == (*_cond))
	{
		return -1;
	}

	return 0;
}

int pthread_cond_wait(pthread_cond_t * _cond, pthread_mutex_t *_mutex)
{
	pthread_mutex_unlock(_mutex);
	osiSemaphoreAcquire((osiSemaphore_t *)_cond);
	pthread_mutex_lock(_mutex);

	return 0;
}

int pthread_cond_timedwait(pthread_cond_t * _cond, pthread_mutex_t *_mutex, struct timespec * _abstime)
{
	int ret = -1;
	unsigned int iDifferenceValue = 0;
	struct timeval stExpectTime = {0};
	struct timeval stCurrentTime = {0};

	stExpectTime.tv_sec = _abstime->tv_sec;
	stExpectTime.tv_usec = (_abstime->tv_nsec / 1000);
	
	gettimeofday(&stCurrentTime, NULL);

	iDifferenceValue = ((stExpectTime.tv_sec * 1000000 + stExpectTime.tv_usec) 
		- (stCurrentTime.tv_sec * 1000000 + stCurrentTime.tv_usec)) / 1000;
	
	pthread_mutex_unlock(_mutex);
	ret = (int)osiSemaphoreTryAcquire((osiSemaphore_t *)_cond, iDifferenceValue);
	pthread_mutex_lock(_mutex);

	return ret;
}

int pthread_cond_signal(pthread_cond_t * _cond)
{
	osiSemaphoreRelease((osiSemaphore_t *)_cond);

	return 0;
}

int pthread_cond_destroy(pthread_cond_t * _cond)
{
	osiSemaphoreDelete((osiSemaphore_t *)_cond);

	return 0;
}


