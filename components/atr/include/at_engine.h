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

#ifndef _AT_ENGINE_H_
#define _AT_ENGINE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "osi_api.h"
#include "osi_event_hub.h"
#include "osi_generic_list.h"
#include "osi_pipe.h"
#include "atr_config.h"
#include "at_cfg.h"
#include "quec_proj_config.h"
#include "drv_uart.h"
#include "drv_serial.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief opaque data structure of AT engine
 */
typedef struct atEngine atEngine_t;

/**
 * @brief data structure of AT device
 */
typedef struct atDevice atDevice_t;

/**
 * @brief opaque data structure of AT dispatch
 */
typedef struct atDispatch atDispatch_t;

/**
 * @brief opaque data structure of AT command mode engine
 */
typedef struct atCmdEngine atCmdEngine_t;

/**
 * @brief opaque data structure of AT data mode engine
 */
typedef struct atDataEngine atDataEngine_t;

/**
 * @brief opaque data structure of AT mux mode engine
 */
typedef struct atCmuxEngine atCmuxEngine_t;

/**
 * @brief data structure of AT command
 */
typedef struct atCommand atCommand_t;

/**
 * @ brief AT device character framing format
 *
 * Refer to V.250 +ICF. The enum value *matches* format parameter in +ICF
 * command.
 */
typedef enum atDeviceFormat
{
    AT_DEVICE_FORMAT_AUTO_DETECT, ///< auto detect
    AT_DEVICE_FORMAT_8N2,         ///< 8 Data; 2 Stop
    AT_DEVICE_FORMAT_811,         ///< 8 Data; 1 Parity; 1 Stop
    AT_DEVICE_FORMAT_8N1,         ///< 8 Data; 1 Stop
    AT_DEVICE_FORMAT_7N2,         ///< 7 Data; 2 Stop
    AT_DEVICE_FORMAT_711,         ///< 7 Data; 1 Parity; 1 Stop
    AT_DEVICE_FORMAT_7N1          ///< 7 Data; 1 Stop
} atDeviceFormat_t;

/**
 * @ brief AT device character framing parity
 *
 * Refer to V.250 +ICF. The enum value *matches* format parameter in +ICF
 * command.
 */
typedef enum atDeviceParity
{
    AT_DEVICE_PARITY_ODD,  ///< Odd
    AT_DEVICE_PARITY_EVEN, ///< Even
    AT_DEVICE_PARITY_MARK, ///< Mark
    AT_DEVICE_PARITY_SPACE ///< Space
} atDeviceParity_t;

/**
 * @ brief AT device receive flow control
 *
 * Refer to V.250 +IFC. The enum value *matches* format parameter in +IFC
 * command. Not supported options are not listed.
 */
typedef enum atDeviceRXFC
{
    AT_DEVICE_RXFC_NONE,  ///< None
    AT_DEVICE_RXFC_HW = 2 ///< Circuit 133 (Ready for Receiving)
} atDeviceRXFC_t;

/**
 * @ brief AT device transfer flow control
 *
 * Refer to V.250 +IFC. The enum value *matches* format parameter in +IFC
 * command. Not supported options are not listed.
 */
typedef enum atDeviceTXFC
{
    AT_DEVICE_TXFC_NONE,  ///< None
    AT_DEVICE_TXFC_HW = 2 ///< Circuit 106 (Clear to Send/Ready for Sending)
} atDeviceTXFC_t;

/**
 * @brief AT device data structure
 */
struct atDevice
{
    /** delete function */
    void (*destroy)(atDevice_t *th);
    /** open function */
    bool (*open)(atDevice_t *th);
    /** close function */
    void (*close)(atDevice_t *th);
    /** write function */
    int (*write)(atDevice_t *th, const void *data, size_t size);
    /** read function */
    int (*read)(atDevice_t *th, void *data, size_t size);
    /** get read available bytes function */
    int (*read_avail)(atDevice_t *th);
    /** get write available space function */
    int (*write_avail)(atDevice_t *th);
    /** set baudrate function */
    void (*set_baud)(atDevice_t *th, size_t baud);
    /** set format function */
    void (*set_format)(atDevice_t *th, atDeviceFormat_t format, atDeviceParity_t parity);
    /** set flow control */
    bool (*set_flow_ctrl)(atDevice_t *th, atDeviceRXFC_t rxfc, atDeviceTXFC_t txfc);
    /** set auto sleep timeout */
    void (*set_auto_sleep)(atDevice_t *th, int timeout);
    /** check if the device is ready*/
    bool isReady;
    /** the dispatch */
    atDispatch_t *recv;
};

/**
 * @brief CMUX configuration
 */
typedef struct
{
    uint8_t transparency;      ///< 0: basic, 1: advanced
    uint8_t subset;            ///< 0: UIH, 1: UI, 2: I
    uint8_t port_speed;        ///< transmission rate
    int max_frame_size;        ///< maximum frame size
    uint8_t ack_timer;         ///< acknowledgement timer in units of ten milliseconds
    uint8_t max_retrans_count; ///< maximum number of re-transmissions
    uint8_t resp_timer;        ///< response timer for the multiplexer control channel in units of ten milliseconds
    uint8_t wakeup_resp_timer; ///< wake up response timer in seconds
    uint8_t window_size;       ///< window size, for Advanced option with Error-Recovery Mode
} atCmuxConfig_t;

/**
 * @brief enum type of command mode engine prompt finish mode
 *
 * In prompt mode, BACKSPACE will be chcked. When BACKSPACE is input
 * in the middle, the previous character will be removed from the
 * buffer.
 */
typedef enum atCmdPromptEndMode
{
    AT_PROMPT_END_CTRL_Z,  ///< ended with CTRL-Z
    AT_PROMPT_END_ESC,     ///< ended with ESCAPE
    AT_PROMPT_END_OVERFLOW ///< provided buffer overflow
} atCmdPromptEndMode_t;

