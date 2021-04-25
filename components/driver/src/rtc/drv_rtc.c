/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('R', 'T', 'C', 'A')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_rtc.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hal_config.h"
#include "drv_config.h"
#include "pb_util.h"
#include "drv_rtc.pb.h"
#include "drv_rtc_hal.h"
#include "vfs.h"
#include "hwregs.h"
#include <sys/queue.h>
#include <stdlib.h>
#include <time.h>

#define RTC_NV_FILE_NAME CONFIG_FS_AP_NVM_DIR "/rtc_alarm.nv"

#define RTC_LOCK(lock) osiMutexLock(lock)
#define RTC_UNLOCK(lock) osiMutexUnlock(lock)

#define RTC_TASK_PRIORITY (OSI_PRIORITY_NORMAL)
#define RTC_TASK_STACK_SIZE (2048)
#define RTC_TASK_QUEUE_SIZE (16)

// #define RTC_TIME_VALID_MIN 1514764800 // 2018-01-01 UTC
#define RTC_TIME_VALID_OFFSET (365 * DAY_SECONDS)

typedef TAILQ_ENTRY(drvRtcAlarmOwner) drvRtcAlarmOwnerIter_t;
typedef TAILQ_HEAD(, drvRtcAlarmOwner) drvRtcAlarmOwnerHead_t;
typedef struct drvRtcAlarmOwner
{
    uint32_t owner;
    drvRtcAlarmCB_t cb;
    void *ctx;
    drvRtcAlarmOwnerIter_t iter;
} drvRtcAlarmOwner_t;

typedef TAILQ_ENTRY(drvRtcAlarmItem) drvRtcAlarmIter_t;
typedef TAILQ_HEAD(drvRtcAlarmHead, drvRtcAlarmItem) drvRtcAlarmHead_t;
typedef struct drvRtcAlarmItem
{
    drvRtcAlarm_t alarm;
    drvRtcAlarmOwner_t *powner;
    drvRtcAlarmIter_t iter;
} drvRtcAlarmItem_t;

typedef struct
{
    uint64_t last_time;
    drvRtcAlarmHead_t alarms;         // active alarms
    drvRtcAlarmHead_t expired_alarms; // expired alarms without owners
    drvRtcAlarmOwnerHead_t owners;
    osiMutex_t *lock;

    bool storenv_needed;
    int64_t last_alarm; // 0 for none
    osiWorkQueue_t *work_queue;
    osiWork_t *work;
    osiTimer_t *timer;

    drvRtcHalContext_t hal;
} drvRtcContext_t;

static drvRtcContext_t gDrvRtcCtx;

// Set expire second. 64bits store is not atomic
// Most of accesses are protected by mutex, and expire_sec will be
// read inside drvRtcGetWakeupTime without mutex. So, critical
// section is used for atomic.
static inline void prvSetExpireSec(drvRtcAlarmItem_t *p, int64_t sec)
{
    uint32_t critical = osiEnterCritical();
    p->alarm.expire_sec = sec;
    osiExitCritical(critical);
}

// Insert alarm in expiration time order
static void prvInsertAlarm(drvRtcAlarmHead_t *head, drvRtcAlarmItem_t *pa)
{
    drvRtcAlarmItem_t *p;
    TAILQ_FOREACH(p, head, iter)
    {
        if (pa->alarm.expire_sec < p->alarm.expire_sec)
        {
            TAILQ_INSERT_BEFORE(p, pa, iter);
            return;
        }
    }
    TAILQ_INSERT_TAIL(head, pa, iter);
}

// decode one alarm from pb stream
static bool prvRtcAlarmDecode(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    drvRtcContext_t *d = (drvRtcContext_t *)*arg;
    pbDrvRtcAlarm pbdata;
    pbDrvRtcAlarm *pbs = &pbdata;

    drvRtcAlarmItem_t *setting = (drvRtcAlarmItem_t *)calloc(1, sizeof(drvRtcAlarmItem_t));
    if (setting == NULL)
        return false;

    PB_DEC_STRING(setting->alarm.info, info);

    if (!pb_decode(stream, pbDrvRtcAlarm_fields, pbs))
    {
        free(setting);
        return false;
    }

    PB_DEC_ASSIGN(setting->alarm.owner, owner);
    PB_DEC_ASSIGN(setting->alarm.name, name);
    PB_DEC_ASSIGN(setting->alarm.type, type);
    PB_DEC_ASSIGN(setting->alarm.expire_sec, expire_sec);
    PB_OPT_DEC_ASSIGN(setting->alarm.wday_repeated.wday_mask, repeated_wday_mask);
    PB_OPT_DEC_ASSIGN(setting->alarm.wday_repeated.sec_in_day, repeated_sec_in_day);
    PB_OPT_DEC_ASSIGN(setting->alarm.wday_repeated.timezone, repeated_timezone);

    // The store alarms may be unsorted. They are insert to active list, and
    // will check expiration later.
    prvInsertAlarm(&d->alarms, setting);
    return true;
}

