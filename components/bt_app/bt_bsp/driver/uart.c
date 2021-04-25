/******************************************************************************
** File Name:      uart.c
** Author:
** DATE:           2012/1/4
** Copyright:      2012 Spreatrum, Incoporated. All Rights Reserved.
** Description:
******************************************************************************/
#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('B', 'T', 'U', 'A')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "osi_log.h"
#include <stdlib.h>
#include "osi_api.h"
#include "hwregs.h"
#include "drv_names.h"
#include "drv_uart.h"
#include "bt_abs.h"
#include "sci_types.h"
//#include "hci_mem.h"
#include "bt_hci_dev.h"
#include "bt_app.h"


#define PACKET_TYPE 0
#define EVT_HEADER_EVENT 1
#define EVT_HEADER_SIZE 2

#define RX_STAGE_MSG_TYPE 0
#define RX_STAGE_MSG_HEADER 1
#define RX_STAGE_MSG_DATA 2

#define BT_RX_BUF_SIZE 1024

#define BT_TX_BUF_SIZE 512
static uint8_t  s_uart_tx_buf[BT_TX_BUF_SIZE<<1];
static uint16_t s_uart_tx_left_num = 0;
static uint16_t s_uart_tx_total_size = 0;

extern osiMessageQueue_t *g_bt_driver_msg;
static bool s_is_controller_bqb = false;
void (*BT_UART_FUCTION)(char *, unsigned int) = NULL;
static int re_flag = 0;

extern void bt_send_data_event(unsigned char type, unsigned char *data, unsigned short length);
extern void HCI_TRACE_RecordWireSharkLog(UINT8 Type, UINT8 Direction, void *PBuf, unsigned int Len);

void UART_SetControllerBqbMode(BOOLEAN is_ctrlr_bqb_mode)
{
    OSI_LOGI(0, "[BT UART] SetControllerBqbMode %d", is_ctrlr_bqb_mode);
    s_is_controller_bqb = is_ctrlr_bqb_mode;
}

BOOLEAN UART_GetControllerBqbMode(void)
{
    // OSI_LOGI(0,"uart.c:UART_GetControllerBqbMode->mode:%d",(s_is_controller_bqb==TRUE));
    return s_is_controller_bqb;
}

typedef struct
{
    bool inited;
    bt_hci_dev_t *hci_dev;
    osiMutex_t *read_lock;
    osiMutex_t *write_lock;
    uint8_t rx_packet[BT_RX_BUF_SIZE];
    unsigned rx_size;
    unsigned rx_len;
    uint8_t rx_stage;
} btUart_t;

static btUart_t gBtUart = {0};

static void prv_uart_rx_avail_cb(bt_hci_dev_t *d, void *param)
{
    OSI_LOGD(0, "[BT UART] rx avail");
    BT_INT_MSG_T *msg_ptr = (BT_INT_MSG_T *)malloc(sizeof(BT_INT_MSG_T));
    SCI_ASSERT(NULL != msg_ptr);
    msg_ptr->message[0] = 0;
    msg_ptr->message[1] = OS_MSG_EXTEND_TRIG_DATA;
    msg_ptr->message[2] = 0;
    msg_ptr->message[3] = 0;
    bt_SendMsgToSchedTask(BT_INT_HANDLE_MSG, msg_ptr);
}

void UartDrv_SetUartId(uint8 id)
{
}

void UartDrv_Configure(uint32 baudrate)
{
    bt_hci_dev_set_baud(baudrate);
}
int GET_RE_FLAG()
{
    return re_flag;
}
void SET_RE_FLAG(int flag)
{
    re_flag = flag;
}