/**
 * @brief UART AT device configuration
 */
typedef struct
{
    uint32_t name;           ///< uart name, such as DRV_NAME_UART1
    size_t baud;             ///< baud rate
    atDeviceFormat_t format; ///< character framing format
    atDeviceParity_t parity; ///< character framing parity
    bool rts_enable;         ///< hw flow control, rts enable
    bool cts_enable;         ///< hw flow control, cts enable
} atDeviceUartConfig_t;

/**
 * @brief virtual AT device configuration
 */
typedef struct
{
    uint32_t name;      ///< device name, only used in trace
    osiPipe_t *rx_pipe; ///< AT RX pipe
    osiPipe_t *tx_pipe; ///< AT TX pipe
} atDeviceVirtConfig_t;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
typedef struct
{
    atDevice_t ops; // API
    drvUart_t *drv;
    atDeviceUartConfig_t config; // UART configuration
    osiWork_t *work;
    uint32_t pending_event;
} atDeviceUart_t;

typedef struct
{
    atDevice_t ops; // API
    uint32_t name;
    drvSerial_t *serial;
} atDeviceUsrl_t;
#endif

/**
 * @brief callback function type after prompt mode is finished
 *
 * @param ctx       provided callback context
 * @param end_mode  prompt end mode, refer to \p atCmdPromptEndMode_t
 * @param size      received data size
 */
typedef void (*atCmdPromptCB_t)(void *ctx, atCmdPromptEndMode_t end_mode, size_t size);

/**
 * @brief callback function type in command bypass mode
 *
 * @param ctx       provided callback context
 * @param data      received data pointer
 * @param size      received data size
 * @return
 *      - consumed bytes, it can be less than \p size
 *      - 0 if nothing consumed
 */
typedef int (*atCmdBypassCB_t)(void *ctx, const void *data, size_t size);

/**
 * @brief callback function type in data bypass mode
 *
 * @param ctx       provided callback context
 * @param data      received data pointer
 * @param size      received data size
 * @return
 *      - consumed bytes, it can be less than \p size
 *      - 0 if nothing consumed
 */
typedef int (*atDataBypassCB_t)(void *ctx, const void *data, size_t size);

/**
 * @brief set AT device dispatch
 *
 * @param th        AT device pointer, must be valid
 * @param recv      AT dispatch pointer, must be valid
 */
static inline void atDeviceSetDispatch(atDevice_t *th, atDispatch_t *recv) { th->recv = recv; }

/**
 * @brief get AT device dispatch
 *
 * @param th        AT device pointer, must be valid
 * @return      AT dispatch pointer
 */
static inline atDispatch_t *atDeviceGetDispatch(atDevice_t *th) { return th->recv; }

/**
 * @brief create UART AT device
 *
 * After create, the device is in *close* state. \p atDeviceOpen
 * should be called before access the device.
 *
 * @param cfg       UART AT device configuration, must be valid
 * @return
 *      - UART AT device pointer
 *      - NULL if out of memory
 */
atDevice_t *atDeviceUartCreate(atDeviceUartConfig_t *cfg);

/**
 * @brief create serial AT device
 *
 * This used to create USB CDC/ACM AT device.
 *
 * @param name      serial device name, such as DRV_NAME_USRL_COM0
 * @return
 *      - serial AT device pointer
 *      - NULL if out of memory
 */
atDevice_t *atDeviceUserialCreate(uint32_t name);

/**
 * @brief create diag at device
 *
 * This used to create diag AT device
 *
 * @return
 *      - NULL      fail
 *      - other     the at device
 */
atDevice_t *atDeviceDiagCreate();

/**
 * @brief create virtual AT device
 *
 * @param cfg       virtual AT device configuration, must be valid
 * @return
 *      - AT device pointer
 *      - NULL if out of memory
 */
atDevice_t *atDeviceVirtCreate(const atDeviceVirtConfig_t *cfg);

/**
 * @brief delete the AT device
 *
 * When \p th is NULL, nothing will be done.
 *
 * @param th        AT device to be deleted
 */
void atDeviceDelete(atDevice_t *th);

/**
 * @brief open the AT device for read and write
 *
 * @param th        AT device, must be valid
 * @return
 *      - true on success
 */
bool atDeviceOpen(atDevice_t *th);

/**
 * @brief close the AT device
 *
 * @param th        AT device, must be valid
 */
void atDeviceClose(atDevice_t *th);

/**
 * @brief write data to AT device
 *
 * AT device will try to write all data. When the output buffer is
 * full, it will wait.
 *
 * Usually, AT device will define a *reasonable* timeout. At timeout,
 * the written size may be less than specified size.
 *
 * When \p size is 0, nothing will be done.
 *
 * @param th        AT device, must be valid
 * @param data      data pointer to be written, must be valid if \p size
 *                  is not zero
 * @param size      data size
 * @return
 *      - written byte count
 *      - -1 if parameter is invalid, or device error
 */
int atDeviceWrite(atDevice_t *th, const void *data, size_t size);

/**
 * @brief read data from AT device
 *
 * It will just read from the device receive buffer. When the buffer
 * is empty, return 0. Even the receive buffer is not empty, the return
 * size may be less than the specified size.
 *
 * When \p size is 0, nothing will be done.
 *
 * @param th        AT device, must be valid
 * @param data      mempry pointer for read, must be valid if \p size
 *                  is not zero
 * @param size      memory size
 * @return
 *      - read byte count
 *      - -1 if parameter is invalid, or device error
 */
int atDeviceRead(atDevice_t *th, void *data, size_t size);

/**
 * @brief AT device read available bytes
 *
 * @param th        AT device, must be valid
 * @return
 *      - (-1) Parameter error
 *      - OTHERS (>=0) Available size in byte
 */