// encode one alarm list to pb stream
static bool prvRtcAlarmHeadEncode(const drvRtcAlarmHead_t *head, pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    pbDrvRtcAlarm pbdata;
    pbDrvRtcAlarm *pbs = &pbdata;

    drvRtcAlarmItem_t *setting;

    TAILQ_FOREACH(setting, head, iter)
    {
        if (!pb_encode_tag_for_field(stream, field))
            return false;

        memset(pbs, 0, sizeof(*pbs));
        PB_ENC_ASSIGN(setting->alarm.owner, owner);
        PB_ENC_ASSIGN(setting->alarm.name, name);
        PB_ENC_ASSIGN(setting->alarm.type, type);
        PB_ENC_ASSIGN(setting->alarm.expire_sec, expire_sec);
        PB_ENC_STRING(setting->alarm.info, info);
        if (setting->alarm.type == DRV_RTC_ALARM_WDAY_REPEATED)
        {
            PB_OPT_ENC_ASSIGN(setting->alarm.wday_repeated.wday_mask, repeated_wday_mask);
            PB_OPT_ENC_ASSIGN(setting->alarm.wday_repeated.sec_in_day, repeated_sec_in_day);
            PB_OPT_ENC_ASSIGN(setting->alarm.wday_repeated.timezone, repeated_timezone);
        }
        if (!pb_encode_submessage(stream, pbDrvRtcAlarm_fields, pbs))
            return false;
    }
    return true;
}

// encode all alarms to pb stream
static bool prvRtcAlarmEncode(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    const drvRtcContext_t *d = (const drvRtcContext_t *)*arg;
    if (!prvRtcAlarmHeadEncode(&d->expired_alarms, stream, field, arg))
        return false;
    if (!prvRtcAlarmHeadEncode(&d->alarms, stream, field, arg))
        return false;
    return true;
}

// decode NV to RTC context
static bool prvNvDecode(drvRtcContext_t *setting, uint8_t *buffer, unsigned length)
{
    const pb_field_t *fields = pbDrvRtc_fields;
    pbDrvRtc pbdata;
    pbDrvRtc *pbs = &pbdata;

    PB_DEC_CB(alarms, prvRtcAlarmDecode, setting);

    pb_istream_t is = pb_istream_from_buffer(buffer, length);
    if (!pb_decode(&is, fields, pbs))
        return false;

    PB_DEC_ASSIGN(setting->last_time, last_time);
    return true;
}

// enode NV from RTC context
static int prvNvEncode(const drvRtcContext_t *setting, void *buffer, unsigned length)
{
    pbDrvRtc pbdata;
    pbDrvRtc *pbs = &pbdata;

    PB_ENC_CB(alarms, prvRtcAlarmEncode, (void *)setting);
    PB_ENC_ASSIGN(setting->last_time, last_time);
    return pbEncodeToMem(pbDrvRtc_fields, pbs, buffer, length);
}

// load RTC NV file
static bool prvLoadnv(drvRtcContext_t *d)
{
    vfs_sfile_init(RTC_NV_FILE_NAME);
    ssize_t file_size = vfs_sfile_size(RTC_NV_FILE_NAME);
    if (file_size < 0)
        return false;

    uint8_t *buffer = (uint8_t *)calloc(1, file_size);
    if (buffer == NULL)
        return false;

    bool ok = (vfs_sfile_read(RTC_NV_FILE_NAME, buffer, file_size) == file_size) &&
              prvNvDecode(d, buffer, file_size);
    free(buffer);
    return ok;
}

