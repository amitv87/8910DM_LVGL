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

#ifndef _DRV_IPC_FIFO_H_
#define _DRV_IPC_FIFO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief flag to indicate the fifo is ready
 */
#define DRV_IPC_FIFO_READY (1 << 0)

/**
 * \brief forward declaration
 */
struct osiFifo;

/**
 * \brief IPC fifo from the point of view of master
 *
 * There are 2 fifos for each IPC channel, one for send and the other
 * for receive. The fifo control data struct is located in the shared
 * memory. So, the fields of send for one side is the fields of receive
 * for the other side. This is the data struct for master.
 *
 * The data struct in shared memory should be 32 bytes aligned.
 *
 * For master side, it is recommened to define:
 * \code{.cpp}
 * typedef drvIpcFifoMasterCtrl_t drvIpcFifoCtrl_t;
 * \endcode
 */
typedef struct
{
    struct
    {
        uint32_t status;      ///< status
        uint32_t send_base;   ///< send fifo base address
        uint32_t recv_base;   ///< receive fifo base address
        uint32_t send_size;   ///< send fifo size
        uint32_t recv_size;   ///< receive fifo size
        uint32_t send_wr;     ///< send fifo write position
        uint32_t recv_rd;     ///< receive fifo read position
        uint32_t reserved[1]; ///< reserved
    } th;                     ///< this side (master) writable struct
    struct
    {
        uint32_t status;      ///< status
        uint32_t send_base;   ///< send fifo base address
        uint32_t recv_base;   ///< receive fifo base address
        uint32_t send_size;   ///< send fifo size
        uint32_t recv_size;   ///< receive fifo size
        uint32_t send_rd;     ///< send fifo read position
        uint32_t recv_wr;     ///< receive fifo write position
        uint32_t reserved[1]; ///< reserved
    } peer;                   ///< peer side (slave) writable struct
} drvIpcFifoMasterCtrl_t;

/**
 * \brief IPC fifo from the point of view of slave
 *
 * For slave side, it is recommened to define:
 * \code{.cpp}
 * typedef drvIpcFifoSlaveCtrl_t drvIpcFifoCtrl_t;
 * \endcode
 *
 * Based on \p drvIpcFifoCtrl_t, the source codes for master and slave
 * are identical.
 */
typedef struct
{
    struct
    {
        uint32_t status;      ///< status
        uint32_t recv_base;   ///< receive fifo base address
        uint32_t send_base;   ///< send fifo base address
        uint32_t recv_size;   ///< receive fifo size
        uint32_t send_size;   ///< send fifo size
        uint32_t recv_wr;     ///< receive fifo write position
        uint32_t send_rd;     ///< send fifo read position
        uint32_t reserved[1]; ///< reserved
    } peer;                   ///< peer side (master) writable struct
    struct
    {
        uint32_t status;      ///< status
        uint32_t recv_base;   ///< receive fifo base address
        uint32_t send_base;   ///< send fifo base address
        uint32_t recv_size;   ///< receive fifo size
        uint32_t send_size;   ///< send fifo size
        uint32_t recv_rd;     ///< receive fifo write position
        uint32_t send_wr;     ///< send fifo read position
        uint32_t reserved[1]; ///< reserved
    } th;                     ///< peer side (slave) writable struct
} drvIpcFifoSlaveCtrl_t;

/**
 * \brief ipc command header
 *
 * Usually, channel 0 is the control channel. And the data in control
 * channel is command packets.
 *
 * The command packet header is compatible with RPC packet. And \p opcode
 * shouldn't conflict with RPC when it is used for other purpose. Also,
 * multiple purposes can be mixed in one channel.
 *
 * \p size is required to be aligned to 8 bytes. When needed, the memory
 * in shared memory can be casted to \p drvIpcCmdHeader_t data struct.
 */
typedef struct
{
    uint32_t opcode; ///< operation in command header
    uint32_t size;   ///< the whole command packet size, including header
} drvIpcCmdHeader_t;

/**
 * \brief initialze ipc fifo (master)
 *
 * \param pctrl ipc fifo control
 * \param send_base send fifo base address
 * \param send_size send fifo size
 * \param recv_base receive fifo base address
 * \param recv_size receive fifo size
 */
void drvIpcFifoMasterInit(drvIpcFifoMasterCtrl_t *pctrl,
                          unsigned send_base, unsigned send_size,
                          unsigned recv_base, unsigned recv_size);