//extern void app_hci_evt_patch(unsigned char type, unsigned char *data, unsigned short length);
static bool prv_rx_parse_locked(void)
{
    btUart_t *d = &gBtUart;

    if (d->rx_stage == RX_STAGE_MSG_TYPE)
    {
        if (bt_hci_dev_read(d->hci_dev, &d->rx_packet[0], 1) != 1)
            return false;

        uint8_t msg_type = d->rx_packet[0];
        if (msg_type == HCI_TYPE_EVT)
        {
            d->rx_size = EVT_HEADER_SIZE + 1;
        }
        else if (msg_type == HCI_TYPE_ACL)
        {
            d->rx_size = 4 + 1;
        }
        else
        {
            OSI_LOGW(0, "[BT UART] unknown BT msg type %d", msg_type);
            d->rx_stage = RX_STAGE_MSG_TYPE;
            return true;
        }

        d->rx_len = 1;
        d->rx_stage = RX_STAGE_MSG_HEADER;
        return true;
    }

    if (d->rx_stage == RX_STAGE_MSG_HEADER)
    {
        unsigned size = d->rx_size - d->rx_len;
        int rsize = bt_hci_dev_read(d->hci_dev, &d->rx_packet[d->rx_len], size);
        if (rsize < 0)
            return false;

        d->rx_len += rsize;
        if (rsize != size)
            return false;

        uint8_t msg_type = d->rx_packet[0];
        if (msg_type == HCI_TYPE_EVT)
        {
            d->rx_size += d->rx_packet[EVT_HEADER_SIZE];
        }
        else if (msg_type == HCI_TYPE_ACL)
        {
            d->rx_size += (d->rx_packet[4] << 8) + d->rx_packet[3];
        }

        if (d->rx_size > BT_RX_BUF_SIZE)
        {
            OSI_LOGXW(OSI_LOGPAR_M, 0, "[BT UART] invalid packet size %*s",
                      d->rx_len, d->rx_packet);
            d->rx_stage = RX_STAGE_MSG_TYPE;
            return true;
        }

        d->rx_stage = RX_STAGE_MSG_DATA;
        return true;
    }

    if (d->rx_stage == RX_STAGE_MSG_DATA)
    {
        unsigned size = d->rx_size - d->rx_len;
        int rsize = bt_hci_dev_read(d->hci_dev, &d->rx_packet[d->rx_len], size);
        if (rsize < 0)
            return false;

        d->rx_len += rsize;
        if (rsize != size)
            return false;

        uint8_t msg_type = d->rx_packet[0];
        OSI_LOGXI(OSI_LOGPAR_IIIM, 0, "[BT UART] read packet %d, BQB=%d, re=%d %*s",
                  d->rx_size, UART_GetControllerBqbMode(), GET_RE_FLAG(),
                  OSI_MIN(unsigned, d->rx_size, 10), d->rx_packet);
        HCI_TRACE_RecordWireSharkLog(msg_type, 1, &d->rx_packet[1], d->rx_size - 1);

        if ((UART_GetControllerBqbMode() == TRUE) && (BT_UART_FUCTION != NULL))
        {
            BT_UART_FUCTION((char *)d->rx_packet, d->rx_size);
            SET_RE_FLAG(0);
            return false;
        }

        uint8_t *send_data = (uint8_t *)malloc(d->rx_size - 1);
        if (send_data == NULL)
        {
            OSI_LOGW(0, "[BT UART] out of memory, packet drop");
        }
        else
        {
            memcpy(send_data, &d->rx_packet[1], d->rx_size - 1);
            // send_data will be free inside
            bt_send_data_event(msg_type, send_data, d->rx_size - 1);

            // patch hci evt
            //app_hci_evt_patch(msg_type, send_data, d->rx_size - 1);
        }

        d->rx_stage = RX_STAGE_MSG_TYPE;
        return true;
    }

    return false;
}

void UartDrv_Rx(void)
{
    if (!gBtUart.inited)
        return;

    btUart_t *d = &gBtUart;
    osiMutexLock(d->read_lock);

    while (prv_rx_parse_locked())
        ;

    osiMutexUnlock(d->read_lock);
}

