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

#ifdef AT_MQTTSN_SUPPORT

#include "osi_api.h"
#include <stdlib.h>

#include <sys/socket.h>

#include "MQTTSNPacket.h"
#include "MQTTSNConnect.h"

#define MQTTSN_DEFAULT_TIMEOUT (10)

//#define malloc COS_MALLOC
//#define free COS_FREE

typedef struct
{
    int sock;
    char *host;
    char *port;
    int addressFamily;
#ifdef LWIP_IPV6_ON
    struct sockaddr_in6 addr;
#else
    struct sockaddr_in addr;
#endif
    size_t addrLen;
} mqttsn_context_t;

typedef struct _sub_topic_map_t mqttsn_sub_topic_map_t;

struct _sub_topic_map_t
{
    uint16_t packetid;
    uint16_t topicid;
    char *topicname;
    uint8_t topictype; // 0 predefined, 1 short, 2 normal
    uint8_t msgtype;   // 0 subscribe // publish
    mqttsn_sub_topic_map_t *next;
};

typedef struct _pub_topic_map_t mqttsn_pub_topic_map_t;
struct _pub_topic_map_t
{
    uint16_t topicid;
    char *topicname;
    uint8_t dup;
    uint8_t qos;
    uint8_t retained;
    uint16_t packetid;
    uint8_t *payload;
    int payloadlen;
    mqttsn_pub_topic_map_t *next;
};

typedef struct _mqttsn_will_t mqttsn_will_t;

struct _mqttsn_will_t
{
    MQTTSNString willtopic;
    MQTTSNString willmsg;
    uint8_t willqos;
    uint8_t willretain;
};

typedef void (*mqttsn_result_callback_t)(void *param, int success);

extern time_t time(time_t *timer);

#endif
