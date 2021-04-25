 /*
 * Copyright (c) 2020, UNISOC Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "osi_api.h"
#include <stdlib.h>
#include <string.h>
#include "osi_log.h"
#include "bluetooth/bt_types.h"
#include "bluetooth/bt_driver.h"
//#include "hci_mem.h"
//#include "hci_debug.h"


#define SPRD_DP_RW_REG_SHIFT_BYTE 14
#define SPRD_DP_DMA_READ_BUFFER_BASE_ADDR 0x40280000
#define SPRD_DP_DMA_UARD_SDIO_BUFFER_BASE_ADDR (SPRD_DP_DMA_READ_BUFFER_BASE_ADDR + (1 << SPRD_DP_RW_REG_SHIFT_BYTE))
#define WORD_ALIGN 4
#define BT_RXSEM_NUM 100
#define BT_SIPC_CHNL_ID       15


#define LE_ADV_REPORT_COUNT 40
#define LE_ADV_REPORT_SIZE 50
#define UINT_MAX 65535
#define BT_STREAM_TO_u16(u16,p)   {u16 = 0;u16 = (uint16)((*(p + 1)) << 8) + (uint16)(*p);}

extern uint16_t UART_Write(const uint8_t *buffer, uint16_t length);
extern void UartDrv_Rx(void);
osiMessageQueue_t *g_bt_driver_msg = NULL;
bt_driver_callback_t *driver_callback;


void bt_send_data_event(unsigned char type, unsigned char *data, unsigned short length)
{
    if (driver_callback != NULL)
    {
        driver_callback->recv_cb(type, data, length);
    }
}
#if 0
void bt_send_data_event(unsigned char type, unsigned short hci_handle, unsigned short datalen, unsigned char *buffer)
{
    hci_data_t *hci_data = (hci_data_t*)osi_malloc(sizeof(hci_data_t));
    hci_data->data = buffer;
    hci_data->transport = type;
    hci_data->len = datalen;
    hci_data->size = (datalen + sizeof(hci_data_t));
    hci_data->flags = HCI_BUF_INCOMING_PACKET_MASK;
    hci_data->hci_handle_flags = hci_handle;
    hci_data->hci_type = type;
    hci_data->tail = NULL;
    
if(0)
{ // trace
    uint8_t trac_len = (datalen > 10)?(10):(datalen);
    for(uint8_t i=0; i<trac_len; i++)
    {
        OSI_LOGI(0, "BT UART READ: buf[%d]:0x%x", i, buffer[i]);
    }
}


    if (driver_callback != NULL)
    {
        driver_callback->recv_cb(type, hci_data, 0);
    }

}
#endif

int bt_driver_init(const bt_driver_callback_t *p_cb)
{
    driver_callback = (bt_driver_callback_t*)p_cb;

    if (driver_callback != NULL)
    {
        driver_callback->state_changed_callback(BT_DRV_STATE_ON);
    }
//    return bt_driver_task_create();

    return 0;
}
extern bool UART_GetControllerBqbMode(void);
void bt_data_send(unsigned char type, void *p_buf, unsigned int length)
{
    OSI_LOGI(0,"bt_data_send type: %d,bqb_mode=%d",type,UART_GetControllerBqbMode());
    if(UART_GetControllerBqbMode()==TRUE)
    {
        return;
    }

    uint8_t *ph2c = NULL;
    uint16_t bytesWritten;
    switch (type)
    {
        case HCI_TYPE_CMD:
            ph2c = (uint8_t*)malloc((1+length));
            memcpy(&ph2c[1], p_buf, length);
            ph2c[0] = type;
            //HCI_TRACE_RecordWireSharkLog(type, 0, p_buf, length);
    
            bytesWritten = UART_Write(ph2c, (length+1));
            free(ph2c);

            if(bytesWritten < (length+1))
            {
                // error, not all bytes have been written.
                OSI_LOGE(0,"bt_data_send, error");
            }
            //HCIDUMP_EX(HCI_TYPE_CMD, 1, pdu->data, pdu->len);
    //        hci_cmd_mem_free(p_buf);
            break;
        case HCI_TYPE_ACL:
            #if 0
            ph2c = (uint8_t*)osi_malloc((5+length));
            memcpy(&ph2c[5], p_buf, length);
            ph2c[0] = type;
            ph2c[1] = (uint8_t)(handle & 0xFF);
            ph2c[2] = (uint8_t)(handle >> 8);
            ph2c[3] = (uint8_t)(length & 0xFF);
            ph2c[4] = (uint8_t)(length >> 8);
            #else
            ph2c = (uint8_t*)malloc((1+length));
            memcpy(&ph2c[1], p_buf, length);
            ph2c[0] = type;
            #endif

            //HCI_TRACE_RecordWireSharkLog(type, 0, &ph2c[1], (length+4));

            bytesWritten = UART_Write(ph2c, (length+1));
            free(ph2c);

            if(bytesWritten < (length+1))
            {
                // error, not all bytes have been written.
                OSI_LOGE(0,"bt_data_send, error");
            }
            
//            hci_acl_tx_data_mem_free(p_buf);
            break;
        default:
            OSI_LOGE(0,"Unknown packet type %d",type);
        break;

    }

    if(p_buf != NULL)
    {
        driver_callback->free_cb(p_buf);
        p_buf = NULL;
    }
 //   vPortFree(p_buf);
}

void bt_cleanup(void)
{


}

static const bt_driver_interface_t bt_driver_interface =
{
    .init = bt_driver_init,
    .send = bt_data_send,
    .cleanup = bt_cleanup,
};

const bt_driver_interface_t *get_bt_driver_interface(void)
{
    return &bt_driver_interface;
}

