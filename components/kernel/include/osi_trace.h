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

#ifndef _OSI_TRACE_H_
#define _OSI_TRACE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "osi_api.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration for debug port
 */
struct drvDebugPort;

/**
 * \brief trace output device
 */
typedef enum
{
    OSI_TRACE_DEVICE_NONE = 0,         ///< no output
    OSI_TRACE_DEVICE_DEBUGHOST = 0x01, ///< output through debughost
    OSI_TRACE_DEVICE_USBSERIAL = 0x02, ///< output through USB serial
    OSI_TRACE_DEVICE_FILESYS = 0x04,   ///< output to file system, usually on SDCARD
} osiTraceDevice_t;

/**
 * \brief data structure of host packet
 *
 * *frame length* is the length of host packet, *exclude* header. So,
 * frame length is the packet length minus 4.
 */
typedef struct
{
    uint8_t sync;          ///< sync byte, 0xAD
    uint8_t frame_len_msb; ///< MSB byte of frame length
    uint8_t frame_len_lsb; ///< LSB byte of frame length
    uint8_t flowid;        ///< flow ID
} osiHostPacketHeader_t;

/**
 * \brief tra packet header data structure
 */
typedef struct
{
    uint16_t sync;     ///< sync 16bits word, 0xBBBB
    uint8_t plat_id;   ///< platform id, fill 1 at unsure.
    uint8_t type;      ///< tra packet type
    uint32_t sn;       ///< serial number
    uint32_t fn_wcdma; ///< WCDMA frame number, fill tick count when there are no WCDMA
    uint32_t fn_gge;   ///< GSM frame number
    uint32_t fn_lte;   ///< LTE frame number
} osiTraPacketHeader_t;

/**
 * \brief diag packet header
 */
typedef struct
{
    uint32_t seq_num; ///< Message sequence number, used for flow control
    uint16_t len;     ///< The totoal size of the packet
    uint8_t type;     ///< Main command type
    uint8_t subtype;  ///< Sub command type
} osiDiagPacketHeader_t;

/**
 * \brief diag log packet header
 */
typedef struct
{
    uint16_t type;   ///< log type
    uint16_t length; ///< log length
} osiDiagLogHeader_t;

/**
 * \brief global trace sequence number
 *
 * *Don't* modify it, unless in trace output engine.
 */
extern uint32_t gTraceSequence;

/**
 * \brief global variable for trace enable/disable
 *
 * *Don't* modify it. It is only for trace API for faster check,
 * to save a little cycles than calling API.
 */
extern bool gTraceEnabled;

/**
 * \brief set trace enable or not
 *
 * After disable, trace calls won't put trace data to trace buffer. Also,
 * debug port will stop trace output (after the already started transfer
 * finished).
 *
 * \param enable false for disable, true for enable
 */
void osiTraceSetEnable(bool enable);

/**
 * \brief get GSM frame number
 *
 * When GSM frame number is needed to be embedded into trace, this function
 * should be implemented outside trace module.
 *
 * \return GSM frame number
 */
uint32_t osiTraceGsmFrameNumber(void);

/**
 * \brief get LTE frame number
 *
 * When LTE frame number is needed to be embedded into trace, this function
 * should be implemented outside trace module.
 *
 * \return GSM frame number
 */
uint32_t osiTraceLteFrameNumber(void);

/**
 * \brief fill host packet header
 *
 * It is just a helper to fill host packet header.
 *
 * \param [in] header host packet header
 * \param [in] flowid host packet flowid
 * \param [in] frame_len host packet frame length
 */
static inline void osiFillHostHeader(osiHostPacketHeader_t *header, uint8_t flowid, uint16_t frame_len)
{
    header->sync = 0xAD;
    header->frame_len_msb = frame_len >> 8;
    header->frame_len_lsb = frame_len & 0xff;
    header->flowid = flowid;
}

/**
 * \brief trace buffer initialization
 *
 * It should be called only once at system boot. When this is called,
 * it is possible that RTOS hasn't initialized.
 */
void osiTraceBufInit(void);

/**
 * \brief put a trace packet into trace buffer
 *
 * Inside, packet data may be copied to global trace buffer or malloc-copy
 * to dynamic memory, depends on the packet size. After the call, \p data
 * won't be used any more.
 *
 * When diag trace is used, this is the real packet, rather than hdlc
 * encoded packet. Hdlc encoding will be performed inside.
 *
 * \param data packet data
 * \param size packet total size
 * \return
 *      - true on success
 *      - false on error, invalid paramters or trace buffer full
 */
bool osiTraceBufPut(const void *data, unsigned size);

/**
 * \brief put a trace packet into trace buffer
 *
 * It is similar to \p osiTraceBufPut, just the packet is described by
 * multiple pieces of buffer. It can save time for caller to combine them.
 *
 * When \p size is -1, the total size will be calculated. Otherwise it will be
 * used as total size. Total size will only be used to check whether space is
 * enough, and caller should ensure \p size matches the total size.
 *
 * \param bufs buffers of packet data
 * \param count buffer count
 * \param size total size, -1 means callee should calculate total size
 * \return
 *      - true on success
 *      - false on error, invalid paramters or trace buffer full
 */
bool osiTraceBufPutMulti(const osiBuffer_t *bufs, unsigned count, int size);

/**
 * \brief put an external trace packet into trace buffer
 *
 * Put a "reference" of packet to trace output. The data won't be copied,
 * so \p data will still be accessed after this returns. Only after
 * \p osiTraceBufPacketFinished returns true, the packet has been transfered,
 * and won't be accessed any more.
 *
 * For host packet, \p size should be 4 bytes aligned.
 *
 * For diag packet, it is hdlc encoded packet, with leading and trailing
 * sync byte.
 *
 * There is count limit of raw packets. When there are too many raw packets
 * are pending, it will return false.
 *
 * \param data raw packet data
 * \param size raw packet total size
 * \return
 *      - true on success
 *      - false on error, invalid paramters or too many raw packets
 */
bool osiTraceBufPutPacket(const void *data, unsigned size);

/**
 * \brief whether the external trace packet is transfered
 *
 * \p data must be the pointer called by \p osiTraceBufPutPacket.
 *
 * \param data raw packet data
 * \return true if transfer done
 */
bool osiTraceBufPacketFinished(const void *data);

/**
 * \brief fetch a piece of trace buffer
 *
 * When trace buffer is empty, the returned size will be 0.
 *
 * The returned buffer may contain one or more packets, and it won't
 * contain partial packet.
 *
 * After \p osiTraceBufFetch is called, it is needed to call
 * \p osiTraceBufHandled before next call.
 *
 * It should be called by trace output only.
 *
 * \return buffer with trace data
 */
osiBuffer_t osiTraceBufFetch(void);

/**
 * \brief indicate trace buffer is handled
 *
 * It should be called by trace output only.
 */
void osiTraceBufHandled(void);

/**
 * \brief get existed trace data size
 *
 * In most cases, this shouldn't be cared. One excception is in usb trace,
 * when the remaining trace data is smaller than the threshold, usb
 * transfer should wait a while.
 *
 * \return existed trace data size
 */
unsigned osiTraceDataSize(void);

/**
 * \brief set trace debug port
 *
 * \p drvDebugPortSetTraceEnable will be called when the current enabled
 * state changed.
 *
 * \param port debug port for trace
 */
void osiTraceSetDebugPort(struct drvDebugPort *port);

OSI_EXTERN_C_END
#endif
