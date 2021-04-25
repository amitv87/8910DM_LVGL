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

#ifndef _DRV_DEBUG_PORT_H_
#define _DRV_DEBUG_PORT_H_

#include "osi_api.h"
#include "drv_names.h"
#include <sys/queue.h>

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration
 */
struct drvDebugPort;

/**
 * \brief debug port operations
 *
 * It is by design that there are no delete API. All debug port instances
 * doesn't support delete.
 */
typedef struct
{
    /** refer to \p drvDebugPortSendPacket */
    bool (*send_packet)(struct drvDebugPort *d, const void *data, unsigned size);
    /** refer to \p drvDebugPortRead */
    int (*read)(struct drvDebugPort *d, void *data, unsigned size);
    /** refer to \p drvDebugPortSetRxCallback  */
    void (*set_rx_callback)(struct drvDebugPort *d, osiCallback_t cb, void *param);
    /** refer to \p drvDebugPortSetTraceEnable */
    void (*set_trace_enable)(struct drvDebugPort *d, bool enable);
} drvDebugPortOps_t;

/**
 * \brief debug port protocol
 */
typedef enum
{
    DRV_DEBUG_PROTOCOL_HOST = 1,         ///< host protocol, trace and command
    DRV_DEBUG_PROTOCOL_DIAG,             ///< diag protocol, trace and command
    DRV_DEBUG_PROTOCOL_MODEM_LOG = 0x10, ///< modem log
    DRV_DEBUG_PROTOCOL_MOS_LOG,          ///< mos log
} drvDebugPortProtocol_t;

/**
 * \brief debug port working mode
 */
typedef union {
    struct
    {
        uint8_t protocol;      ///< refer to \p drvDebugPortProtocol_t
        bool trace_enable : 1; ///< whether trace is enabled
        bool cmd_enable : 1;   ///< whether command is enabled
        bool bs_enable : 1;    ///< whether to work under blue screen
        bool bs_only : 1;      ///< whether to work under blue screen only
    };
    uint32_t mode; ///< combined mode word
} drvDebugPortMode_t;

/**
 * \brief debug port status
 */
typedef union {
    struct
    {
        bool usb_enabled : 1;     ///< for usb debug port only, whether is enabled
        bool usb_host_opened : 1; ///< for usb debug port only, whether host has opened
    };
    uint32_t status; ///< combined status word
} drvDebugPortStatus_t;

/**
 * \brief debug port base data struct
 *
 * This is base of debug port. That means the operation pointer must be
 * the first member, and private data can follow.
 */
typedef struct drvDebugPort
{
    /** operations */
    const drvDebugPortOps_t *ops;
    /** debug port device name */
    unsigned name;
    /** debug port mode */
    drvDebugPortMode_t mode;
    /** debug port status */
    drvDebugPortStatus_t status;
    /** iterator for the global debug port list */
    /**/ SLIST_ENTRY(drvDebugPort) iter;
} drvDebugPort_t;

/**
 * \brief send packet through debug port
 *
 * The packet can be host protocol or diag protocol, depends on the debug
 * mode at creation.
 *
 * Usually, it is called by the host command processing or diag command
 * processing.
 *
 * When host protocol is used, the packet should be a complete host packet.
 * \p data should be 4 bytes aligned, and there are no alignment requirement
 * for \p size.
 *
 * When diag protocol is used, the packet should be a complete hdlc encoded
 * packet, with leading and trailing sync byte. \p data should be 4 bytes
 * aligned, and there are no erquirement for \p size.
 *
 * When trace is enabled in the debug port, the packet will be mixed with
 * trace data.
 *
 * \param d debug port instance, must be valid
 * \param data packet data
 * \param size packet size
 * \return
 *      - true on success
 *      - false on error, invalid parameters or send timeout
 */
bool drvDebugPortSendPacket(drvDebugPort_t *d, const void *data, unsigned size);