// store RTC NV file
static bool prvStorenv(drvRtcContext_t *d)
{
    int length = prvNvEncode(d, NULL, 0);
    if (length < 0)
        return false;

    void *buffer = calloc(1, length);
    if (buffer == NULL)
        goto failed;

    length = prvNvEncode(d, buffer, length);
    if (length < 0)
        goto failed;

    if (vfs_sfile_write(RTC_NV_FILE_NAME, buffer, length) != length)
        goto failed;

    OSI_LOGI(0, "RTC store nv length/%d", length);
    free(buffer);
    return true;

failed:
    OSI_LOGI(0, "RTC store nv failed");
    free(buffer);
    return false;
}

// Calculate weekday repeated alarm next expire time,
// must be GREATER THAN current.
static int64_t prvNextExpire(int64_t curr, drvRtcAlarmWdayRepeated_t *par)
{
    int timeoffset = par->timezone;
    time_t tcurr = curr + timeoffset;
    struct tm tm;

    gmtime_r(&tcurr, &tm);
    uint32_t sec_in_day = tm.tm_sec + tm.tm_min * MIN_SECONDS + tm.tm_hour * HOUR_SECONDS;
    unsigned wday = tm.tm_wday;
    unsigned offset = 0; // seconds from now

    if (sec_in_day >= par->sec_in_day)
    {
        wday++;
        offset = (DAY_SECONDS + par->sec_in_day) - sec_in_day;
    }
    else
    {
        offset = par->sec_in_day - sec_in_day;
    }

    for (int n = 0; n < WEEK_DAYS; n++)
    {
        unsigned mask = 1 << ((wday + n) % WEEK_DAYS);
        if ((par->wday_mask & mask) != 0)
            break;
        offset += DAY_SECONDS;
    }
    return curr + offset;
}

// Find alarm by owner and name, NULL for not found
static inline drvRtcAlarmItem_t *prvFindAlarm(drvRtcContext_t *d, uint32_t owner, uint32_t name)
{
    drvRtcAlarmItem_t *p;
    TAILQ_FOREACH(p, &d->alarms, iter)
    {
        if (p->alarm.name == name && p->alarm.owner == owner)
            return p;
    }
    return NULL;
}

// Find owner, NULL for not found
static inline drvRtcAlarmOwner_t *prvFindOwner(drvRtcContext_t *d, uint32_t owner)
{
    drvRtcAlarmOwner_t *o;
    TAILQ_FOREACH(o, &d->owners, iter)
    {
        if (o->owner == owner)
            return o;
    }
    return NULL;
}

// Handle alarm, including:
// * call expired alarm callback
// * set/unset RTC alarm
// * store NV
static void prvHandleAlarm(void *ctx)
{
    drvRtcContext_t *d = &gDrvRtcCtx;

    RTC_LOCK(d->lock);
    bool storenv_needed = d->storenv_needed;
    d->storenv_needed = false;

    int64_t msec = osiEpochTime();
    int64_t sec = OSI_DIV_ROUND_UP(msec, 1000);

    // Collect expired alarms, and exclude alarms without owner.
    // The alarms without owner should be kept in the list.
    drvRtcAlarmItem_t *p;
    drvRtcAlarmItem_t *pt;
    drvRtcAlarmHead_t ex_list;
    TAILQ_INIT(&ex_list);

    int64_t next_expired = 0;
    TAILQ_FOREACH_SAFE(p, &d->alarms, iter, pt)
    {
        if (p->alarm.expire_sec > sec)
        {
            next_expired = p->alarm.expire_sec;
            break;
        }

        TAILQ_REMOVE(&d->alarms, p, iter);
        if (p->powner == NULL)
        {
            // Put to expired list when owner is not registered
            prvInsertAlarm(&d->expired_alarms, p);
        }
        else
        {
            // When there are expired alarm, store NV is always needed
            storenv_needed = true;
            TAILQ_INSERT_TAIL(&ex_list, p, iter);
        }
    }

    // run callbacks of expired alarms
    TAILQ_FOREACH_SAFE(p, &ex_list, iter, pt)
    {
        TAILQ_REMOVE(&ex_list, p, iter);

        // alarm set/unset is prohibited inside callbacks.
        if (p->powner->cb)
            p->powner->cb(&p->alarm, p->powner->ctx);

        if (p->alarm.type == DRV_RTC_ALARM_ONE_TIME)
        {
            free(p);
        }
        else
        {
            // "sec + 1" is used to check repeated alarm. It is needed
            // to get next expiration with "sec + 1".
            prvSetExpireSec(p, prvNextExpire(sec + 1, &p->alarm.wday_repeated));
            prvInsertAlarm(&d->alarms, p);
        }
    }

    // unset/set RTC alarm if needed
    if (next_expired == 0)
    {
        if (d->last_alarm != 0)
        {
            d->last_alarm = 0;
            drvRtcHalUnsetAlarm(&d->hal);
            osiTimerStop(d->timer);
        }
    }
    else if (next_expired != d->last_alarm)
    {
        // It is ensured that the timer period won't underflow
        d->last_alarm = next_expired;
        drvRtcHalSetAlarm(&d->hal, next_expired);
        osiTimerStart(d->timer, next_expired * 1000 - msec);
    }

    if (storenv_needed)
    {
        d->last_time = sec;
        prvStorenv(d);
    }

    RTC_UNLOCK(d->lock);
}

