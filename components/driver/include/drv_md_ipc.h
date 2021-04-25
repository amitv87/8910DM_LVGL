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

#ifndef _DRV_MD_IPC_H_
#define _DRV_MD_IPC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IPC channel ID
 */
enum smd_ch_id
{
    SMD_CH_AT = 1,       ///< alias for SMD_CH_AT1
    SMD_CH_AT1 = 1,      ///< AT1, stream type
    SMD_CH_AT2 = 2,      ///< AT2, stream type
    SMD_CH_PS = 3,       ///< PS, packet type
    SMD_CH_A2C_CTRL = 4, ///< A2C_CTRL, queue type
    SMD_CH_AUD_CTRL = 5, ///< AUD_CTRL, command type
    SMD_CH_MAX           ///< placeholder for count
};

/**
 * @brief channel Event
 */
enum smd_ch_flag
{
    CH_READ_AVAIL = 0x1,                          ///< indicate read available
    CH_WRITE_AVAIL = 0x2,                         ///< indicate write available
    CH_OPENED = 0x4,                              ///< flag for channel opened
    CH_RW_MASK = (CH_READ_AVAIL | CH_WRITE_AVAIL) ///< read and write mask
};

/**
 * PS uplink buffer maximum size
 *
 * When write to PS IPC channel, the maximum packet size can't exceed
 * this.
 */
#define IPC_PS_BUF_UL_LEN_B 1568

/**
 * PS uplink buffer header size
 *
 * When copy data to PC uplink buffer, the payload should be copied
 * to the allocated buffer with is offset.
 */
#define IPC_PS_UL_HDR_LEN 40

/**
 * IPC command header
 */
struct ipc_cmd
{
    uint32_t id;    ///< command id
    uint32_t para0; ///< 1st parameter
    uint32_t para1; ///< 2nd parameter
    uint32_t para2; ///< 3rd parameter
};

/**
 * IPC PS buf header
 */
struct ps_header
{
    uint32_t next;     ///< next offset (from shared memory) in the chain
    uint8_t cid;       ///< CID
    uint8_t simid;     ///< SIM number
    uint16_t len;      ///< data length
    uint32_t flag;     ///< BUF_IN_USR or BUF_IN_IDLE
    uint32_t buf_size; ///< 300B or 1600B
    uint32_t data_off; ///< data offset in buffer
    uint32_t id;       ///< only for debug
};

struct smd_ch;

/**
 * @brief IPC module initialization
 */
void ipcInit(void);

/**
 * @brief open an IPC channel
 *
 * Each IPC channel can be opened only once.
 *
 * It is permitted \p notify to be NULL.
 *
 * \p notify will be called in ISR context. So, it should follow ISR
 * programming guide. In the notify itself, it is not permitted to
 * read or write the channel
 *
 * @param ch_id     channel ID
 * @param ch        output channel pointer
 * @param priv      channel notify callback first argument
 * @param notify    channel notify callback
 * @return
 *      - 0 on success
 *      - -ENODEV/-EPERM on error
 */
int ipc_ch_open(int ch_id, struct smd_ch **ch, void *priv,
                void (*notify)(void *, uint32_t));

/**
 * @brief set IPC channel event mask
 *
 * Only masked event will invoke the registered \p notify callback.
 *
 * The default event mask is 0. That is the callback won't be invoked.
 *
 * @param ch        IPC channel, must be valid
 * @param event_mask    event mask to be set
 */
void ipc_ch_set_event_mask(struct smd_ch *ch, uint32_t event_mask);

/**
 * @brief read from IPC channel
 *
 * The read content depends on channel type:
 * - stream: read data. The return value is the actual read bytes,
 *   and may be less than \p len.
 * - packet: \p data is the packet offset, \p len is the offset count
 *   rather than size of \p data. However, the return value is the
 *   read bytes.
 * - command: \p data is the command including header and data,
 *   the return value is the actual read bytes.
 * - queue: \p data is \p ipc_cmd, \p len is the count of \p ipc_cmd,
 *   the return value is the actual read bytes
 *
 * @param ch        IPC channel, must be valid
 * @param data      data to be read
 * @param len       data length
 * @return
 *      - actual read
 *      - -EINVAL, -EAGAIN on fail
 */
int ipc_ch_read(struct smd_ch *ch, void *data, uint32_t len);

/**
 * @brief write to IPC channel
 *
 * The write content depends on channel type:
 * - stream: write data. The return value is the actual written bytes,
 *   and may be less than \p len.
 * - packet: \p data is the packet offset, \p len is the offset count
 *   rather than size of \p data. However, the return value is the
 *   written bytes.
 * - command: write content is command (includes header and data).
 *   When \p size is 0, the control fifo will be cleared.
 * - queue: \p data is \p ipc_cmd, \p len is the count of \p ipc_cmd,
 *   the return value is the actual read bytes
 *
 * After write, even partial write, it will set peer's interrupt.
 *
 * @param ch        IPC channel, must be valid
 * @param data      data to be written
 * @param len       data length
 * @return
 *      - actual written
 *      - 0 for no space, and won't set peer's interrupt
 *      - -ENODEV if the uplink channel is not openned
 *      - -EINVAL for invalid parameters
 */
int ipc_ch_write(struct smd_ch *ch, const void *data, uint32_t len);

