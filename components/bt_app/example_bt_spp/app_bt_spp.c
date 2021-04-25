#include <assert.h>
#include "stdlib.h"
#include "bluetooth/bluetooth.h"
#include "bluetooth/bt_spp.h"
#include "bt_abs.h"
#include "sci_types.h"
#include "ddb.h"
#include "bt_drv.h"
#include "osi_log.h"
#include "bt_app.h"

extern void app_msg_to_at(unsigned int msg_id, char status, void *data_ptr);

static uint32 s_bt_spp_state = BTSPP_CONNECTION_STATE_DISCONNECTED;
static bdaddr_t s_spp_peer_addr;

static void bt_app_spp_connection_state_callback(btspp_connection_state_t state, bdaddr_t *addr)
{
    OSI_LOGI(0, "[BT] enter bt_app_spp_connection_state_callback state=%d", state);
    if (BTSPP_CONNECTION_STATE_CONNECTED == state)
    {
        app_msg_to_at(ID_STATUS_SPP_CONNECT_RES, BT_SUCCESS, NULL);
    }
    else if (BTSPP_CONNECTION_STATE_DISCONNECTED == state)
    {
        app_msg_to_at(ID_STATUS_SPP_DISCONNECT_RES, BT_SUCCESS, NULL);
    }

    s_bt_spp_state = state;
    
    return;
}

static bool  bt_spp_data_memory_alloc(BT_AT_SPP_DATA **pAtSppData, UINT16 dataLen)
{
    *pAtSppData = malloc(sizeof(BT_AT_SPP_DATA));
    if(*pAtSppData == NULL)
    {
         OSI_LOGI(0, "bt_spp_data_memory_alloc fail");
         return FALSE;
    }
   ((BT_AT_SPP_DATA *)( *pAtSppData))->buf = malloc(dataLen);
    if(((BT_AT_SPP_DATA *)( *pAtSppData))->buf == NULL)
    {
        free(*pAtSppData);
        OSI_LOGI(0, "Spp Data buf malloc error");
        return false;
    }
    return true;
    
}

static UINT8 bt_app_spp_data_recv_callback(UINT16 port, UINT8 *data, UINT16 length)
{
    BT_AT_SPP_DATA *pAtSppData = NULL;

    OSI_LOGI(0, "[BT] enter bt_app_spp_data_recv_callback");

    if (data)
    {
        if (bt_spp_data_memory_alloc(&pAtSppData, length))
        {
            pAtSppData->dataLen = length;
            memcpy(pAtSppData->buf, data, length);
            app_msg_to_at(ID_STATUS_SPP_DATA_RECIEVE_IND, BT_SUCCESS, (void *)pAtSppData);
        }
    }
    else
    {
        OSI_LOGI(0, "[BT]  data is NULL");
    }
    return 0;
}

static UINT8 bt_app_spp_flowctrl_ind_callback(UINT32 result)
{
    OSI_LOGI(0, "[BT] enter bt_app_spp_flowctrl_ind_callback result=%d", result);

    return 0;
}

static UINT8 bt_app_spp_mtu_result_callback(UINT16 frame_size)
{
    OSI_LOGI(0, "[BT] enter bt_app_spp_mtu_result_callback frame_size=%d", frame_size);

    return 0;
}

btspp_callbacks_t btspp_callbacks = {
    bt_app_spp_connection_state_callback,
    bt_app_spp_data_recv_callback,
    bt_app_spp_flowctrl_ind_callback,
    bt_app_spp_mtu_result_callback};

void app_bt_spp_init(void)
{
    bt_spp_callback_register(&btspp_callbacks);
}

uint32 app_bt_spp_state_get(void)
{
    return s_bt_spp_state;
}

bool app_bt_spp_connect_req(bdaddr_t* bdAddr)
{
    if(s_bt_spp_state != BTSPP_CONNECTION_STATE_DISCONNECTED)
        return false;
    
    if(bt_spp_connect(bdAddr) == BT_SUCCESS)
    {
        memcpy(s_spp_peer_addr.bytes, bdAddr->bytes, SIZE_OF_BDADDR);
        return true;
    }

    return false;
}

bool app_bt_spp_disconnect_req(void)
{
    if(s_bt_spp_state != BTSPP_CONNECTION_STATE_DISCONNECTED)
        return false;
    
    if(bt_spp_disconnect(&s_spp_peer_addr) == BT_SUCCESS)
    {
        return true;
    }

    return false;
}

bool app_bt_spp_data_send_req(const uint8 *src_buf, uint32 data_len)
{
    if(s_bt_spp_state != BTSPP_CONNECTION_STATE_DISCONNECTED)
        return false;

    if(bt_spp_data_send(&s_spp_peer_addr, (uint8 *)src_buf, data_len) == BT_SUCCESS)
        return true;

    return false;
}
