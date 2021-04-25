// //////////////////////////////////////////////////////////////////////////////
//
// //////////////////////////////////////////////////////////////////////////////
#ifndef __AT_CMD_ALIC_H__
#define __AT_CMD_ALIC_H__
#include "ats_config.h"
#include "at_engine.h"
#include "at_command.h"

#ifdef CONFIG_AT_ALIC_SUPPORT

#define AT_ALIC_TOPIC_LEN 64 //50 - 128
#define AT_ALIC_OUT_MSG_LEN 256
#define AT_ALIC_MSG_LEN 1024
#define AT_ALIC_INCOMING_MSG_LEN 4096

#define AT_ALIC_RX_BUFFER_SIZE 1500
#define AT_ALIC_TX_BUFFER_SIZE 1500

typedef enum
{
    AT_ALIC_INIT,
    AT_ALIC_AUTH,
    AT_ALIC_CONNECTED,
    AT_ALIC_DISCONNECTED,
} AT_ALIC_STATUS;

#define AT_ALIC_RECV_PACKET_TIMEOUT 200
#define AT_ALIC_RECV_PACKET_DELAY 1000

#define AT_ALIC_KEEPALIVE_MIN 30
#define AT_ALIC_KEEPALIVE_MAX 1200

#endif
#endif