/**
 * @brief write to IPC channel, not trigger peer interrupt
 *
 * It is similar to \p ipc_ch_write, just not to trigger peer interrupt.
 *
 * @param ch        IPC channel, must be valid
 * @param data      data to be written
 * @param len       data length
 * @return
 *      - actual written
 *      - 0 for no space, and won't set peer's interrupt
 *      - -ENODEV if the uplink channel is not openned
 *      - -EINVAL for invalid parameters
 */
int ipc_ch_write_noint(struct smd_ch *ch, const void *data, uint32_t len);

/**
 * @brief channel available read space
 *
 * The meaning of the return unit depends on channel type:
 * - stream: available bytes
 * - packet: available offset count
 * - command: available bytes including command header
 * - queue: available count of \p ipc_cmd
 *
 * @param ch        IPC channel, must be valid
 * @return
 *      - available
 *      - 0 for unavailable
 */
int ipc_ch_read_avail(struct smd_ch *ch);

/**
 * @brief channel available write space
 *
 * The meaning of return unit depends on channel type:
 * - stream: available write bytes
 * - packet: available offset write count
 * - command: available write bytes of command data (except command header)
 * - queue: available count of \p ipc_cmd
 *
 * @param ch        IPC channel, must be valid
 * @return
 *      - available
 *      - 0 for no space
 */
int ipc_ch_write_avail(struct smd_ch *ch);

/**
 * @brief free downlink PS buf
 *
 * Even if the downlink PS buf is in a chain, this will free the
 * specified downlink PS buf only. Other buf in the chain is
 * untouched.
 *
 * @param ch        the PS IPC channel, must be valid
 * @param offset    the offset from share memory of the downlink buf
 * @return
 *      - 0 in success, it will never fail
 */
int ipc_free_dl_ps_buf(struct smd_ch *ch, uint32_t offset);

/**
 * @brief allocate uplink PS buf
 *
 * The size will be used to decide big buffer or little buffer. It doesn't
 * mean that the allocated buffer is large enough for requested \p size.
 *
 * @param ch    the PS IPC channel, must be valid
 * @param size  request size
 * @return
 *      - uplink buf offset in shared memory
 *      - -ENOMEM if no available buffer
 */
int ipc_alloc_ul_ps_buf(struct smd_ch *ch, uint32_t size);

/**
 * @brief notify AP panic to CP
 *
 * It should be called after AP panic.
 */
void ipc_notify_cp_assert(void);

/**
 * @brief show CP assert information
 */
void ipc_show_cp_assert(void);

/**
 * @brief get CP assert information address
 */
void *ipc_get_cp_assert_cause(void);

/**
 * @brief switch cp trace
 *
 * @param en    enable cp trace or not
 */
void ipc_switch_cp_trace(bool en);

/**
 * @brief notify sim plug in/out to CP
 *
 * @param num       sim number (1/2)
 * @param connect   true indicate the sim plug in otherwise plug out
 */
void ipc_notify_sim_detect(int num, bool connect);

/*
* @brief zsp audio request notify.
*
* @param hanlder    the notify handler
*/
void ipc_register_audio_notify(void (*handler)(void));

/*
* @brief modem trace request notify.
*
* @param hanlder    the notify handler
* @param param      caller private data
*/
void ipc_register_trace_notify(void (*handler)(uint32_t, void *), void *param);

/**
 * @brief modem MOS request notify
 *
 * @param param     caller private data
 */
void ipc_register_mos_notify(void (*handler)(void *), void *param);

/**
 * @brief bt riscv log request notify
 *
 * @param param     caller private data
 */
void ipc_register_bt_log_notify(void (*handler)(uint32_t, void *), void *param);

/**
 * @brief bt riscv sleep request notify
 *
 * @param param     caller private data
 */
void ipc_register_bt_sleep_notify(void (*handler)(uint32_t));

/**
 * @brief bt riscv assert request notify
 *
 * @param param     caller private data
 */
void ipc_register_bt_assert_notify(void (*handler)(void));

/**
 * @brief update shared memory MMU properties
 */
void ipc_update_buf_access(void);

/*
* @brief ipc_disable_ch_irq
*
* @param ch  the smd channel
*/
void ipc_disable_ch_irq(struct smd_ch *ch);

/*
* @brief ipc_enable_ch_irq
*
* @param ch  the smd channel
*/
void ipc_enable_ch_irq(struct smd_ch *ch);

/*
* @brief notify function type to be called
* it will be called when cp need notify ap
* to update rf param
*/
typedef void (*rfParamNotify_t)(void *ctx1, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4);

/**
 * @brief modem rf param request notify
 *
 * @param param     caller private data
 */
void ipc_register_rfParam_notify(rfParamNotify_t notify, void *param);

/**
 * @brief cp rf param init function
 */
void ipc_cpRfParamInit(uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4);

/**
 * @brief ipc get delta nv addr
 */
uint8_t *ipc_get_deltanv_addr(void);

/**
 * @brief ipc get ims nv addr and send sysmail
 */
uint8_t *ipc_get_ims_nv_addr(void);

/**
 * @brief ipc set delta addr and size send sysmail
 */
void ipc_set_deltanv_bin(uint32_t addr, uint32_t len);

/**
 * @brief cp wake lock (not permit sleep)
 */
void ipc_cp_wake_lock(void);

#ifdef __cplusplus
}
#endif
#endif
