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

#include "kernel_config.h"
#include "drv_config.h"
#include "osi_log.h"
#include "osi_api.h"
#include "osi_internal.h"
#include "osi_chip.h"
#include "osi_trace.h"
#include "osi_fifo.h"
#include "hwregs.h"
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <alloca.h>

#define LOG_RAMCODE OSI_SECTION_LINE(.ramtext.osi_log)

#define TRACE_TDB_FLAG (1 << 31)
#define FMT_INT_MAY_LL(cc) (cc == 'd' || cc == 'i' || cc == 'o' || cc == 'u' || cc == 'x' || cc == 'X')
#define FMT_INT_NOT_LL(cc) (cc == 'c' || cc == 'p')
#define FMT_DOUBLE(cc) (cc == 'e' || cc == 'E' || cc == 'f' || cc == 'F' || cc == 'g' || cc == 'G' || cc == 'a' || cc == 'A')

/**
 * buffer description for 32bits parameter
 */
#define PUT_PARAM32                          \
    do                                       \
    {                                        \
        uint32_t val = va_arg(ap, uint32_t); \
        bufs->ptr = (uintptr_t)bufdata;      \
        bufs->size = 4;                      \
        dlen += bufs->size;                  \
        bufs++;                              \
        *bufdata++ = val;                    \
    } while (0)

/**
 * buffer description for 64bits parameter
 */
#define PUT_PARAM64                          \
    do                                       \
    {                                        \
        uint64_t val = va_arg(ap, uint64_t); \
        bufs->ptr = (uintptr_t)bufdata;      \
        bufs->size = 8;                      \
        dlen += bufs->size;                  \
        bufs++;                              \
        *bufdata++ = val & 0xffffffff;       \
        *bufdata++ = val >> 32;              \
    } while (0)

/**
 * buffer description for string parameter
 */
#define PUT_PARAMS                                  \
    do                                              \
    {                                               \
        const char *str = va_arg(ap, const char *); \
        if (str == NULL)                            \
            str = gLogNullString;                   \
        unsigned slen = strlen(str);                \
        bufs->ptr = (uintptr_t)str;                 \
        bufs->size = OSI_ALIGN_UP(slen + 1, 4);     \
        dlen += bufs->size;                         \
        bufs++;                                     \
    } while (0)

/**
 * buffer description for dump parameter
 */
#define PUT_PARAMM                                        \
    do                                                    \
    {                                                     \
        unsigned size = va_arg(ap, uint32_t);             \
        const uint8_t *mem = va_arg(ap, const uint8_t *); \
        bufs->ptr = (uintptr_t)bufdata;                   \
        bufs->size = 6;                                   \
        dlen += bufs->size;                               \
        bufs++;                                           \
        *bufdata++ = (unsigned)mem;                       \
        *bufdata++ = size;                                \
        bufs->ptr = (uintptr_t)mem;                       \
        bufs->size = OSI_ALIGN_UP(size + 6, 4) - 6;       \
        dlen += bufs->size;                               \
        bufs++;                                           \
    } while (0)

/**
 * Legacy SX trace control. It is defined in sx_api.h, and put the
 * definition here to simplify module dependency.
 */
typedef struct
{
    uint32_t *fn_stamp;
    uint16_t TraceBitMap[32];
} sxs_IoCtx_t;

/**
 * Application trace (big) header
 */
typedef struct
{
#ifdef CONFIG_KERNEL_HOST_TRACE
    osiHostPacketHeader_t host;
    uint16_t sn;   ///< trace sequence
    uint16_t tick; ///< tick of trace
    uint32_t tag;  ///< application tag
#endif
#ifdef CONFIG_KERNEL_DIAG_TRACE
    osiDiagPacketHeader_t diag;
    osiDiagLogHeader_t log;
    uint32_t tick; ///< tick of trace
    uint32_t tag;  ///< application tag
#endif
} osiTraceHeader_t;

/**
 * tra trace (big) header
 */
typedef struct
{
#ifdef CONFIG_KERNEL_HOST_TRACE
    osiHostPacketHeader_t host;
    osiTraPacketHeader_t tra;
#endif
#ifdef CONFIG_KERNEL_DIAG_TRACE
    osiDiagPacketHeader_t diag;
    osiDiagLogHeader_t log;
    osiTraPacketHeader_t tra;
#endif
} osiTraHeader_t;