/**
 * \brief set rx callback
 *
 * \p cb will be called when there are rx data arrived. Most likely, it
 * will be called in ISR. So, the recommended callback is to enqueue a
 * work for the real processing.
 *
 * \param d debug port instance, must be valid
 * \param cb rx arrive callback
 * \param param callback context
 */
void drvDebugPortSetRxCallback(drvDebugPort_t *d, osiCallback_t cb, void *param);

/**
 * \brief set trace enable
 *
 * It may be called for the registered debug port at \p osiTraceSetEnable.
 *
 * \param d debug port instance, must be valid
 * \param enable trace enable
 */
void drvDebugPortSetTraceEnable(drvDebugPort_t *d, bool enable);

/**
 * \brief read from debug port
 *
 * \p cb will be called when there are rx data arrived. Most likely, it
 * will be called in ISR. So, the recommended callback is to enqueue a
 * work for the real processing.
 *
 * \param d debug port instance, must be valid
 * \param data memory for read
 * \param size maximum read size
 * \return
 *      - read size, 0 for no rx data available
 *      - -1 on error
 */
int drvDebugPortRead(drvDebugPort_t *d, void *data, unsigned size);

/**
 * \brief get debug port device name
 *
 * \param d debug port instance, must be valid
 * \return device name
 */
static inline unsigned drvDebugPortGetName(drvDebugPort_t *d) { return d->name; }

/**
 * \brief get debug port mode
 *
 * \param d debug port instance, must be valid
 * \return debug port mode
 */
static inline drvDebugPortMode_t drvDebugPortGetMode(drvDebugPort_t *d) { return d->mode; }

/**
 * \brief whether the debug port is on usb serial
 *
 * \param d debug port instance, must be valid
 * \return true if debug port on usb
 */
static inline bool drvDebugPortIsUsb(drvDebugPort_t *d) { return DRV_NAME_IS_USRL(d->name); }

/**
 * \brief whether trace is enabled on the debug port
 *
 * \param d debug port instance, must be valid
 * \return true if trace is enabled
 */
static inline bool drvDebugPortIsTraceEnabled(drvDebugPort_t *d) { return d->mode.trace_enable; }

/**
 * \brief whether the usb serial debug port is opened by host
 *
 * \param d debug port instance, must be valid
 * \return true if opened by host
 */
static inline bool drvDebugPortIsUsbHostOpened(drvDebugPort_t *d) { return d->status.usb_host_opened; }

/**
 * \brief register the debug port to global list
 *
 * \param d debug port instance, must be valid
 */
void drvDebugPortRegister(drvDebugPort_t *d);

/**
 * \brief find debug port by deivce name
 *
 * \param name device name
 * \return
 *      - debug port instance
 *      - NULL if there are no debug port on the device
 */
drvDebugPort_t *drvDebugPortFindByName(uint32_t name);

/**
 * \brief create debug port on debughost
 *
 * Usually, there is only one debughost in system. So, it is not needed
 * to specify the device name. Also, the configuration of debughost is
 * fixed.
 *
 * \param mode debugging mode
 * \return
 *      - debughost instance
 */
drvDebugPort_t *drvDhostCreate(drvDebugPortMode_t mode);

/**
 * \brief create debug port on uart
 *
 * \param name uart device name
 * \param mode debugging mode
 * \return
 *      - debug uart instance
 */
drvDebugPort_t *drvDebugUartPortCreate(unsigned name, drvDebugPortMode_t mode);

/**
 * \brief create debug port on usb serial
 *
 * \param name usb serial device name
 * \param mode debugging mode
 * \return
 *      - debug uart instance
 */
drvDebugPort_t *drvDebugUserialPortCreate(unsigned name, drvDebugPortMode_t mode);

/**
 * \brief restore debughost configuration
 *
 * It is for debug purpose only.
 */
void drvDhostRestoreConfig(void);

OSI_EXTERN_C_END
#endif