int atDeviceReadAvail(atDevice_t *th);

/**
 * @brief AT device write available space
 *
 * @param th        AT device, must be valid
 * @return
 *      - (-1) Parameter error
 *      - OTHERS (>=0) Available size in byte
 */
int atDeviceWriteAvail(atDevice_t *th);

/**
 * @brief set AT device baudrate
 *
 * @param th        AT device, must be valid
 * @param baud      baud rate
 */
void atDeviceSetBaudrate(atDevice_t *th, size_t baud);

/**
 * @brief set AT device charactor format
 *
 * @param th        AT device, must be valid
 * @param format    refer to \p atDeviceFormat_t
 * @param parity    refer to \p atDeviceParity_t
 */
void atDeviceSetFormat(atDevice_t *th, atDeviceFormat_t format, atDeviceParity_t parity);

/**
 * @brief set AT device flow control
 *
 * @param th        AT device, must be valid
 * @param rxfc      flow control of RX
 * @param txfc      flow control of TX
 * @return
 *      - true on success
 *      - false on failed
 */
bool atDeviceSetFlowCtrl(atDevice_t *th, atDeviceRXFC_t rxfc, atDeviceTXFC_t txfc);

/**
 * @brief set AT device auto sleep
 *
 * Refer to \p drvUartSetAutoSleep
 *
 * @param th        AT device, must be valid
 * @param timeout   auto sleep wait time after transfer done. It can be
 *                  0 but not recommended. Negative value to disable
 *                  auto sleep feature.
 */
void atDeviceSetAutoSleep(atDevice_t *th, int timeout);

/**
 * @brief create a dispatch of device
 *
 * @param device    AT device pointer, must be valid
 * @return      the created dispatch pointer
 */
atDispatch_t *atDispatchCreate(atDevice_t *device);

/**
 * @brief create dispatch for device or cmux DLC
 *
 * When \p device is not NULL, the dispatch is for device. When
 * \p cmuxParent is not NULL, the dispatch is for cmux DLC. It is
 * invalid that both of \p device and \p cmuxParent are not NULL,
 * or both of \p device and \p cmuxParent are NULL.
 *
 * @param device    AT device
 * @param cmuxParant    parent cmux mode engine
 * @param dlci      when create dispatch for cmux DLC, it is the
 *                  DLC number
 * @return
 *      - the created dispatch
 *      - NULL if out of memory
 */
atDispatch_t *atDispatchDlcCreate(atDevice_t *device, atCmuxEngine_t *cmuxParent, int dlci);

/**
 * @brief delete a dispatch
 *
 * When \p th is NULL, nothing will be done.
 *
 * When engines of the dispatch is busy, the dispatch won't be deleted
 * immediately. Rather, it will be deleted after engines is non-busy.
 * When the dispatch is in delete pending mode, engine output will be
 * disabled.
 *
 * @param th        the dispatch pointer, must be valid
 */
void atDispatchDelete(atDispatch_t *th);

/**
 * @brief read data from AT device
 *
 * Usually, it is called (directly or through thread callback) when there
 * are input data in AT device.
 *
 * @param th        the dispatch pointer, must be valid
 */
void atDispatchRead(atDispatch_t *th);

/**
 * @brief send event to AT thread itself to consume more data
 *
 * AT thread is working in non-blocking asynchronous mode. When downstream
 * is ready, it is needed to notify upstream to consume more data. Event
 * is used to avoid too many levels of function calls.
 *
 * @param th        the dispatch pointer, must be valid
 */
void atDispatchReadLater(atDispatch_t *th);

/**
 * @brief notify dispatch that device is broken
 *
 * This can be called outside AT thread.
 *
 * @param th        the dispatch pointer, must be valid
 */
void atDispatchDeviceBroken(atDispatch_t *th);

/**
 * @brief get AT device of the dispatch
 *
 * When the dispatch is binded to CMUX DLC, the device of the parent
 * CMUX engine will be returned.
 *
 * @param th        the dispatch pointer, must be valid
 * @return      the AT device pointer
 */
atDevice_t *atDispatchGetDevice(atDispatch_t *th);

/**
 * @brief get the command mode engine the dispatch
 *
 * Command mode engine of dispatch will be always created. This will
 * return the command mode engine, no matter which dispatch mode is.
 *
 * @param th        the dispatch pointer, must be valid
 * @return      the command mode engine
 */
atCmdEngine_t *atDispatchGetCmdEngine(atDispatch_t *th);

/**
 * @brief get the data mode engine the dispatch
 *
 * Data mode engine of dispatch will be created when dispatch is changed
 * to data mode. So, the returned pointer may be NULL if not in data mode.
 *
 * @param th        the dispatch pointer, must be valid
 * @return      the data mode engine
 */
atDataEngine_t *atDispatchGetDataEngine(atDispatch_t *th);

/**
 * @brief get the paremt cmux mode engine
 *
 * When the dispatch is not created for cmux DLC, it will return NULL.
 *
 * @param th        the dispatch pointer, must be valid
 * @return      the parent cmux mode engine
 */
atCmuxEngine_t *atDispatchGetParentCmuxEngine(atDispatch_t *th);

/**
 * @brief check whether in command mode
 *
 * @param th        the dispatch pointer, must be valid
 * @return
 *      - true if in command mode
 *      - false if not in command mode
 */
bool atDispatchIsCmdMode(atDispatch_t *th);

/**
 * @brief check whether in data mode
 *
 * @param th        the dispatch pointer, must be valid
 * @return
 *      - true if in data mode
 *      - false if not in data mode
 */
bool atDispatchIsDataMode(atDispatch_t *th);

