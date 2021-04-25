#ifndef BT_HCI_TRACE_H 
#define BT_HCI_TRACE_H

/*---------------------------------------------------------------------------
 * L2CAP_MTU constant
 *
 *     Defines the largest receivable L2CAP data packet payload, in bytes.
 *     This limitation applies only to packets received from the remote device.
 *     When L2CAP_FLOW_CONTROL is enabled, channels requesting Flow & Error
 *     Control mode are not limited by this constant.
 *
 *     This constant also affects the L2CAP Connectionless MTU. Some profiles
 *     require a minimum Connectionless MTU that is greater than the L2CAP
 *     minimum of 48 bytes. When implementing such a profile it is important
 *     that the L2CAP_MTU constant be at least as large as the specified
 *     Connectionless MTU size.
 *
 *     This value may range from 48 to 65529. The default value is 672 bytes.
 */
#ifndef L2CAP_MTU
#define L2CAP_MTU 672
#endif

#if L2CAP_MTU < 48 || L2CAP_MTU > 65529
#error L2CAP_MTU must be between 48 and 65529.
#endif

//#ifdef BT_TRACE_HCI_TO_CAP//@xu.wu 20.9.5
#define WIRESHARK_HCI_HEAD_LEN 12
#define MAX_DUMP_DATA_NUM (L2CAP_MTU+30) //UARTTRAN HEADER:12, TYPE:1, ACL HEADER:4, L2CAP HEADER&FCS MAX:10

#define HCI_COMMAND         (uint8)0x01
#define HCI_ACL_DATA        (uint8)0x02
#define HCI_SCO_DATA        (uint8)0x03
#define HCI_EVENT           (uint8)0x04


void hci_trace_init(void);

#endif