typedef struct
{
    unsigned count;
    unsigned bufsize;
} osiTraceParamInfo_t;

static const char *gLogNullString = "(null)";
bool gTraceEnabled = false;
uint32_t gTraceSequence;
sxs_IoCtx_t sxs_IoCtx;
uint32_t v_tra_pubModuleControl;
uint32_t v_tra_lteModuleControl;
uint32_t v_tra_categoryControl;

/**
 * Tick value for trace
 */
static inline uint32_t prvTraceTick(void)
{
#ifdef CONFIG_SOC_8910
    return hwp_idle->idl_32k_ref >> 1;
#else
    return HAL_TIMER_16K_CURVAL;
#endif
}

/**
 * GSM frame number
 */
uint32_t osiTraceGsmFrameNumber(void)
{
    return (sxs_IoCtx.fn_stamp == NULL) ? 0 : *(sxs_IoCtx.fn_stamp);
}

/**
 * LTE frame number
 */
OSI_WEAK uint32_t osiTraceLteFrameNumber(void)
{
    return 0;
}

/**
 * Fill application trace header
 */
static OSI_FORCE_INLINE void prvFillTraceHeader(osiTraceHeader_t *h, unsigned tag, unsigned tlen)
{
#ifdef CONFIG_KERNEL_HOST_TRACE
    h->host.sync = 0xad;
    h->host.frame_len_msb = (tlen - 4) >> 8;
    h->host.frame_len_lsb = (tlen - 4) & 0xff;
    h->host.flowid = 0x98;
    h->sn = gTraceSequence;
    h->tag = tag;
    h->tick = prvTraceTick();
#endif

#ifdef CONFIG_KERNEL_DIAG_TRACE
    h->diag.seq_num = gTraceSequence;
    h->diag.len = tlen;
    h->diag.type = 0x98;
    h->diag.subtype = 0x00;
    h->log.type = 0x9198;
    h->log.length = tlen - OSI_OFFSETOF(osiTraceHeader_t, log);
    h->tag = tag;
    h->tick = prvTraceTick();
#endif
}

/**
 * Fill tra trace header
 */
static OSI_FORCE_INLINE void prvFillTraHeader(osiTraHeader_t *h, uint8_t type, unsigned tlen)
{
#ifdef CONFIG_KERNEL_HOST_TRACE
    h->host.sync = 0xad;
    h->host.frame_len_msb = (tlen - 4) >> 8;
    h->host.frame_len_lsb = (tlen - 4) & 0xff;
    h->host.flowid = 0x96;
    h->tra.sync = 0xbbbb;
    h->tra.plat_id = 1;
    h->tra.type = type;
    h->tra.sn = gTraceSequence;
    h->tra.fn_wcdma = prvTraceTick();
    h->tra.fn_gge = osiTraceGsmFrameNumber();
    h->tra.fn_lte = osiTraceLteFrameNumber();
#endif

#ifdef CONFIG_KERNEL_DIAG_TRACE
    h->diag.seq_num = gTraceSequence;
    h->diag.len = tlen;
    h->diag.type = 0x98;
    h->diag.subtype = 0x00;
    h->log.type = 0x9196;
    h->log.length = tlen - OSI_OFFSETOF(osiTraceHeader_t, log);
    h->tra.sync = 0xbbbb;
    h->tra.plat_id = 1;
    h->tra.type = type;
    h->tra.sn = gTraceSequence;
    h->tra.fn_wcdma = prvTraceTick();
    h->tra.fn_gge = osiTraceGsmFrameNumber();
    h->tra.fn_lte = osiTraceLteFrameNumber();
#endif
}

/**
 * Get param count by partype, LOGPAR_M takes 2.
 */
static osiTraceParamInfo_t prvParamCount(unsigned partype)
{
    static unsigned parcount[] = {0, 1, 1, 1, 1, 2};
    static unsigned parbufsize[] = {0, 4, 8, 8, 0, 8};
    osiTraceParamInfo_t pari = {0, 0};
    while (partype != 0)
    {
        uint8_t pt = (partype & 0xf);
        partype >>= 4;

        pari.count += parcount[pt];
        pari.bufsize += parbufsize[pt];
    }
    return pari;
}