/**
 * @brief check whether dispatch is in cmux mode
 *
 * @param th        the dispatch pointer, must be valid
 * @return
 *      - true if the dispatch is in cmux mode
 *      - false if the dispatch is not in cmux mode
 */
bool atDispatchIsCmuxMode(atDispatch_t *th);

#ifdef CONFIG_QUEC_PROJECT_FEATURE
extern int atDispatchGetDlci(atDispatch_t *th);
#endif

/**
 * @brief set dispatch to command mode
 *
 * @param th        the dispatch pointer, must be valid
 */
void atDispatchSetCmdMode(atDispatch_t *th);

/**
 * @brief set dispatch to data mode
 *
 * @param th        the dispatch pointer, must be valid
 */
void atDispatchSetDataMode(atDispatch_t *th);

/**
 * @brief finish data mode and set dispatch to command mode
 *
 * @param th        the dispatch pointer, must be valid
 */
void atDispatchEndDataMode(atDispatch_t *th);

/**
 * @brief finish data PPP mode and set dispatch to command mode
 *
 * Comparing to \p atDispatchEndDataMode, the dispatch will still checking
 * whether incoming data are valid PPP packets. Only when the incoming
 * data are invalid PPP packets, or configured timeout reaches, dispatch
 * will start to push data to command line engine.
 *
 * @param th        the dispatch pointer, must be valid
 */
void atDispatchEndDataPppMode(atDispatch_t *th);

/**
 * @brief dispatch end cmux mode
 *
 * It must be called in CMUX mode.
 *
 * @param th        the dispatch pointer, must be valid
 */
void atDispatchEndCmuxMode(atDispatch_t *th);

/**
 * @brief check whether dispatch in data escape mode
 *
 * @param th        the dispatch pointer, must be valid
 */
bool atDispatchInDataEscape(atDispatch_t *th);

/**
 * @brief set dispatch to cmux mode, with specified configuration
 *
 * @param th        the dispatch pointer, must be valid
 * @param cfg       cmux configuration
 */
void atDispatchSetCmuxMode(atDispatch_t *th, const atCmuxConfig_t *cfg);

/**
 * @brief get the global AT dispatch list
 *
 * @return  the global AT dispatch list head
 */
osiSlistHead_t *atDispatchGetList(void);

/**
 * @brief end cmux mode for all dispatches in cmux mode
 *
 * It will check all dispatches, and end cmux mode forcedly if the dispatch
 * is in cmux mode.
 *
 * @return  the count of dispatches in cmux mode
 */
unsigned atDispatchEndCmuxModeAll(void);

/**
 * @brief check whether it is a valid dispatch pointer
 *
 * @param p         the dispatch pointer to be checked
 * @return
 *      - true if \p p is valid dispatch pointer
 *      - false if \p p is not valid dispatch pointer
 */
bool atDispatchIsValid(atDispatch_t *p);

/**
 * @brief dispatch write data
 *
 * When the dispatch is for device, \p data will be written to device.
 * Otherwise, \p data will be written the corresponding DLC.
 *
 * @param th        the dispatch pointer, must be valid
 * @param data      data to be written, can't be NULL is size is not 0
 * @param size      data size
 */
void atDispatchWrite(atDispatch_t *th, const uint8_t *data, unsigned size);

/**
 * @brief push data to dispatch
 *
 * Usually it is called by cmux engine, to push data to dispatch for
 * DLC. At push, all data will be consumed or cached in dispatch.
 *
 * @param th        the dispatch pointer, must be valid
 * @param data      data to be pushed to dispatch, can't be NULL is size is not 0
 * @param size      data size
 */
void atDispatchPushData(atDispatch_t *th, const void *data, size_t size);

/**
 * @brief create AT command mode engine
 *
 * @param dispatch  parent dispatch of the command mode engine
 * @return
 *      - command mode engine pointer
 *      - NULL if out of memory
 */
atCmdEngine_t *atCmdEngineCreate(atDispatch_t *dispatch);

/**
 * @brief delete AT command mode engine
 *
 * @param th        command mode engine, must be valid
 */
void atCmdEngineDelete(atCmdEngine_t *th);

/**
 * @brief push data to command engine
 *
 * It will be called by dispatch. When dispatch received data, either in
 * read mode or push mode, dispatch will push the data to engine.
 *
 * @param th        cmux mode engine, must be valid
 * @param data      data to be pushed, can't be NULL if \p size is not 0.
 * @param size      data size
 * @return
 *      - consumed data size
 *      - -1 if command engine is busy
 */
int atCmdPushData(atCmdEngine_t *th, const void *data, size_t size);

/**
 * @brief write data to AT command mode engine
 *
 * When the parent dispatch is not in command mode, nothing will be done.
 *
 * It will wait all data are written (maybe in buffer of AT device).
 *
 * Though the data should be printable characters in typical cases,
 * non-printable characters are permitted. It can be used to output
 * binary data if needed.
 *
 * @param th        command mode engine, must be valid
 * @param data      data pointer to be write, can't be NULL is size is not 0
 * @param size      data size
 */
void atCmdWrite(atCmdEngine_t *th, const void *data, size_t size);

/**
 * @brief set command mode engine to command line mode
 *
 * Command line mode is the mode to receive AT commands.
 *
 * @param th        command mode engine, must be valid
 */
void atCmdSetLineMode(atCmdEngine_t *th);

/**
 * @brief set command mode engine to command line mode
 *
 * Prompt mode is the mode defined in \p CMGS and \p CMGW.
 *
 * Though the main purpose of this mode is for \p CMGS and \p CMGW, it
 * can be used in other command handling, if the behavior is the same.
 *
 * In prompt mode, the input won't come to command line buffer, rather
 * the input will be written to the provided buffer.
 *
 * @param th        command mode engine, must be valid
 * @param cb        callback after the prompt input is finished
 * @param cb_ctx    callback context
 * @param buff      buffer pointer for input data, can't be NULL
 * @param buff_size buffer size
 * @return
 *      - true on success
 *      - false on invalid parameter, the mode isn't changed
 */