static void prvInitCtx(drvRtcContext_t *d)
{
    d->last_time = 0;
    TAILQ_INIT(&d->alarms);
    TAILQ_INIT(&d->expired_alarms);
    TAILQ_INIT(&d->owners);
}

static void prvFreeAllAlarms(drvRtcAlarmHead_t *head)
{
    drvRtcAlarmItem_t *p;
    while ((p = TAILQ_FIRST(head)) != NULL)
    {
        TAILQ_REMOVE(head, p, iter);
        free(p);
    }
}

static void prvTimerTimeout(void *param)
{
    drvRtcContext_t *d = &gDrvRtcCtx;
    osiWorkEnqueue(d->work, d->work_queue);
}

void drvRtcInit(void)
{
    drvRtcContext_t *d = &gDrvRtcCtx;
    prvInitCtx(d);

    // load context to a temporal storage
    drvRtcContext_t setting;
    prvInitCtx(&setting);
    if (prvLoadnv(&setting))
    {
        d->last_time = setting.last_time;
        TAILQ_SWAP(&d->alarms, &setting.alarms, drvRtcAlarmItem, iter);
    }
    else
    {
        prvFreeAllAlarms(&setting.alarms);
    }

    d->lock = osiMutexCreate();
    d->storenv_needed = false;
    d->last_alarm = 0;
    d->work_queue = osiWorkQueueCreate("rtc", 1, RTC_TASK_PRIORITY, RTC_TASK_STACK_SIZE);
    d->work = osiWorkCreate(prvHandleAlarm, NULL, d);
    d->timer = osiTimerCreate(NULL, prvTimerTimeout, NULL);

    drvRtcHalInit(&d->hal);
    int64_t sec = osiEpochSecond();

    if (d->last_time < CONFIG_KERNEL_MIN_UTC_SECOND ||
        d->last_time > CONFIG_KERNEL_MAX_UTC_SECOND)
        d->last_time = CONFIG_KERNEL_MIN_UTC_SECOND;

    if (sec >= d->last_time && sec <= d->last_time + RTC_TIME_VALID_OFFSET)
    {
        OSI_LOGI(0, "update nv, NV/%u RTC/%u", (unsigned)d->last_time, (unsigned)sec);
        d->last_time = sec;
        prvStorenv(d);
    }
    else
    {
        OSI_LOGI(0, "update rtc, NV/%u RTC/%u", (unsigned)d->last_time, (unsigned)sec);
        drvRtcHalWriteSecond(&d->hal, d->last_time);
    }

    osiBootCause_t cause = osiGetBootCauses();
    if (cause & OSI_BOOTCAUSE_PSM_WAKEUP)
    {
        // epoch time should already be set in PSM wakeup
        drvRtcUpdateTime();
    }
    else
    {
        int64_t rtc_ms = d->last_time * 1000;
        osiSetEpochTime(rtc_ms);
    }

    // Trigger handle to check expired alarms.
    drvRtcHandleAlarm();
}

void drvRtcHandleAlarm(void)
{
    drvRtcContext_t *d = &gDrvRtcCtx;
    osiWorkEnqueue(d->work, d->work_queue);
}