/**
 * Collect param buffers, return data length.
 */
static unsigned prvParamBuf(unsigned partype, osiBuffer_t *bufs, uint32_t *bufdata, va_list ap)
{
    unsigned dlen = 0;
    while (partype != 0)
    {
        uint8_t pt = (partype & 0xf);
        partype >>= 4;

        if (pt == __OSI_LOGPAR_D || pt == __OSI_LOGPAR_F)
            PUT_PARAM64;
        else if (pt == __OSI_LOGPAR_S)
            PUT_PARAMS;
        else if (pt == __OSI_LOGPAR_M)
            PUT_PARAMM;
        else
            PUT_PARAM32;
    }

    return dlen;
}

/**
 * Get param count by partype, LOGPAR_M takes 2.
 */
static osiTraceParamInfo_t prvStrParamCount(const char *fmt)
{
    osiTraceParamInfo_t pari = {0, 0};
    for (;;)
    {
        char c = *fmt++;
        if (c == '\0')
            break;
        if (c != '%')
            continue;

        for (;;)
        {
            char cc = *fmt++;
            if (FMT_INT_MAY_LL(cc))
            {
                pari.count += 1;
                if (fmt[-2] == 'l' && fmt[-3] == 'l')
                    pari.bufsize += 8;
                else
                    pari.bufsize += 4;
                break;
            }
            else if (FMT_INT_NOT_LL(cc))
            {
                pari.count += 1;
                pari.bufsize += 4;
                break;
            }
            else if (FMT_DOUBLE(cc))
            {
                pari.count += 1;
                pari.bufsize += 8;
                break;
            }
            else if (cc == 's')
            {
                if (fmt[-2] == '*')
                {
                    pari.count += 2;
                    pari.bufsize += 8;
                }
                else
                {
                    pari.count += 1;
                }
                break;
            }
            else if (cc == '%')
            {
                break;
            }
            else if (cc == '\0')
            {
                break;
            }
            else
            {
                continue;
            }
        }
    }
    return pari;
}

/**
 * Collect param buffers, return data length.
 */
static unsigned prvStrParamBuf(const char *fmt, osiBuffer_t *bufs, uint32_t *bufdata, va_list ap)
{
    unsigned dlen = 0;
    for (;;)
    {
        char c = *fmt++;
        if (c == '\0')
            break;
        if (c != '%')
            continue;

        for (;;)
        {
            char cc = *fmt++;
            if (FMT_INT_MAY_LL(cc))
            {
                if (fmt[-2] == 'l' && fmt[-3] == 'l')
                    PUT_PARAM64;
                else
                    PUT_PARAM32;
                break;
            }
            else if (FMT_INT_NOT_LL(cc))
            {
                PUT_PARAM32;
                break;
            }
            else if (FMT_DOUBLE(cc))
            {
                PUT_PARAM64;
                break;
            }
            else if (cc == 's')
            {
                if (fmt[-2] == '*')
                    PUT_PARAMM;
                else
                    PUT_PARAMS;
                break;
            }
            else if (cc == '%')
            {
                break;
            }
            else if (cc == '\0')
            {
                break;
            }
            else
            {
                continue;
            }
        }
    }
    return dlen;
}

/**
 * Application basic trace
 */
LOG_RAMCODE static void prvTraceBasic(unsigned tag, unsigned nargs, const char *fmt, va_list ap)
{
    osiTraceHeader_t header;
    unsigned fmt_len = strlen(fmt);
    unsigned fmt_len_aligned = OSI_ALIGN_UP(fmt_len + 1, 4);
    unsigned dlen = nargs * 4;
    unsigned tlen = sizeof(header) + fmt_len_aligned + dlen;
    uint32_t *buf = (uint32_t *)alloca(dlen);
    osiBuffer_t bufs[3] = {
        {(uintptr_t)&header, sizeof(header)},
        {(uintptr_t)fmt, fmt_len_aligned},
        {(uintptr_t)buf, dlen},
    };

    for (unsigned n = 0; n < nargs; n++)
        *buf++ = va_arg(ap, uint32_t);

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraceHeader(&header, tag, tlen);
    osiTraceBufPutMulti(bufs, 3, tlen);
    osiExitCritical(critical);
}