bool atCmdSetPromptMode(atCmdEngine_t *th, atCmdPromptCB_t cb, void *cb_ctx, void *buff, size_t buff_size);

/**
 * @brief set command mode engine to bypass mode
 *
 * In bypass mode, the input will be passed as parameter of callback. Also,
 * the input won't be echoed no matter the setting of *ATE*.
 *
 * After bypass mode input is finished, \p atCmdSetLineMode should be called
 * explicitly.
 *
 * Typical usage is to receive binary data.
 *
 * @param th        command mode engine, must be valid
 * @param cb        callback when there are input data
 * @param cb_ctx    callback context
 * @return
 *      - true on success
 *      - false on invalid parameter
 */
bool atCmdSetBypassMode(atCmdEngine_t *th, atCmdBypassCB_t cb, void *cb_ctx);

/**
 * @brief notify AT device baudrate is changed by AT command
 *
 * AT device flow control change commands: +IPR
 *
 * @param th        command mode engine, must be valid
 */
void atCmdDeviceSetBaudNeeded(atCmdEngine_t *th);

/**
 * @brief notify AT device format is changed by AT command
 *
 * When AT device format is changed, the new setting can't be applied to
 * AT device immediately. Rather, AT device format should be changed after
 * the command line is finished. This is for AT command handler to notify
 * that AT device format change.
 *
 * AT device format change commands:
 * - +IPR
 * - +ICF
 *
 * @param th        command mode engine, must be valid
 */
void atCmdDeviceSetFormatNeeded(atCmdEngine_t *th);

/**
 * @brief notify AT device flow control is changed by AT command
 *
 * AT device flow control change commands: +IFC
 *
 * @param th        command mode engine, must be valid
 */
void atCmdDeviceSetIfcNeeded(atCmdEngine_t *th);

/**
 * @brief ignore remain commands in the command line
 *
 * @param th        command mode engine, must be valid
 */
void atCmdClearRemains(atCmdEngine_t *th);

/**
 * @brief handle one command finish
 *
 * @param engine    command mode engine, must be valid
 * @param ok        whether the command is success or fail
 */
void atCmdFinalHandle(atCmdEngine_t *engine, bool ok);

/**
 * @brief notify one command in command line is finished
 *
 * One command line may contain multiple AT commands. After each command
 * handling is finished, this should be called. And command mode engine
 * will schedule to handle the next command, if available.
 *
 * @param th        command mode engine, must be valid
 */
void atCmdCommandFinished(atCmdEngine_t *th);

/**
 * @brief AT command timeout handler function type
 *
 * @param cmd       current AT command in handling
 */
typedef void (*atCommandTimeoutHandler_t)(atCommand_t *cmd);

/**
 * @brief set command timeout handler
 *
 * Set AT command handler timeout handler. At timeout, \p handler will be
 * called.
 *
 * It is not needed to unregister the timeout handler manually. At
 * \p atCmdFinalHandle, the timeout handler will be cleared automatically.
 *
 * When \p handler is NULL, the previously registered timeout handler will
 * be cleared, through it is not needed in typical cases.
 *
 * This *must* be called in AT command handler.
 *
 * Usually, the timeout handler shall do the command clean up, and response
 * a final result code. For example:
 *
 * \code{.cpp}
 * static void _timeout(atCommand_t *cmd)
 * {
 *     // clean up ...
 *     atCmdRespCmdError(cmd->engine, cme_error_code);
 * }
 *
 * void AT_COMMAND_HANDLE(atCommand_t *cmd)
 * {
 *     // ......
 *     atCmdSetTimeoutHandler(cmd->engine, ms, _timeout);
 * }
 * \endcode
 *
 * When the AT command will switch channel to data or CMUX mode, the timeout
 * handler may be called when the channel is not in command mode. Usually, it
 * is not wanted. And it is suggested not to set timeout handler if the
 * command will switch mode.
 *
 * @param th        command mode engine, must be valid
 * @param timeout   timeout in milliseconds
 * @param handler   timeout handler
 * @return
 *      - true on success
 *      - false if there are no AT command in handling
 */
bool atCmdSetTimeoutHandler(atCmdEngine_t *th, uint32_t timeout, atCommandTimeoutHandler_t handler);

/**
 * @brief AT command abort handler function type
 *
 * @param cmd       current AT command in handling
 */
typedef void (*atCommandAbortHandler_t)(atCommand_t *cmd);

/**
 * @brief set command handler abort handler
 *
 * By default, when an AT command is in handling, further input will be
 * hold. Only after the current AT command finished, further input will
 * be parsed.
 *
 * When a valid \p handler is registered, \p handler will be called on
 * *any* input character. Usually, final result code shall be responsed
 * in \p handler to abort the current command handling.
 *
 * When command engine is not in command line mode (such as prompt mode),
 * abort handler won't be called in further input.
 *
 * It is not needed to unregister the abort handler manually. At
 * \p atCmdFinalHandle, the abort handler will be cleared automatically.
 *
 * When \p handler is NULL, the previously registered abort handler will
 * be cleared.
 *
 * Usually, the timeout handler shall do the command clean up, and response
 * a final result code. For example:
 *
 * \code{.cpp}
 * static void _aborted(atCommand_t *cmd)
 * {
 *     // clean up ...
 *     atCmdRespCmdError(cmd->engine, cme_error_code);
 * }
 *
 * void AT_COMMAND_HANDLE(atCommand_t *cmd)
 * {
 *     // ......
 *     atCmdSetAbortHandler(cmd->engine, _aborted);
 * }
 * \endcode
 *
 * @param th        command mode engine, must be valid
 * @param handler   abort handler
 * @return
 *      - true on success
 *      - false if there are no command in handling
 */
