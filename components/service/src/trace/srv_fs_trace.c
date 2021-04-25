/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('T', 'D', 'F', 'S')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

/**
 * \brief file system trace
 *
 * File system trace module will write various trace into file system,
 * typically write to sdcard.
 *
 * File write will be performed in separated thread. And when there are
 * multiple trace sources, such as aplog, cplog and etc, only one thread
 * will be used. It is unreasonable to use a separated thread for each
 * trace data source. The priority of thread of fstarce should be higher
 * than normal. This can reduce possibility of trace lose, and it is
 * expected that fstrace shouldn't occupy too many cpu cycles.
 *
 * The write speed of sdcard depends on write block size. The larger
 * of the write block, the write performance will be better. Also, the
 * total cpu cycles will be less. Currently, there are no cache for
 * sdcard/fatfs, and it is possible that the cache won't be added. So,
 * fstrace module will try best to write larger blocks.
 *
 * For aplog, there will be an memcpy. The fstrace module will allocate
 * one buffer. Trace data will be copied to fstrace buffer. Only when
 * fstrace buffer is full, it will be written to file system.
 *
 * For cplog, additional memcpy is not needed. The fstrace module will
 * write from shared memory directly. And then it should be avoid to
 * write small blocks as possible. The shared memory is read only, and
 * it is needed to access them as cachable for performance.
 *
 * Periodic sync is be needed. In case of power off, it will reduce
 * trace data lose. However, the interval should be relative large to
 * reduce the impact of normal system.
 *
 * For blue screen, when blue screen occurs on the call chain of fstrace,
 * there may exist trace data lose. This limitation shoule be accepted.
 *
 * At erntering blue screen, the trace data will be flushed. Also, the
 * blue screen data will be stored.
 */

#include "srv_trace.h"
#include "srv_config.h"
#include "fs_config.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_trace.h"
#include "osi_profile.h"
#include "vfs.h"
#include "drv_modem_log_buf.h"
#include "hal_chip.h"
#include "hal_hw_map.h"
#include "hal_adi_bus.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef CONFIG_FS_TRACE_ENABLE

#define FSTRACE_POLL_INTERVAL (50)
#define FSTRACE_FSYNC_INTERVAL (1000)
#define FSTRACE_THREAD_PRIORITY OSI_PRIORITY_BELOW_NORMAL
#define FSTRACE_THREAD_STACK_SIZE (2048)

#define FSTRACE_FNAME_MAX (64)
#define FSTRACE_SIZE_MAX (2048 * 1024 * 1024LL)
#define FSTRACE_APLOG_EACH_MAX (10 * 1024 * 1024)
#define FSTRACE_CPLOG_EACH_MAX (50 * 1024 * 1024)
#define FSTRACE_MOSLOG_EACH_MAX (1 * 1024 * 1024)
#define FSTRACE_PROLOG_EACH_MAX (1 * 1024 * 1024)
#define FSTRACE_SEQ_START (1000)
#define FSTRACE_BSDUMP_RANGE_COUNT_MAX (8)

#ifdef CONFIG_SOC_8910
#define FSTRACE_SUPPORT_CPLOG
#define FSTRACE_SUPPORT_MOSLOG
#endif