/**
 * Application basic trace, with ID
 */
LOG_RAMCODE static void prvTraceIdBasic(unsigned tag, unsigned nargs, unsigned fmtid, va_list ap)
{
    unsigned dlen = 4 + 4 * nargs;
    unsigned tlen = sizeof(osiTraceHeader_t) + dlen;
    uint32_t *p = (uint32_t *)alloca(tlen);
    uint32_t *buf = p + (sizeof(osiTraceHeader_t) / 4);
    *buf++ = fmtid;
    for (unsigned n = 0; n < nargs; n++)
        *buf++ = va_arg(ap, uint32_t);

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraceHeader((osiTraceHeader_t *)p, tag | TRACE_TDB_FLAG, tlen);
    osiTraceBufPut(p, tlen);
    osiExitCritical(critical);
}

/**
 * Application extended trace
 */
static void prvTraceEx(unsigned tag, unsigned partype, const char *fmt, va_list ap)
{
    osiTraceHeader_t header;
    unsigned fmt_len = strlen(fmt);
    unsigned fmt_len_aligned = OSI_ALIGN_UP(fmt_len + 1, 4);
    osiTraceParamInfo_t pari = prvParamCount(partype);
    osiBuffer_t *bufs = (osiBuffer_t *)alloca((pari.count + 2) * sizeof(osiBuffer_t));
    bufs[0].ptr = (uintptr_t)&header;
    bufs[0].size = sizeof(header);
    bufs[1].ptr = (uintptr_t)fmt;
    bufs[1].size = fmt_len_aligned;
    uint32_t *bufdata = (uint32_t *)alloca(pari.bufsize);
    unsigned dlen = prvParamBuf(partype, &bufs[2], bufdata, ap);
    unsigned tlen = sizeof(header) + fmt_len_aligned + dlen;

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraceHeader(&header, tag, tlen);
    osiTraceBufPutMulti(bufs, pari.count + 2, tlen);
    osiExitCritical(critical);
}

/**
 * Application extended trace, with ID
 */
static void prvTraceIdEx(unsigned tag, unsigned partype, unsigned fmtid, va_list ap)
{
    osiTraceHeader_t header;
    osiTraceParamInfo_t pari = prvParamCount(partype);
    osiBuffer_t *bufs = alloca((pari.count + 2) * sizeof(osiBuffer_t));
    bufs[0].ptr = (uintptr_t)&header;
    bufs[0].size = sizeof(header);
    bufs[1].ptr = (uintptr_t)&fmtid;
    bufs[1].size = 4;
    uint32_t *bufdata = alloca(pari.bufsize);
    unsigned dlen = prvParamBuf(partype, &bufs[2], bufdata, ap);
    unsigned tlen = sizeof(header) + 4 + dlen;

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraceHeader(&header, tag | TRACE_TDB_FLAG, tlen);
    osiTraceBufPutMulti(bufs, pari.count + 2, tlen);
    osiExitCritical(critical);
}

/**
 * Application basic trace
 */
LOG_RAMCODE void osiTraceBasic(unsigned tag, unsigned nargs, const char *fmt, ...)
{
    if (!gTraceEnabled)
        return;

    va_list ap;
    va_start(ap, fmt);
    prvTraceBasic(tag, nargs, fmt, ap);
    va_end(ap);
}

/**
 * Application basic trace, with ID
 */
LOG_RAMCODE void osiTraceIdBasic(unsigned tag, unsigned nargs, unsigned fmtid, ...)
{
    if (!gTraceEnabled)
        return;

    va_list ap;
    va_start(ap, fmtid);
    prvTraceIdBasic(tag, nargs, fmtid, ap);
    va_end(ap);
}

/**
 * Application extended trace
 */
void osiTraceEx(unsigned tag, unsigned partype, const char *fmt, ...)
{
    if (!gTraceEnabled)
        return;

    va_list ap;
    va_start(ap, fmt);
    prvTraceEx(tag, partype, fmt, ap);
    va_end(ap);
}

/**
 * Application extended trace, with ID
 */