bool atCmdSetAbortHandler(atCmdEngine_t *th, atCommandAbortHandler_t handler);

/**
 * @brief check whether in command line mode
 *
 * @param th        command mode engine, must be valid
 * @return
 *      - true if in command line mode
 *      - false if not in command line mode
 */
bool atCmdIsLineMode(atCmdEngine_t *th);

/**
 * @brief check whether in prompt mode
 *
 * @param th        command mode engine, must be valid
 * @return
 *      - true if in prompt mode
 *      - false if not in prompt mode
 */
bool atCmdIsPromptMode(atCmdEngine_t *th);

/**
 * @brief check whether in bypass mode
 *
 * @param th        command mode engine, must be valid
 * @return
 *      - true if in bypass mode
 *      - false if not in bypass mode
 */
bool atCmdIsBypassMode(atCmdEngine_t *th);

/**
 * @brief get this or parent dispatch in cmux mode
 *
 * When the channel is in cmux mode, return the dispatch of current channel.
 * When the channel is a sub-channel of cmux mode, return the dispatch of
 * parent channel, which is in cmux mode. Otherwise, return NULL.
 *
 * @param th        command mode engine, must be valid
 * @return
 *      - dispatch of current or parent channel, which is in cmux mode
 *      - NULL if neither current nor parent channel is in cmux mode
 */
atDispatch_t *atCmdGetDispatchInCmux(atCmdEngine_t *th);

/**
 * @brief set the binded SIM
 *
 * @param th        command mode engine, must be valid
 * @param sim       SIM number
 */
void atCmdSetSim(atCmdEngine_t *th, uint8_t sim);

/**
 * @brief get the binded SIM
 *
 * @param th        command mode engine, must be valid
 * @return      the binded SIM number
 */
uint8_t atCmdGetSim(atCmdEngine_t *th);

/**
 * @brief get the parent dispatch
 *
 * @param th        command mode engine, must be valid
 * @return      the parent dispatch
 */
atDispatch_t *atCmdGetDispatch(atCmdEngine_t *th);

/**
 * @brief get the pointer of per channel settings
 *
 * It returns the per channel settings, and it can be modified
 * directly.
 *
 * @param th        command mode engine, must be valid
 * @return      per channel settings pointer
 */
atChannelSetting_t *atCmdChannelSetting(atCmdEngine_t *th);

/**
 * @brief check whether it is the first infomation text
 *
 * Depends on AT channel setting, there are extra characters for
 * the first information text.
 *
 * @param th        command mode engine, must be valid
 * @return
 *      - true if it is the first information text, that is there
 *        are no information text is output before.
 *      - false if not
 */
bool atCmdIsFirstInfoText(atCmdEngine_t *th);

/**
 * @brief set the first infomation text flag
 *
 * @param th        command mode engine, must be valid
 * @param is_first  the first information text flag
 */
void atCmdSetFirstInfoText(atCmdEngine_t *th, bool is_first);

/**
 * @brief check whether the command list is empty
 *
 * A command line will contain multiple AT commands. This will check
 * whether the command list is empty.
 *
 * @param th        command mode engine, must be valid
 * @return
 *      - true if the command list is empty
 *      - false if not
 */
bool atCmdListIsEmpty(atCmdEngine_t *th);

/**
 * @brief get the channel index
 *
 * Channel index is an internal index for each channel. Application
 * shouldn't assume the meaning of the channel index. Also, there are
 * no API to find channel by index.
 *
 * @param th        command mode engine, must be valid
 * @return      the internal channel index
 */
uint8_t atCmdChannelIndex(atCmdEngine_t *th);

/**
 * @brief increase reference for command engine
 *
 * In case that the command engine is needed for certain asynchronous
 * handling, this should be called to indicate that the command engine
 * shouldn't be deleted. After the asynchronous handling is done,
 * \p atCmdDeceaseRef should be called to indicate the command engine
 * is no longer needed.
 *
 * @param th        command mode engine, must be valid
 */
void atCmdIncreaseRef(atCmdEngine_t *th);

/**
 * @brief decrease reference for command engine
 *
 * @param th        command mode engine, must be valid
 */
void atCmdDeceaseRef(atCmdEngine_t *th);

/**
 * @brief indicate start of URC output
 *
 * URC output may be buffered, and it may be needed to drop oldest URC
 * at buffer full. This indicates the start of URC output, to keep record
 * of starting position of each URC in buffer.
 *
 * @param th        command mode engine, must be valid
 */
void atCmdUrcStart(atCmdEngine_t *th);

/**
 * @brief indicate end of URC output
 *
 * @param th        command mode engine, must be valid
 */
void atCmdUrcStop(atCmdEngine_t *th);

/**
 * @brief write URC data to AT command mode engine
 *
 * The URC data will be buffered, and output at \p atCmdUrcFlush.
 *
 * @param th        command mode engine, must be valid
 * @param data      data pointer to be write, can't be NULL is size is not 0
 * @param size      data size
 */
void atCmdUrcWrite(atCmdEngine_t *th, const void *data, size_t size);

/**
 * @brief clear (discard) all buffered URC data
 *
 * @param th        command mode engine, must be valid
 */
void atCmdUrcClear(atCmdEngine_t *th);

/**
 * @brief flush (output) all buffered URC data
 *
 * @param th        command mode engine, must be valid
 */
void atCmdUrcFlush(atCmdEngine_t *th);

/**
 * @brief call async function in worker thread
 *
 * \p call will be executed in AT command channel worker thread. So, don't
 * call AT response APIs in \p call.
 *
 * @param th        command mode engine, must be valid
 * @param call      async call function
 * @param ctx       async call context
 */