bool drvRtcSetAlarm(uint32_t owner, uint32_t name, const void *info,
                    uint32_t info_size, int64_t sec, bool replace)
{
    if (info_size > DRV_RTC_ALARM_INFO_SIZE)
        return false;

    drvRtcContext_t *d = &gDrvRtcCtx;
    int64_t curr = osiEpochSecond();
    if (sec <= curr)
        return false;

    RTC_LOCK(d->lock);

    drvRtcAlarmItem_t *p = prvFindAlarm(d, owner, name);
    if (p != NULL)
    {
        if (!replace)
        {
            RTC_UNLOCK(d->lock);
            return false;
        }
        TAILQ_REMOVE(&d->alarms, p, iter);
    }
    else
    {
        p = (drvRtcAlarmItem_t *)calloc(1, sizeof(*p));
        if (p == NULL)
        {
            RTC_UNLOCK(d->lock);
            return false;
        }
    }

    p->alarm.name = name;
    p->alarm.owner = owner;
    p->alarm.type = DRV_RTC_ALARM_ONE_TIME;
    p->powner = prvFindOwner(d, owner);
    memcpy(p->alarm.info, info, info_size);
    prvSetExpireSec(p, sec);

    prvInsertAlarm(&d->alarms, p);
    d->storenv_needed = true;

    RTC_UNLOCK(d->lock);
    drvRtcHandleAlarm();
    return true;
}

bool drvRtcSetRepeatAlarm(uint32_t owner, uint32_t name, const void *info,
                          uint32_t info_size, uint8_t wday_mask,
                          uint32_t sec_in_day, int timezone, bool replace)
{
    if (info_size > DRV_RTC_ALARM_INFO_SIZE || wday_mask == 0)
        return false;

    drvRtcAlarmWdayRepeated_t par = {
        .wday_mask = wday_mask,
        .sec_in_day = sec_in_day,
        .timezone = timezone,
    };

    drvRtcContext_t *d = &gDrvRtcCtx;
    RTC_LOCK(d->lock);

    drvRtcAlarmItem_t *p = prvFindAlarm(d, owner, name);
    if (p != NULL)
    {
        if (!replace)
        {
            RTC_UNLOCK(d->lock);
            return false;
        }
        TAILQ_REMOVE(&d->alarms, p, iter);
    }
    else
    {
        p = (drvRtcAlarmItem_t *)calloc(1, sizeof(*p));
        if (p == NULL)
        {
            RTC_UNLOCK(d->lock);
            return false;
        }
    }

    int64_t curr = osiEpochSecond();
    p->alarm.owner = owner;
    p->alarm.name = name;
    p->alarm.type = DRV_RTC_ALARM_WDAY_REPEATED;
    p->alarm.wday_repeated = par;
    p->powner = prvFindOwner(d, owner);
    memcpy(p->alarm.info, info, info_size);
    prvSetExpireSec(p, prvNextExpire(curr, &par));

    prvInsertAlarm(&d->alarms, p);
    d->storenv_needed = true;

    RTC_UNLOCK(d->lock);
    drvRtcHandleAlarm();
    return true;
}

bool drvRtcRemoveAlarm(uint32_t owner, uint32_t name)
{
    drvRtcContext_t *d = &gDrvRtcCtx;
    int count = 0;

    RTC_LOCK(d->lock);

    // Though it shouldn't exist duplicated alarms, we will
    // implement delete all.
    drvRtcAlarmItem_t *p;
    while ((p = prvFindAlarm(d, owner, name)) != NULL)
    {
        TAILQ_REMOVE(&d->alarms, p, iter);

        free(p);
        d->storenv_needed = true;
        count++;
    }

    RTC_UNLOCK(d->lock);

    if (count > 0)
        drvRtcHandleAlarm();
    return (count > 0);
}

bool drvRtcAlarmOwnerSetCB(uint32_t owner, void *ctx, drvRtcAlarmCB_t cb)
{
    drvRtcContext_t *d = &gDrvRtcCtx;
    RTC_LOCK(d->lock);

    drvRtcAlarmOwner_t *powner = prvFindOwner(d, owner);
    if (powner != NULL)
    {
        powner->ctx = ctx;
        powner->cb = cb;
    }
    else
    {
        powner = (drvRtcAlarmOwner_t *)calloc(1, sizeof(*powner));
        if (powner == NULL)
        {
            RTC_UNLOCK(d->lock);
            return false;
        }

        powner->owner = owner;
        powner->ctx = ctx;
        powner->cb = cb;
        TAILQ_INSERT_TAIL(&d->owners, powner, iter);

        drvRtcAlarmItem_t *p, *pt;
        TAILQ_FOREACH(p, &d->alarms, iter)
        {
            if (p->alarm.owner == owner)
                p->powner = powner;
        }

        // Check the expired alarms to make the callback to be called.
        TAILQ_FOREACH_SAFE(p, &d->expired_alarms, iter, pt)
        {
            if (p->alarm.owner == owner)
            {
                p->powner = powner;
                TAILQ_REMOVE(&d->expired_alarms, p, iter);
                prvInsertAlarm(&d->alarms, p);
            }
        }
    }

    RTC_UNLOCK(d->lock);
    drvRtcHandleAlarm();
    return true;
}