uint16_t UART_Write(const uint8_t *buffer, uint16_t length)
{
//    OSI_LOGI(0, "[BT UART] uart_write %d", gBtUart.inited);
    if (!gBtUart.inited)
        return 0;

    OSI_LOGXI(OSI_LOGPAR_IM, 0, "[BT UART] write %d %*s", length, OSI_MIN(unsigned, length, 10), buffer);
    HCI_TRACE_RecordWireSharkLog(buffer[0], 0, (void *)&buffer[1], length - 1);

    btUart_t *d = &gBtUart;
    osiMutexLock(d->write_lock);

    int trans = bt_hci_dev_write(d->hci_dev, buffer, length);

    osiMutexUnlock(d->write_lock);

    return trans < 0 ? 0 : (uint16_t)trans;
}

uint16_t UartDrv_Tx(const uint8_t *buffer, uint16_t length)
{

    OSI_LOGI(0, "[BT UART] transmit data enter: left_size %u,buffer[0]=%d,buffer[%d]=%d", s_uart_tx_left_num,buffer[0],length-1,buffer[length-1]);  

    memcpy((s_uart_tx_buf + s_uart_tx_left_num), buffer, length);

    if(s_uart_tx_left_num == 0)
    {
        if(buffer[0]==1)
        {
            s_uart_tx_total_size = buffer[3] + 4;
        }
        else if(buffer[0]==2)
        {
            s_uart_tx_total_size = ((uint16_t)buffer[4] << 8) + buffer[3] + 5;
        }
        else
        {
            s_uart_tx_total_size = 0;
        }
    
    }

    if((s_uart_tx_left_num + length) == s_uart_tx_total_size)
    {
        UART_Write(s_uart_tx_buf,s_uart_tx_total_size);
        s_uart_tx_left_num = 0;
        s_uart_tx_total_size = 0;
    }
    else
    {
        s_uart_tx_left_num += length;
    }
    
    OSI_LOGI(0, "[BT UART] transmit data exit: left_size %u,total_size %d", s_uart_tx_left_num,s_uart_tx_total_size);

    return 0;
}

void SIO_BT_Write(uint8 *buffer, uint count)
{
    UART_Write(buffer, count);
}

void UartDrv_Stop(void)
{
    OSI_LOGI(0, "[BT UART] UartDrv_Stop %d", gBtUart.inited);
    if (gBtUart.inited)
    {
        btUart_t *d = &gBtUart;
        d->inited = false;

        // wait read/write done
        osiMutexLock(d->read_lock);
        osiMutexLock(d->write_lock);

        // delete uart instance
        bt_hci_dev_close(d->hci_dev);

        osiMutexUnlock(d->write_lock);
        osiMutexUnlock(d->read_lock);

        // delete mutex after it is unlocked
        osiMutexDelete(d->read_lock);
        osiMutexDelete(d->write_lock);
        d->read_lock = NULL;
        d->write_lock = NULL;
        d->hci_dev = NULL;
    }
}

bool UartDrv_Start(void)
{
    OSI_LOGI(0, "[BT UART] UartDrv_Start %d", gBtUart.inited);

    if (gBtUart.inited)
        return true;

    btUart_t *d = &gBtUart;
    d->rx_stage = RX_STAGE_MSG_TYPE;
    d->read_lock = osiMutexCreate();
    d->write_lock = osiMutexCreate();
    if (d->read_lock == NULL || d->write_lock == NULL)
    {
        OSI_LOGE(0, "[BT UART] start create lock fail");
        goto fail;
    }

    d->hci_dev = bt_hci_dev_open(prv_uart_rx_avail_cb, NULL);
    if (d->hci_dev == NULL)
        goto fail;

    d->inited = true;
    return true;

fail:
    osiMutexDelete(d->read_lock);
    osiMutexDelete(d->write_lock);
    d->read_lock = NULL;
    d->write_lock = NULL;
    d->hci_dev = NULL;
    return false;
}

void UART_SetBaudSpeed(uint32 port, uint32 baudrate)
{
    OSI_LOGI(0, "[BT] UART_SetBaudSpeed port=%d  baudrate=%d", port, baudrate);
    // TODO need change baudrate ???
}