void atCmdWorkerCall(atCmdEngine_t *th, osiCallback_t call, void *ctx);

/**
 * @brief create AT data mode engine
 *
 * @param dispatch  parent dispatch of the data mode engine
 * @return
 *      - data mode engine pointer
 *      - NULL if out of memory
 */
atDataEngine_t *atDataEngineCreate(atDispatch_t *dispatch);

/**
 * @brief delete AT data mode engine
 *
 * @param th        data mode engine, must be valid
 */
void atDataEngineDelete(atDataEngine_t *th);

/**
 * @brief push data to data engine
 *
 * It will be called by dispatch. When dispatch received data, dispatch
 * will push the data to engine.
 *
 * @param th        data mode engine, must be valid
 * @param data      data to be pushed, can't be NULL if \p size is not 0.
 * @param size      data size
 * @return
 *      - consumed data size
 *      - -1 if data engine is busy
 */
int atDataPushData(atDataEngine_t *th, const void *data, size_t size);

/**
 * @brief write data to data mode engine
 *
 * When the parent dispatch is not in data mode, nothing will be done.
 *
 * It will wait all data are written (maybe in buffer of AT device).
 *
 * @param th        data mode engine, must be valid
 * @param data      data pointer to be write, can't be NULL is size is not 0
 * @param size      data size
 */
void atDataWrite(atDataEngine_t *th, const void *data, size_t size);

/**
 * @brief set data mode engine to PPP mode
 *
 * @param th        data mode engine, must be valid
 * @param ppp       the PPP context pointer
 */
void atDataSetPPPMode(atDataEngine_t *th, void *ppp);
/* @brief Exit data mode engine to PPP mode
 *
 * @param th        data mode engine, must be valid
 */
void atDataExitPPPMode(atDataEngine_t *th);

/**
 * @brief set data mode engine to bypass mode
 *
 * @param th        data mode engine, must be valid
 * @param cb        callback when there are input data
 * @param cb_ctx    callback context
 * @return
 *      - true on success
 *      - false on invalid parameter
 */
void atDataSetBypassMode(atDataEngine_t *th, atDataBypassCB_t cb, void *cb_ctx);

/**
 * @brief check whether in PPP mode
 *
 * @param th        data mode engine, must be valid
 * @return
 *      - true if in PPP mode
 *      - false if not in PPP mode
 */
bool atDataIsPPPMode(atDataEngine_t *th);

/**
 * @brief check whether in bypass mode
 *
 * @param th        data mode engine, must be valid
 * @return
 *      - true if in bypass mode
 *      - false if not in bypass mode
 */
bool atDataIsBypassMode(atDataEngine_t *th);

/**
 * @brief get the parent dispatch
 *
 * @param th        data mode engine, must be valid
 * @return      the parent dispatch
 */
atDispatch_t *atDataGetDispatch(atDataEngine_t *th);

/**
 * @brief get the PPP context
 *
 * @param th        data mode engine, must be valid
 * @return      the PPP context pointer
 */
void *atDataEngineGetPppSession(atDataEngine_t *th);
/**
 * @brief clear the pppsession pointer
 *
 * @param th  data mode engine, must be valid
 *
 */
void atDataClearPPPSession(atDataEngine_t *th);

/**
 * @brief create AT cmux mode engine
 *
 * @param dispatch  parent dispatch of the cmux mode engine
 * @return
 *      - cmux mode engine pointer
 *      - NULL if out of memory
 */
atCmuxEngine_t *atCmuxEngineCreate(atDispatch_t *dispatch, const atCmuxConfig_t *cfg);

/**
 * @brief delete AT cmux mode engine
 *
 * @param th        cmux mode engine, must be valid
 */
void atCmuxEngineDelete(atCmuxEngine_t *th);

/**
 * @brief push data to cmux engine
 *
 * It will be called by dispatch. When dispatch received data, dispatch
 * will push the data to engine.
 *
 * @param th        cmux mode engine, must be valid
 * @param data      data to be pushed, can't be NULL if \p size is not 0.
 * @param size      data size
 * @return
 *      - consumed data size
 *      - -1 if cmux engine is busy
 */
int atCmuxPushData(atCmuxEngine_t *th, const void *data, size_t size);

/**
 * @brief write data to specified DLC of AT cmux mode engine
 *
 * It will wait all data are written (maybe in buffer of AT device).
 * The data will be sent in UIH packet.
 *
 * @param th        cmux mode engine, must be valid
 * @param data      data pointer to be write, can't be NULL is size is not 0
 * @param size      data size
 */
void atCmuxDlcWrite(atCmuxEngine_t *th, const void *data, size_t size, int dcli);

/**
 * @brief get the cmux mode engine current configuration
 *
 * @param th        cmux mode engine, must be valid
 * @return      the cmux mode engine current configuration
 */
atCmuxConfig_t atCmuxGetConfig(atCmuxEngine_t *th);

/**
 * @brief get max frame size of AT cmux mode engine
 *
 * @param th        cmux mode engine, must be valid
 * @return
 *      - max frame size
 */
int atCmuxMaxFrameSize(atCmuxEngine_t *th);

/**
 * @brief get device of AT cmux mode engine
 *
 * @param th        cmux mode engine, must be valid
 * @return
 *      - cmux configuration
 */
atDevice_t *atCmuxGetDevice(atCmuxEngine_t *th);

/**
 * @brief get the parent dispatch
 *
 * @param th        cmux mode engine, must be valid
 * @return      the parent dispatch
 */
atDispatch_t *atCmuxGetDispatch(atCmuxEngine_t *th);

/**
 * @brief callback function for async AT command
 *
 * For async command, the callback shall be registered though \a cfwRequestUTI
 * to wait CFW event with specified UTI, or through \a atSetPendingIdCmd
 * to wait speficied event ID. When the matching event is arrived, this
 * callback will be called (and the registration will be removed automatically).
 */