void osiTraceIdEx(unsigned tag, unsigned partype, unsigned fmtid, ...)
{
    if (!gTraceEnabled)
        return;

    va_list ap;
    va_start(ap, fmtid);
    prvTraceIdEx(tag, partype, fmtid, ap);
    va_end(ap);
}

/**
 * Application trace printf
 */
void osiTraceVprintf(unsigned tag, const char *fmt, va_list ap)
{
    if (!gTraceEnabled)
        return;

    osiTraceHeader_t header;
    unsigned fmt_len = strlen(fmt);
    unsigned fmt_len_aligned = OSI_ALIGN_UP(fmt_len + 1, 4);
    osiTraceParamInfo_t pari = prvStrParamCount(fmt);
    osiBuffer_t *bufs = alloca((pari.count + 2) * sizeof(osiBuffer_t));
    bufs[0].ptr = (uintptr_t)&header;
    bufs[0].size = sizeof(header);
    bufs[1].ptr = (uintptr_t)fmt;
    bufs[1].size = fmt_len_aligned;
    uint32_t *bufdata = (uint32_t *)alloca(pari.bufsize);
    unsigned dlen = prvStrParamBuf(fmt, &bufs[2], bufdata, ap);
    unsigned tlen = sizeof(header) + fmt_len_aligned + dlen;

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraceHeader(&header, tag, tlen);
    osiTraceBufPutMulti(bufs, pari.count + 2, tlen);
    osiExitCritical(critical);
}

/**
 * Application trace printf
 */
void osiTracePrintf(unsigned tag, const char *fmt, ...)
{
    if (!gTraceEnabled)
        return;

    va_list ap;
    va_start(ap, fmt);
    osiTraceVprintf(tag, fmt, ap);
    va_end(ap);
}

/**
 * tra basic trace
 */
LOG_RAMCODE static void prvTraBasic(unsigned nargs, const char *fmt, va_list ap)
{
    osiTraHeader_t header;
    unsigned fmt_len = strlen(fmt);
    unsigned fmt_len_aligned = OSI_ALIGN_UP(fmt_len + 1, 4);
    unsigned dlen = nargs * 4;
    unsigned tlen = sizeof(header) + fmt_len_aligned + dlen;
    uint32_t *buf = (uint32_t *)alloca(dlen);
    osiBuffer_t bufs[3] = {
        {(uintptr_t)&header, sizeof(header)},
        {(uintptr_t)fmt, fmt_len_aligned},
        {(uintptr_t)buf, dlen},
    };

    for (uint32_t n = 0; n < nargs; n++)
        *buf++ = va_arg(ap, uint32_t);

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraHeader(&header, 0xc9, tlen);
    osiTraceBufPutMulti(bufs, 3, tlen);
    osiExitCritical(critical);
}

/**
 * tra basic trace, with ID
 */
LOG_RAMCODE static void prvTraIdBasic(unsigned nargs, unsigned fmtid, va_list ap)
{
    unsigned dlen = 4 + 4 * nargs;
    unsigned tlen = sizeof(osiTraHeader_t) + dlen;
    uint32_t *p = alloca(tlen);
    uint32_t *buf = p + (sizeof(osiTraHeader_t) / 4);
    *buf++ = fmtid;
    for (uint32_t n = 0; n < nargs; n++)
        *buf++ = va_arg(ap, uint32_t);

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraHeader((osiTraHeader_t *)p, 0xc8, tlen);
    osiTraceBufPut(p, tlen);
    osiExitCritical(critical);
}

/**
 * tra extended trace
 */
static void prvTraEx(unsigned partype, const char *fmt, va_list ap)
{
    osiTraHeader_t header;
    uint32_t fmt_len = strlen(fmt);
    uint32_t fmt_len_aligned = OSI_ALIGN_UP(fmt_len + 1, 4);
    osiTraceParamInfo_t pari = prvParamCount(partype);
    osiBuffer_t *bufs = (osiBuffer_t *)alloca((pari.count + 2) * sizeof(osiBuffer_t));
    bufs[0].ptr = (uintptr_t)&header;
    bufs[0].size = sizeof(header);
    bufs[1].ptr = (uintptr_t)fmt;
    bufs[1].size = fmt_len_aligned;
    uint32_t *bufdata = (uint32_t *)alloca(pari.bufsize);
    uint32_t dlen = prvParamBuf(partype, &bufs[2], bufdata, ap);
    unsigned tlen = sizeof(header) + fmt_len_aligned + dlen;

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraHeader(&header, 0xc9, tlen);
    osiTraceBufPutMulti(bufs, pari.count + 2, tlen);
    osiExitCritical(critical);
}