#define FSTRACE_MOUNT_POINT CONFIG_FS_SDCARD_MOUNT_POINT
#define FSTRACE_DIR_PATTERN /**/ "%d"
#define FSTRACE_APLOG_FILE_PATTERN /**/ "aplog-%d.bin"
#define FSTRACE_PROLOG_FILE_PATTERN /**/ "prolog-%d.bin"
#define FSTRACE_CPLOG_FILE_PATTERN /**/ "cplog-%d.bin"
#define FSTRACE_MOSLOG_FILE_PATTERN /**/ "moslog-%d.bin"
#define FSTRACE_DIR_NAME /**/ CONFIG_FS_SDCARD_MOUNT_POINT "/syslog"
#define FSTRACE_DIR_NAME_PATTERN /**/ FSTRACE_DIR_NAME "/" FSTRACE_DIR_PATTERN
#define FSTRACE_APLOG_DIR_NAME_PATTERN /**/ FSTRACE_DIR_NAME_PATTERN "/aplog"
#define FSTRACE_PROLOG_DIR_NAME_PATTERN /**/ FSTRACE_DIR_NAME_PATTERN "/prolog"
#define FSTRACE_CPLOG_DIR_NAME_PATTERN /**/ FSTRACE_DIR_NAME_PATTERN "/cplog"
#define FSTRACE_MOSLOG_DIR_NAME_PATTERN /**/ FSTRACE_DIR_NAME_PATTERN "/moslog"
#define FSTRACE_BSDUMP_FILE_NAME_PATTERN /**/ FSTRACE_DIR_NAME_PATTERN "/bsdump/%08x.bin"

#define FSTRACE_OUTPUT_FLUSH (1 << 0)

typedef struct
{
    bool prepared;              // initial scan is done
    osiTimer_t *tx_timer;       // timer for output
    osiWork_t *tx_work;         // work for output
    osiWorkQueue_t *tx_wq;      // work queue for output
    unsigned option;            // options
    unsigned prolog_pos;        // profile position
    int prolog_index;           // profile file index
    int prolog_fd;              // profile file descriptor
    int prolog_fsize;           // profile file size
    int aplog_index;            // aplog file index
    int aplog_fd;               // aplog file descriptor
    int aplog_fsize;            // aplog file size
#ifdef FSTRACE_SUPPORT_CPLOG    //
    int cplog_index;            // cplog file index
    int cplog_fd;               // cplog file descriptor
    int cplog_fsize;            // cplog file size
    drvModemLogBuf_t cplogbuf;  // cplog shared buffer
    drvModemLogBuf_t zsplogbuf; // zsplog shared buffer
#endif                          //
#ifdef FSTRACE_SUPPORT_MOSLOG   //
    int moslog_index;           // moslog file index
    int moslog_fd;              // moslog file descriptor
    int moslog_fsize;           // moslog file size
    drvModemLogBuf_t moslogbuf; // moslog shared buffer
#endif                          //
    unsigned sequence;          // fstrace sequence
    int64_t total_size;         // fstrace total size
    int64_t last_sync;          // last fsync time
} srvFsTraceContext_t;

static srvFsTraceContext_t *gFstraceCtx OSI_SECTION_RW_KEEP = NULL;

/**
 * Find minimal and maximal sequence under a directory. \p pattern should
 * only contain one \p %d, and others shouls be fixed characters. When
 * there are no found, return false.
 */
static bool prvFstraceFindMinMaxSeq(
    srvFsTraceContext_t *d,
    const char *dname,
    int *min_seq,
    int *max_seq,
    const char *pattern)
{
    DIR *dir = vfs_opendir(FSTRACE_DIR_NAME);
    if (dir == NULL)
        return false;

    char name[FSTRACE_FNAME_MAX];
    struct dirent ent;
    struct dirent *rent = NULL;
    *min_seq = INT_MAX;
    *max_seq = INT_MIN;
    while (vfs_readdir_r(dir, &ent, &rent) == 0 && rent != NULL)
    {
        if (rent->d_type != DT_DIR)
            continue;

        int seq;
        if (sscanf(rent->d_name, pattern, &seq) != 1)
            continue;

        // Brute force checking the name match pattern exactly.
        sprintf(name, pattern, seq);
        if (strcmp(rent->d_name, name) != 0)
            continue;

        if (seq < *min_seq)
            *min_seq = seq;
        if (seq > *max_seq)
            *max_seq = seq;
    }

    vfs_closedir(dir);
    return (*min_seq <= *max_seq);
}