int drvRtcGetAlarmCount(uint32_t owner)
{
    int count = 0;
    drvRtcContext_t *d = &gDrvRtcCtx;
    RTC_LOCK(d->lock);

    // For specified owner, it is not needed to check expired alarms.
    drvRtcAlarmItem_t *p;
    TAILQ_FOREACH(p, &d->alarms, iter)
    {
        if (p->alarm.owner == owner)
            count++;
    }

    RTC_UNLOCK(d->lock);
    return count;
}

int drvRtcGetAlarms(uint32_t owner, drvRtcAlarm_t *alarms, uint32_t max_count)
{
    if (alarms == NULL)
        return 0;

    int count = 0;
    drvRtcContext_t *d = &gDrvRtcCtx;
    RTC_LOCK(d->lock);

    // For specified owner, it is not needed to check expired alarms.
    drvRtcAlarmItem_t *p;
    TAILQ_FOREACH(p, &d->alarms, iter)
    {
        if (count == max_count)
            break;

        if (p->alarm.owner == owner)
        {
            *alarms++ = p->alarm;
            count++;
        }
    }

    RTC_UNLOCK(d->lock);
    return count;
}

void drvRtcRemoveAllAlarms(void)
{
    drvRtcContext_t *d = &gDrvRtcCtx;
    RTC_LOCK(d->lock);

    prvFreeAllAlarms(&d->alarms);
    d->storenv_needed = true;

    RTC_UNLOCK(d->lock);
    drvRtcHandleAlarm();
}

void drvRtcUpdateTime(void)
{
    drvRtcContext_t *d = &gDrvRtcCtx;
    RTC_LOCK(d->lock);

    int64_t curr = osiEpochSecond();
    d->last_alarm = 0;
    d->storenv_needed = true;
    drvRtcHalUnsetAlarm(&d->hal);
    drvRtcHalWriteSecond(&d->hal, curr);

    // Repeated alarm at system time change:
    // * When system time is changed forward (increase), repeated alarm will
    //   expire as normal. Such as there is an alarm at Tuesday, system time
    //   is changed from Monday to Wendsday, the alarm will be expired and
    //   the next expiration time will be set to next Tuesday.
    // * When system time is changed backward (decrease), there are no
    //   expiration. And the next expiration time will be recalculated based
    //   on the new system time.
    //
    // So, recalculate non-expired and check expired as normal

    drvRtcAlarmItem_t *p, *pt;
    drvRtcAlarmHead_t repeat_list;
    TAILQ_INIT(&repeat_list);
    TAILQ_FOREACH_SAFE(p, &d->alarms, iter, pt)
    {
        // reset unexpired repeat alarms
        if (p->alarm.type == DRV_RTC_ALARM_WDAY_REPEATED && p->alarm.expire_sec > curr)
        {
            TAILQ_REMOVE(&d->alarms, p, iter);
            TAILQ_INSERT_HEAD(&repeat_list, p, iter);
        }
    }

    TAILQ_FOREACH_SAFE(p, &repeat_list, iter, pt)
    {
        TAILQ_REMOVE(&repeat_list, p, iter);
        prvSetExpireSec(p, prvNextExpire(curr, &p->alarm.wday_repeated));
        prvInsertAlarm(&d->alarms, p);
    }

    RTC_UNLOCK(d->lock);
    drvRtcHandleAlarm();
}

int64_t drvRtcGetWakeupTime(void)
{
    drvRtcContext_t *d = &gDrvRtcCtx;
    uint32_t critical = osiEnterCritical();

    drvRtcAlarmItem_t *p = TAILQ_FIRST(&d->alarms);
    int64_t wakeup = (p == NULL) ? INT64_MAX : p->alarm.expire_sec * 1000; // convert second to ms

    osiExitCritical(critical);
    return wakeup;
}