/**
 * tra extended trace, with ID
 */
static void prvTraIdEx(unsigned partype, unsigned fmtid, va_list ap)
{
    osiTraHeader_t header;
    osiTraceParamInfo_t pari = prvParamCount(partype);
    osiBuffer_t *bufs = (osiBuffer_t *)alloca((pari.count + 2) * sizeof(osiBuffer_t));
    bufs[0].ptr = (uintptr_t)&header;
    bufs[0].size = sizeof(header);
    bufs[1].ptr = (uintptr_t)&fmtid;
    bufs[1].size = 4;
    uint32_t *bufdata = (uint32_t *)alloca(pari.bufsize);
    unsigned dlen = prvParamBuf(partype, &bufs[2], bufdata, ap);
    unsigned tlen = sizeof(header) + 4 + dlen;

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraHeader(&header, 0xc8, tlen);
    osiTraceBufPutMulti(bufs, pari.count + 2, tlen);
    osiExitCritical(critical);
}

/**
 * tra basic trace without module and category
 */
LOG_RAMCODE void osiTraceTraBasic(unsigned nargs, const char *fmt, ...)
{
    if (!gTraceEnabled)
        return;

    va_list ap;
    va_start(ap, fmt);
    prvTraBasic(nargs, fmt, ap);
    va_end(ap);
}

/**
 * tra pub basic trace
 */
LOG_RAMCODE void osiTracePubBasic(unsigned module, unsigned category, unsigned nargs, const char *fmt, ...)
{
    if (!gTraceEnabled ||
        !(module & v_tra_pubModuleControl) ||
        !(category & v_tra_categoryControl))
        return;

    va_list ap;
    va_start(ap, fmt);
    prvTraBasic(nargs, fmt, ap);
    va_end(ap);
}

/**
 * tra lte basic trace
 */
LOG_RAMCODE void osiTraceLteBasic(unsigned module, unsigned category, unsigned nargs, const char *fmt, ...)
{
    if (!gTraceEnabled ||
        !(module & v_tra_lteModuleControl) ||
        !(category & v_tra_categoryControl))
        return;

    va_list ap;
    va_start(ap, fmt);
    prvTraBasic(nargs, fmt, ap);
    va_end(ap);
}

/**
 * tra basic trace without module and category, with ID
 */
LOG_RAMCODE void osiTraceTraIdBasic(unsigned nargs, unsigned fmtid, ...)
{
    if (!gTraceEnabled)
        return;

    va_list ap;
    va_start(ap, fmtid);
    prvTraIdBasic(nargs, fmtid, ap);
    va_end(ap);
}

/**
 * tra pub basic trace, with ID
 */
LOG_RAMCODE void osiTracePubIdBasic(unsigned module, unsigned category, unsigned nargs, unsigned fmtid, ...)
{
    if (!gTraceEnabled ||
        !(module & v_tra_pubModuleControl) ||
        !(category & v_tra_categoryControl))
        return;

    va_list ap;
    va_start(ap, fmtid);
    prvTraIdBasic(nargs, fmtid, ap);
    va_end(ap);
}

/**
 * tra lte basic trace, with ID
 */
LOG_RAMCODE void osiTraceLteIdBasic(unsigned module, unsigned category, unsigned nargs, unsigned fmtid, ...)
{
    if (!gTraceEnabled ||
        !(module & v_tra_lteModuleControl) ||
        !(category & v_tra_categoryControl))
        return;

    va_list ap;
    va_start(ap, fmtid);
    prvTraIdBasic(nargs, fmtid, ap);
    va_end(ap);
}

/**
 * tra extended trace without module and category
 */
void osiTraceTraEx(unsigned partype, const char *fmt, ...)
{
    if (!gTraceEnabled)
        return;

    va_list apl;
    va_start(apl, fmt);
    prvTraEx(partype, fmt, apl);
    va_end(apl);
}