/**
 * Check whether total size exceeds threshold. When exceeds, recursive
 * remove the directory with least sequence. When current sequence is the
 * maximal sequence, which means it is the only one, not to remove it.
 * As a safe check, when the minimal sequence doesn't change after remove
 * the least sequence, which means that something wrong, exit the checking
 * to avoid infinite loop.
 */
static bool prvFstraceCheckSeqSize(srvFsTraceContext_t *d)
{
    char path[FSTRACE_FNAME_MAX];
    int last_min_seq = -1;
    for (;;)
    {
        if (d->total_size < FSTRACE_SIZE_MAX)
            return true;

        int min_seq, max_seq;
        if (!prvFstraceFindMinMaxSeq(d, FSTRACE_DIR_NAME, &min_seq,
                                     &max_seq, FSTRACE_DIR_PATTERN))
            return false;

        if (max_seq == d->sequence || min_seq == last_min_seq)
            return false;

        last_min_seq = min_seq;
        sprintf(path, FSTRACE_DIR_NAME_PATTERN, min_seq);

        int64_t subtotal = vfs_dir_total_size(path);
        if (subtotal > 0)
            d->total_size -= subtotal;
        vfs_rmdir_recursive(path);
    }
}

/**
 * Index file name under \p dname, with name pattern \p pattern.
 */
static void prvFstraceIndexFname(
    char *path,
    const char *dname,
    const char *pattern,
    int index)
{
    int dname_len = strlen(dname);
    memcpy(path, dname, dname_len);
    path[dname_len++] = '/';
    sprintf(&path[dname_len], pattern, index);
}

/**
 * Similar to \p prvFstraceCheckSeqSize, just to remove file with least
 * index under \p dname.
 */
static bool prvFstraceCheckIndexSize(
    srvFsTraceContext_t *d,
    const char *dname,
    const char *pattern,
    int curindex)
{
    char path[FSTRACE_FNAME_MAX];
    int last_min_index = -1;
    for (;;)
    {
        if (d->total_size < FSTRACE_SIZE_MAX)
            return true;

        int min_index, max_index;
        if (!prvFstraceFindMinMaxSeq(d, dname, &min_index, &max_index, pattern))
            return false;

        if (max_index == curindex || min_index == last_min_index)
            return false;

        last_min_index = min_index;
        prvFstraceIndexFname(path, dname, pattern, min_index);

        int size = vfs_file_size(path);
        if (size > 0)
            d->total_size -= size;
        vfs_unlink(path);
    }
}

/**
 * Create a file under \p dname, with file name pattern \p pattern. Before
 * file creation, the total size will be checked, and remove sequence or
 * index file if needed.
 */
static int prvFstraceCreateFile(
    srvFsTraceContext_t *d,
    const char *dname,
    const char *pattern,
    int curindex,
    bool remove_index)
{
    if (!prvFstraceCheckSeqSize(d))
    {
        if (!remove_index)
            return -1;
        if (!prvFstraceCheckIndexSize(d, dname, pattern, curindex))
            return -1;
    }

    char path[FSTRACE_FNAME_MAX];
    prvFstraceIndexFname(path, dname, pattern, curindex);

    if (vfs_mkfilepath(path, 0) != 0)
        return -1;

    return vfs_open(path, O_RDWR | O_CREAT | O_TRUNC);
}

/**
 * Check aplog file. When it exceeds threshold, close current file and
 * create a new one. \p prvFstraceCreateFile will be called to create
 * file, and total size will be checked in that.
 */
static bool prvFstraceCheckAplogFile(srvFsTraceContext_t *d)
{
    if (d->aplog_fd >= 0 && d->aplog_fsize < FSTRACE_APLOG_EACH_MAX)
        return true;

    if (d->aplog_fd >= 0)
    {
        vfs_close(d->aplog_fd);
        d->aplog_index++;
    }

    char path[FSTRACE_FNAME_MAX];
    sprintf(path, FSTRACE_APLOG_DIR_NAME_PATTERN, d->sequence);
    d->aplog_fd = prvFstraceCreateFile(d, path, FSTRACE_APLOG_FILE_PATTERN,
                                       d->aplog_index, true);
    d->aplog_fsize = 0;
    return (d->aplog_fd >= 0);
}