typedef void (*atCommandAsyncCB_t)(atCommand_t *cmd, const osiEvent_t *event);

/**
 * @brief register an ID pending command
 *
 * When the event with specified ID arrived, the callback will be called.
 *
 * @param cmd       the AT command context
 * @param id        event ID
 * @param handler   the callback to be called
 * @return
 *      - true if success
 *      - false on error. Refer to \a osiEventDispatchRegister
 */
bool atSetPendingIdCmd(atCommand_t *cmd, uint32_t id, atCommandAsyncCB_t handler);

/**
 * @brief init at device as an alarm owner
 *
 * @param device    the AT device
 */
void atAlarmInit();

/**
 * @brief schedule a callback running in AT thread
 *
 * It is just \p osiThreadCallback with AT thread as parameter.
 *
 * @param cb        callback to be executed
 * @param cb_ctx    callback context
 */
void atEngineCallback(osiCallback_t cb, void *ctx);

/**
 * @brief find the channel in data mode
 *
 * Now only one channel can be set in data mode (that is, PPP). This will
 * find the data mode channel if exists. When there are no channel in data
 * mode it will return NULL.
 *
 * @return
 *      - the channel in data mode if exists
 *      - NULL if no channels are in data mode
 */
atDispatch_t *atFindDataChannel(void);

/**
 * @brief AT dispatch mode switch cause
 */
typedef enum atModeSwitchCause
{
    /**
     * @brief start data mode
     */
    AT_MODE_SWITCH_DATA_START,
    /**
     * @brief end data mode
     */
    AT_MODE_SWITCH_DATA_END,
    /**
     * @brief end data PPP mode
     *
     * Refer to \p atDispatchEndDataPppMode.
     */
    AT_MODE_SWITCH_DATA_PPP_END,
    /**
     * @brief escape from data mode to command line mode
     *
     * The data engine will be kept, and push data to command line engine.
     * Typical usage is to process command temporally.
     */
    AT_MODE_SWITCH_DATA_ESCAPE,
    /**
     * @brief resume data mode from escaped command line mode
     */
    AT_MODE_SWITCH_DATA_RESUME,
} atModeSwitchCause_t;

void atEngineSchedule(osiCallback_t cb, void *cb_ctx);
void atEngineModeSwitch(atModeSwitchCause_t cause, atDispatch_t *d);
void atEngineSetDeviceAutoSleep(bool enabled);
void atMemFreeLater(void *ptr);
void atMemUndoFreeLater(void *ptr);
void atEngineStart(void);
osiThread_t *atEngineGetThreadId(void);
bool atEventRegister(uint32_t id, osiEventHandler_t handler);
bool atEventsRegister(uint32_t id, ...);
void AT_SetAsyncTimerMux(atCmdEngine_t *cmd, uint32_t timeout);
bool atCmdEngineIsValid(atCmdEngine_t *cmd);

/**
 * @brief whether there are commands running in any channel
 *
 * When channel is not in command mode, the *hold* command won't be regarded
 * as running command. For example, when a channel enters data mode by ATD
 * command, the ATD is not regards as running command even it is not finished.
 *
 * Usually, it can be use a hint for not blocking at engine.
 *
 * @return
 *      -true if there are commands running in any channel
 *      -false if not
 */
bool atEngineIsCmdRunning(void);

/**
 * @brief AT dispatch ModeSwitch handler function
 *
 * @param cmd       current ATdispatch pointer
 * @param cause     ModeSwitch cause
 */
typedef void (*atDispatchModeSwitchHandler_t)(atDispatch_t *th, atModeSwitchCause_t cause);

/**
 * @brief set ModeSwitch handler
 *
 * Set AT ModeSwitch handler. At ModeSwitch, \p handler will be
 * called.
 *
 * When \p handler is NULL, the previously registered ModeSwitch handler will
 * be cleared, through it is not needed in typical cases.
 *
 * This *must* be called in AT command handler.
 *
 *
 * @param th        the dispatch pointer, must be valid
 * @param handler   ModeSwitch  handler
 */
void atDispatchSetModeSwitchHandler(atDispatch_t *th, atDispatchModeSwitchHandler_t handler);

/**
 * @brief invoke ModeSwitch handler
 *
 * Invoke AT ModeSwitch handler. Call handler.
 *
 *
 * @param th        the dispatch pointer, must be valid
 * @param cause   ModeSwitch  cause
 */
void atDispatchInvokeModeSwitchHandle(atDispatch_t *th, atModeSwitchCause_t cause);

/**
 * @brief default ppp ModeSwitch handler
 *
 * @param th        the dispatch pointer, must be valid
 * @param cause   ModeSwitch  cause
 */
void pppAtModeSwitchHandler(atDispatch_t *th, atModeSwitchCause_t cause);

/**
 * @brief create usb serial AT channel
 *
 * @param name  the usb serial name
 * @return
 *      - succeed create the channel or fail
 */
bool atEngineCreateUSerialChannel(uint32_t name);

/**
 * @brief create uart AT channel
 *
 * @param name  the UART name
 * @return
 *      - succeed create the channel or fail
 */
bool atEngineCreateUartChannel(uint32_t name);

/**
 * @brief get AT device ready status
 *
 * @param th        AT device pointer, must be valid
 * @return
 *      - ture device ready or fail not ready
 */
bool atDispatchGetDeviceReadyStatus(atDispatch_t *th);

/**
 * @brief set AT device ready status
 *
 * @param th        AT device pointer, must be valid
 * @param bReady    Whether device is ready;
 * @return
 *      NONE
 */
void atDispatchSetDeviceReadyStatus(atDispatch_t *th, bool bReady);

#ifdef __cplusplus
}
#endif
#endif