/**
 * tra pub extended trace
 */
void osiTracePubEx(unsigned module, unsigned category, unsigned partype, const char *fmt, ...)
{
    if (!gTraceEnabled ||
        !(module & v_tra_pubModuleControl) ||
        !(category & v_tra_categoryControl))
        return;

    va_list apl;
    va_start(apl, fmt);
    prvTraEx(partype, fmt, apl);
    va_end(apl);
}

/**
 * tra lte extended trace
 */
void osiTraceLteEx(unsigned module, unsigned category, unsigned partype, const char *fmt, ...)
{
    if (!gTraceEnabled ||
        !(module & v_tra_lteModuleControl) ||
        !(category & v_tra_categoryControl))
        return;

    va_list apl;
    va_start(apl, fmt);
    prvTraEx(partype, fmt, apl);
    va_end(apl);
}

/**
 * tra extended trace without module and category, with ID
 */
void osiTraceTraIdEx(unsigned partype, unsigned fmtid, ...)
{
    if (!gTraceEnabled)
        return;

    va_list apl;
    va_start(apl, fmtid);
    prvTraIdEx(partype, fmtid, apl);
    va_end(apl);
}

/**
 * tra pub extended trace, with ID
 */
void osiTracePubIdEx(unsigned module, unsigned category, unsigned partype, unsigned fmtid, ...)
{
    if (!gTraceEnabled ||
        !(module & v_tra_pubModuleControl) ||
        !(category & v_tra_categoryControl))
        return;

    va_list apl;
    va_start(apl, fmtid);
    prvTraIdEx(partype, fmtid, apl);
    va_end(apl);
}

/**
 * tra lte extended trace, with ID
 */
void osiTraceLteIdEx(unsigned module, unsigned category, unsigned partype, unsigned fmtid, ...)
{
    if (!gTraceEnabled ||
        !(module & v_tra_lteModuleControl) ||
        !(category & v_tra_categoryControl))
        return;

    va_list apl;
    va_start(apl, fmtid);
    prvTraIdEx(partype, fmtid, apl);
    va_end(apl);
}

/**
 * send tra trace packet data. bufs[0] is reserved for header.
 */
void osiTraceSendTraData(uint8_t type, osiBuffer_t *bufs, unsigned count, unsigned dlen)
{
    if (!gTraceEnabled)
        return;

    osiTraHeader_t header;
    bufs[0].ptr = (uintptr_t)&header;
    bufs[0].size = sizeof(header);
    unsigned tlen = sizeof(header) + dlen;

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraHeader(&header, type, tlen);
    osiTraceBufPutMulti(bufs, count, tlen);
    osiExitCritical(critical);
}

/**
 * Set pointer of GSM frame number, only for SX trace
 */
void sxs_SetFnStamp(uint32_t *fn_stamp)
{
    sxs_IoCtx.fn_stamp = fn_stamp;
}

/**
 * Set SX trace level control
 */
void sxs_SetTraceLevel(uint8_t Id, uint16_t LevelBitMap)
{
    sxs_IoCtx.TraceBitMap[Id & 0x1f] = LevelBitMap;
}

/**
 * It is called in BT log, and should be redesigned.
 */
void osiTraceRawSend(uint8_t flowid, unsigned tag, const void *data, uint32_t len)
{
#ifdef CONFIG_KERNEL_HOST_TRACE
    osiTraceHeader_t header;
    osiBuffer_t bufs[2] = {
        {(uintptr_t)&header, sizeof(header)},
        {(uintptr_t)data, len},
    };
    unsigned tlen = sizeof(header) + len;

    unsigned critical = osiEnterCritical();
    gTraceSequence++;
    prvFillTraceHeader(&header, tag, tlen);
    header.host.flowid = flowid;
    osiTraceBufPutMulti(bufs, 2, tlen);
    osiExitCritical(critical);
#endif
}

/**
 * It is called in BT log, and should be redesigned.
 */
void SCI_TraceCapData(unsigned data_type, const void *src_ptr, uint32_t size)
{
    osiTraceRawSend(0x70, data_type, src_ptr, size);
}