/**
 * Output aplog
 */
static int prvFstraceOutputAplog(srvFsTraceContext_t *d, unsigned option)
{
    if (!(d->option & SRV_FSTRACE_OPT_AP_ENABLE))
        return 0;

    if (d->aplog_fd < 0 || !prvFstraceCheckAplogFile(d))
        return 0;

    osiBuffer_t buf = osiTraceBufFetch();
    if (buf.size == 0)
        return 0;

    d->aplog_fsize += buf.size;
    vfs_write(d->aplog_fd, (void *)buf.ptr, buf.size);
    osiTraceBufHandled();

    if (option & FSTRACE_OUTPUT_FLUSH)
        vfs_fsync(d->aplog_fd);
    return buf.size;
}

/**
 * Check profilelog file, similar to \p prvFstraceCheckAplogFile.
 */
static bool prvFstraceCheckPrologFile(srvFsTraceContext_t *d)
{
    if (d->prolog_fd >= 0 && d->prolog_fsize < FSTRACE_PROLOG_EACH_MAX)
        return true;

    if (d->prolog_fd >= 0)
    {
        vfs_close(d->prolog_fd);
        d->prolog_index++;
    }

    char path[FSTRACE_FNAME_MAX];
    sprintf(path, FSTRACE_PROLOG_DIR_NAME_PATTERN, d->sequence);
    d->prolog_fd = prvFstraceCreateFile(d, path, FSTRACE_PROLOG_FILE_PATTERN,
                                        d->prolog_index, true);
    d->prolog_fsize = 0;
    return (d->prolog_fd >= 0);
}

/**
 * Output profile data
 */
static int prvFstraceOutputProlog(srvFsTraceContext_t *d, unsigned option)
{
    if (!(d->option & SRV_FSTRACE_OPT_PRO_ENABLE))
        return 0;

    if (d->prolog_fd < 0 || !prvFstraceCheckPrologFile(d))
        return 0;

    osiBuffer_t buf = osiProfileLinearBuf(d->prolog_pos);
    if (buf.size == 0)
        return 0;

    d->prolog_pos += buf.size;
    d->prolog_fsize += buf.size;
    vfs_write(d->prolog_fd, (void *)buf.ptr, buf.size);

    if (option & FSTRACE_OUTPUT_FLUSH)
        vfs_fsync(d->prolog_fd);
    return buf.size;
}

#ifdef FSTRACE_SUPPORT_CPLOG
/**
 * Check cplog file, similar to \p prvFstraceCheckAplogFile.
 */
static bool prvFstraceCheckCplogFile(srvFsTraceContext_t *d)
{
    if (d->cplog_fd >= 0 && d->cplog_fsize < FSTRACE_CPLOG_EACH_MAX)
        return true;

    if (d->cplog_fd >= 0)
    {
        vfs_close(d->cplog_fd);
        d->cplog_index++;
    }

    char path[FSTRACE_FNAME_MAX];
    sprintf(path, FSTRACE_CPLOG_DIR_NAME_PATTERN, d->sequence);
    d->cplog_fd = prvFstraceCreateFile(d, path, FSTRACE_CPLOG_FILE_PATTERN,
                                       d->cplog_index, true);
    d->cplog_fsize = 0;
    return (d->cplog_fd >= 0);
}

/**
 * Output cplog
 */