/**
 * \brief ipc fifo read available byte count (master)
 *
 * \param pctrl ipc fifo control
 */
unsigned drvIpcFifoMasterAvail(drvIpcFifoMasterCtrl_t *pctrl);

/**
 * \brief ipc fifo write space byte count (master)
 *
 * \param pctrl ipc fifo control
 */
unsigned drvIpcFifoMasterSpace(drvIpcFifoMasterCtrl_t *pctrl);

/**
 * \brief read from ipc fifo (master)
 *
 * The returned real read size may be smaller than \p size.
 *
 * It is not thread safe. Thread safe shall be considered by caller.
 *
 * \param pctrl ipc fifo control
 * \param data memory for read
 * \param size memory size
 * \return real read size
 */
int drvIpcFifoMasterRead(drvIpcFifoMasterCtrl_t *pctrl, void *data, unsigned size);

/**
 * \brief write to ipc fifo (master)
 *
 * The returned real written size may be smaller than \p size.
 *
 * It is not thread safe. Thread safe shall be considered by caller.
 *
 * \param pctrl ipc fifo control
 * \param data data to be written
 * \param size data size
 * \return real written size
 */
int drvIpcFifoMasterWrite(drvIpcFifoMasterCtrl_t *pctrl, const void *data, unsigned size);

/**
 * \brief read from ipc fifo and write to local fifo (master)
 *
 * It is not thread safe. Thread safe shall be considered by caller.
 *
 * \param pctrl ipc fifo control
 * \param fifo local fifo
 * \return real read size
 */
int drvIpcFifoMasterReadToFifo(drvIpcFifoMasterCtrl_t *pctrl, struct osiFifo *fifo);

/**
 * \brief write to ipc fifo and read from local fifo (master)
 *
 * It is not thread safe. Thread safe shall be considered by caller.
 *
 * \param pctrl ipc fifo control
 * \param fifo local fifo
 * \return real written size
 */
int drvIpcFifoMasterWriteFromFifo(drvIpcFifoMasterCtrl_t *pctrl, struct osiFifo *fifo);

/**
 * \brief initialze ipc fifo (slave)
 *
 * \param pctrl ipc fifo control
 */
void drvIpcFifoSlaveInit(drvIpcFifoSlaveCtrl_t *pctrl);

/**
 * \brief ipc fifo read available byte count (slave)
 *
 * \param pctrl ipc fifo control
 */
unsigned drvIpcFifoSlaveAvail(drvIpcFifoSlaveCtrl_t *pctrl);

/**
 * \brief ipc fifo write space byte count (slave)
 *
 * \param pctrl ipc fifo control
 */
unsigned drvIpcFifoSlaveSpace(drvIpcFifoSlaveCtrl_t *pctrl);

/**
 * \brief read from ipc fifo (slave)
 *
 * It is similar to \p drvIpcFifoMasterRead, just the fifo control is
 * interpreted as slave.
 *
 * \param pctrl ipc fifo control
 * \param data memory for read
 * \param size memory size
 * \return real read size
 */
int drvIpcFifoSlaveRead(drvIpcFifoSlaveCtrl_t *pctrl, void *data, unsigned size);

/**
 * \brief write to ipc fifo (slave)
 *
 * It is similar to \p drvIpcFifoMasterWrite, just the fifo control is
 * interpreted as slave.
 *
 * \param pctrl ipc fifo control
 * \param data data to be written
 * \param size data size
 * \return real written size
 */
int drvIpcFifoSlaveWrite(drvIpcFifoSlaveCtrl_t *pctrl, const void *data, unsigned size);

/**
 * \brief read from ipc fifo and write to local fifo (slave)
 *
 * It is similar to \p drvIpcFifoMasterReadToFifo, just the fifo control is
 * interpreted as slave.
 *
 * \param pctrl ipc fifo control
 * \param fifo local fifo
 * \return real read size
 */
int drvIpcFifoSlaveReadToFifo(drvIpcFifoSlaveCtrl_t *pctrl, struct osiFifo *fifo);

/**
 * \brief write to ipc fifo and read from local fifo (slave)
 *
 * It is not thread safe. Thread safe shall be considered by caller.
 *
 * \param pctrl ipc fifo control
 * \param fifo local fifo
 * \return real written size
 */
int drvIpcFifoSlaveWriteFromFifo(drvIpcFifoSlaveCtrl_t *pctrl, struct osiFifo *fifo);

#ifdef __cplusplus
}
#endif
#endif
