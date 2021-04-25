#include "sci_types.h"
#include "osi_log.h"
#include "bt_abs.h"
#include "ddb.h"
#include "bt_hci_tra.h"



static uint8 *dumpDataBuf = NULL;
uint8 wireshark_log_mem[MAX_DUMP_DATA_NUM];

#ifdef CONFIG_QUEC_PROJECT_FEATURE
extern uint32 GetHostNvCtrl(void);
#else
extern uint32_t GetHostNvCtrl(void);
#endif

void hci_trace_init(void)
{
    dumpDataBuf = &wireshark_log_mem[0];
    SCI_MEMSET(dumpDataBuf, 0, MAX_DUMP_DATA_NUM);
}


void HCI_TRACE_RecordWireSharkLog(UINT8  Type, UINT8  Direction, void *PBuf, unsigned int Len)
{
    uint16  TempLen = MAX_DUMP_DATA_NUM - WIRESHARK_HCI_HEAD_LEN - 1;

    if ((!((GetHostNvCtrl()) & HOST_NV_CTRL_HCI_CAP)) || (dumpDataBuf == NULL))
    {
	      //OSI_LOGI(0, "[BT] liufeng exit 111 HCI_TRACE_RecordWireSharkLog");
		return;
	}
	
	dumpDataBuf[WIRESHARK_HCI_HEAD_LEN] = Type;
	switch (Type)
	{
		case HCI_TYPE_CMD:
		case HCI_TYPE_EVT:
		case HCI_TYPE_ACL:
			if(Len > TempLen)
			{
				Len = TempLen;
			}

			SCI_MEMCPY((UINT8 *)(dumpDataBuf + WIRESHARK_HCI_HEAD_LEN + 1), PBuf, Len);
			break;
		case HCI_TYPE_SCO:
		default:
			OSI_LOGE(0,"Notice packet type %d",Type);
			break;
	}
	
	if(Direction)	//receive data
	{
		dumpDataBuf[2] = 1; 
	}
	else//send data
	{
		dumpDataBuf[2] = 0;
	}

	dumpDataBuf[0] = (Len + 1) & 0xff; // 0 and 1 bytes means payload data len
	dumpDataBuf[1] = ((Len + 1) & 0xff00) >> 8;
	
	SCI_TRACE_DATA(0x01, dumpDataBuf, Len+1+12); 
	
        //OSI_LOGI(0, "[BT] liufeng exit 222 HCI_TRACE_RecordWireSharkLog");

	return;
}

//#endif//@xu.wu 20.9.5