static int prvFstraceOutputCplog(srvFsTraceContext_t *d, unsigned option)
{
    if (!(d->option & (SRV_FSTRACE_OPT_CP_ENABLE | SRV_FSTRACE_OPT_ZSP_ENABLE)))
        return 0;

    if (d->cplog_fd < 0 || !prvFstraceCheckCplogFile(d))
        return 0;

    int size = 0;
    bool is_tail = false;
    osiBuffer_t buf;
    if (d->option & SRV_FSTRACE_OPT_CP_ENABLE)
    {
        buf = drvModemLogLinearBuf(&d->cplogbuf, &is_tail);
        if (buf.size > 0)
        {
            size += buf.size;
            d->cplog_fsize += buf.size;
            vfs_write(d->cplog_fd, (void *)buf.ptr, buf.size);
            drvModemLogBufAdvance(&d->cplogbuf, buf.size);
        }
    }

    if (d->option & SRV_FSTRACE_OPT_ZSP_ENABLE)
    {
        buf = drvModemLogLinearBuf(&d->zsplogbuf, &is_tail);
        if (buf.size > 0)
        {
            size += buf.size;
            d->cplog_fsize += buf.size;
            vfs_write(d->cplog_fd, (void *)buf.ptr, buf.size);
            drvModemLogBufAdvance(&d->zsplogbuf, buf.size);
        }
    }

    if (size > 0 && (option & FSTRACE_OUTPUT_FLUSH))
        vfs_fsync(d->cplog_fd);
    return size;
}
#endif

#ifdef FSTRACE_SUPPORT_MOSLOG
/**
 * Check moslog file, similar to \p prvFstraceCheckAplogFile.
 */
static bool prvFstraceCheckMoslogFile(srvFsTraceContext_t *d)
{
    if (d->moslog_fd >= 0 && d->moslog_fsize < FSTRACE_MOSLOG_EACH_MAX)
        return true;

    if (d->moslog_fd >= 0)
    {
        vfs_close(d->moslog_fd);
        d->moslog_index++;
    }

    char path[FSTRACE_FNAME_MAX];
    sprintf(path, FSTRACE_MOSLOG_DIR_NAME_PATTERN, d->sequence);
    d->moslog_fd = prvFstraceCreateFile(d, path, FSTRACE_MOSLOG_FILE_PATTERN,
                                        d->moslog_index, true);
    d->moslog_fsize = 0;
    return (d->moslog_fd >= 0);
}

/**
 * Output moslog
 */
static int prvFstraceOutputMoslog(srvFsTraceContext_t *d, unsigned option)
{
    if (!(d->option & SRV_FSTRACE_OPT_MOS_ENABLE))
        return 0;

    if (d->moslog_fd < 0 || !prvFstraceCheckMoslogFile(d))
        return 0;

    bool is_tail = false;
    osiBuffer_t buf = drvModemLogLinearBuf(&d->moslogbuf, &is_tail);
    if (buf.size == 0)
        return 0;

    d->moslog_fsize += buf.size;
    vfs_write(d->moslog_fd, (void *)buf.ptr, buf.size);
    drvModemLogBufAdvance(&d->moslogbuf, buf.size);

    if (option & FSTRACE_OUTPUT_FLUSH)
        vfs_fsync(d->moslog_fd);
    return buf.size;
}
#endif

/**
 * Output, to be called in work
 */
static void prvFstraceOutput(void *param)
{
    srvFsTraceContext_t *d = (srvFsTraceContext_t *)param;

    unsigned option = 0;
    int64_t curr = osiUpTime();
    if (curr - d->last_sync > FSTRACE_FSYNC_INTERVAL)
    {
        option |= FSTRACE_OUTPUT_FLUSH;
        d->last_sync = curr;
    }

    prvFstraceOutputAplog(d, option);
#ifdef FSTRACE_SUPPORT_CPLOG
    prvFstraceOutputCplog(d, option);
#endif
#ifdef FSTRACE_SUPPORT_MOSLOG
    prvFstraceOutputMoslog(d, option);
#endif
    prvFstraceOutputProlog(d, option);

    osiTimerStartRelaxed(d->tx_timer, FSTRACE_POLL_INTERVAL, OSI_DELAY_MAX);
}

/**
 * fstrace prepare
 */
static void prvFstracePrepare(void *param)
{
    srvFsTraceContext_t *d = (srvFsTraceContext_t *)param;

    d->total_size = vfs_dir_total_size(FSTRACE_DIR_NAME);

    int min_seq, max_seq;
    d->sequence = FSTRACE_SEQ_START;
    if (prvFstraceFindMinMaxSeq(d, FSTRACE_DIR_NAME, &min_seq, &max_seq,
                                FSTRACE_DIR_PATTERN) &&
        max_seq >= FSTRACE_SEQ_START)
        d->sequence = max_seq + 1;

    d->aplog_fd = -1;
    d->aplog_index = FSTRACE_SEQ_START;
    if (d->option & SRV_FSTRACE_OPT_AP_ENABLE)
        prvFstraceCheckAplogFile(d);

    d->prolog_fd = -1;
    d->prolog_index = FSTRACE_SEQ_START;
    if (d->option & SRV_FSTRACE_OPT_PRO_ENABLE)
        prvFstraceCheckPrologFile(d);

#ifdef FSTRACE_SUPPORT_CPLOG
    d->cplog_fd = -1;
    d->cplog_index = FSTRACE_SEQ_START;
    bool cplog_needed = false;
    if (d->option & SRV_FSTRACE_OPT_CP_ENABLE)
    {
        if (drvModemLogBufInitCp(&d->cplogbuf))
            cplog_needed = true;
    }
    if (d->option & SRV_FSTRACE_OPT_ZSP_ENABLE)
    {
        if (drvModemLogBufInitZsp(&d->zsplogbuf))
            cplog_needed = true;
    }
    if (cplog_needed)
        prvFstraceCheckCplogFile(d);
#endif

#ifdef FSTRACE_SUPPORT_MOSLOG
    d->moslog_fd = -1;
    d->moslog_index = FSTRACE_SEQ_START;
    if (d->option & SRV_FSTRACE_OPT_MOS_ENABLE)
    {
        if (drvModemLogBufInitMos(&d->moslogbuf))
            prvFstraceCheckMoslogFile(d);
    }
#endif

    d->prepared = true;
    osiWorkResetCallback(d->tx_work, prvFstraceOutput, NULL, d);
    osiTimerStartRelaxed(d->tx_timer, FSTRACE_POLL_INTERVAL, OSI_DELAY_MAX);
}

/**
 * Blue screen dump
 */
static void prvFstraceBsDump(srvFsTraceContext_t *d)
{
    if (!(d->option & SRV_FSTRACE_OPT_BS_ENABLE))
        return;

    const halBsDumpRegion_t *r = halHwBsDumpRegions();
    if (r == NULL)
        return;

    unsigned mask = HAL_BS_REGION_FLASH;
    if ((hwp_pwrctrl->zsp_pwr_stat & 0x3) == 0x3)
        mask |= HAL_BS_REGION_ZSP;
    if ((hwp_pwrctrl->gge_pwr_stat & 0x3) == 0x3)
        mask |= HAL_BS_REGION_GGE;
    if ((hwp_pwrctrl->btfm_pwr_stat & 0x3) == 0x3)
        mask |= HAL_BS_REGION_WCN;
    if ((hwp_pwrctrl->rf_pwr_stat & 0x3) == 0x3)
        mask |= HAL_BS_REGION_RFDIG;
    if ((hwp_pwrctrl->lte_pwr_stat & 0x3) == 0x3)
        mask |= HAL_BS_REGION_LTE;

    char fname[FSTRACE_FNAME_MAX];
    for (; !(r->property & HAL_BS_REGION_END); ++r)
    {
        if ((r->property & mask) != r->property)
            continue;

        OSI_LOGI(0, "fstrace bs dump 0x%08x/0x%x/0x%x", r->address, r->size, r->property);
        sprintf(fname, FSTRACE_BSDUMP_FILE_NAME_PATTERN, d->sequence, (unsigned)r->address);
        if (vfs_mkfilepath(fname, 0) != 0)
            break;

        if (HAL_ADDR_IS_ADI(r->address))
        {
            int fd = vfs_open(fname, O_RDWR | O_CREAT | O_TRUNC);
            if (fd < 0)
                continue;

            for (unsigned reg = r->address; reg < r->address + r->size; reg += 4)
            {
                unsigned val = halAdiBusRead((volatile uint32_t *)reg);
                vfs_write(fd, &val, 4);
            }
            vfs_close(fd);
        }
        else
        {
            vfs_file_write(fname, (void *)r->address, r->size);
        }
    }
}

/**
 * Callback at enter blue screen
 */
static void prvFstraceBsEnter(void *param)
{
    srvFsTraceContext_t *d = (srvFsTraceContext_t *)param;
    if (!d->prepared)
        return;

    OSI_LOGI(0, "fstrace bs enter");
    prvFstraceOutputAplog(d, FSTRACE_OUTPUT_FLUSH);
#ifdef FSTRACE_SUPPORT_CPLOG
    prvFstraceOutputCplog(d, FSTRACE_OUTPUT_FLUSH);
#endif
#ifdef FSTRACE_SUPPORT_MOSLOG
    prvFstraceOutputMoslog(d, FSTRACE_OUTPUT_FLUSH);
#endif
    prvFstraceOutputProlog(d, FSTRACE_OUTPUT_FLUSH);
    prvFstraceBsDump(d);
}

/**
 * Callback for enter blue polling
 */
static void prvFstraceBsPoll(void *param)
{
    srvFsTraceContext_t *d = (srvFsTraceContext_t *)param;
    if (!d->prepared)
        return;

    prvFstraceOutputAplog(d, FSTRACE_OUTPUT_FLUSH);
#ifdef FSTRACE_SUPPORT_CPLOG
    prvFstraceOutputCplog(d, FSTRACE_OUTPUT_FLUSH);
#endif
#ifdef FSTRACE_SUPPORT_MOSLOG
    prvFstraceOutputMoslog(d, FSTRACE_OUTPUT_FLUSH);
#endif
    prvFstraceOutputProlog(d, FSTRACE_OUTPUT_FLUSH);
}

/**
 * Initialize fstrace
 */
bool srvFstraceInit(unsigned option)
{
#ifndef FSTRACE_SUPPORT_CPLOG
    option &= ~(SRV_FSTRACE_OPT_CP_ENABLE | SRV_FSTRACE_OPT_ZSP_ENABLE);
#endif
#ifndef FSTRACE_SUPPORT_CPLOG
    option &= ~SRV_FSTRACE_OPT_MOS_ENABLE;
#endif

    if (option == 0)
        return true;

    // In case the sdcard isn't mounted, fstarce will write to
    // nor flash silently, and it should be avoided.
    if (vfs_mount_handle(FSTRACE_MOUNT_POINT) == NULL)
        return false;

    srvFsTraceContext_t *d = (srvFsTraceContext_t *)calloc(1, sizeof(srvFsTraceContext_t));
    if (d == NULL)
        return false;

    d->option = option;
    d->prepared = false;
    d->tx_wq = osiWorkQueueCreate("fstrace", 1, FSTRACE_THREAD_PRIORITY, FSTRACE_THREAD_STACK_SIZE);
    d->tx_work = osiWorkCreate(prvFstracePrepare, NULL, d);
    d->tx_timer = osiTimerCreateWork(d->tx_work, d->tx_wq);
    osiRegisterBlueScreenHandler(prvFstraceBsEnter, prvFstraceBsPoll, d);
    osiWorkEnqueue(d->tx_work, d->tx_wq);
    gFstraceCtx = d;
    return true;
}

#endif // CONFIG_FS_TRACE_ENABLE
