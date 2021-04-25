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

#include "string.h"
#include "netdb.h"

#include "MQTTSNApi.h"
#if !defined(SOCKET_ERROR)
/** error in socket operation */
#define SOCKET_ERROR -1
#endif

mqttsn_context_t g_mqttsn_context;

mqttsn_will_t g_mqttsn_will;

bool g_keep_running = true;

uint16_t g_packet_id = 1;

mqttsn_result_callback_t mqttsn_result_callback;
void *mqttsn_result_callback_param;

mqttsn_sub_topic_map_t *g_sub_topic_map;
mqttsn_pub_topic_map_t *g_pub_topic_map;

MQTTSNPacket_connectData g_connecData;

static uint8_t timeout = MQTTSN_DEFAULT_TIMEOUT;
static time_t last_transmit = 0;

void mqttsn_set_result_callback(mqttsn_result_callback_t handler, void *param)
{
    mqttsn_result_callback = handler;
    mqttsn_result_callback_param = param;
}

void mqttsn_report_result(int result)
{
    if (mqttsn_result_callback)
    {
        mqttsn_result_callback(mqttsn_result_callback_param, result);
    }
}

void mqttsn_set_timeout(int value)
{
    if (value < 1)
    {
        timeout = MQTTSN_DEFAULT_TIMEOUT;
    }
    else
    {
        timeout = value;
    }
}

void mqttsn_add_pub_topic(uint16_t packetid, uint16_t topicid, char *topicname, uint8_t dup, uint8_t qos, uint8_t retained, uint8_t *payload, int payloadlen)
{
    if (g_pub_topic_map == NULL)
    {
        g_pub_topic_map = (mqttsn_pub_topic_map_t *)malloc(sizeof(mqttsn_pub_topic_map_t));
        g_pub_topic_map->packetid = packetid;
        g_pub_topic_map->topicid = topicid;
        g_pub_topic_map->topicname = topicname;
        g_pub_topic_map->dup = dup;
        g_pub_topic_map->qos = qos;
        g_pub_topic_map->retained = retained;
        g_pub_topic_map->payload = payload;
        g_pub_topic_map->payloadlen = payloadlen;
        g_pub_topic_map->next = NULL;
        return;
    }

    mqttsn_pub_topic_map_t *pub_topic_map = g_pub_topic_map;
    while (pub_topic_map)
    {
        if (pub_topic_map->next == NULL)
        {
            mqttsn_pub_topic_map_t *temp = (mqttsn_pub_topic_map_t *)malloc(sizeof(mqttsn_pub_topic_map_t));
            g_pub_topic_map = (mqttsn_pub_topic_map_t *)malloc(sizeof(mqttsn_pub_topic_map_t));
            g_pub_topic_map->packetid = packetid;
            g_pub_topic_map->topicid = topicid;
            g_pub_topic_map->topicname = topicname;
            g_pub_topic_map->dup = dup;
            g_pub_topic_map->qos = qos;
            g_pub_topic_map->retained = retained;
            g_pub_topic_map->payload = payload;
            g_pub_topic_map->payloadlen = payloadlen;
            g_pub_topic_map->next = NULL;
            pub_topic_map->next = temp;
            break;
        }
        pub_topic_map = pub_topic_map->next;
    }
}

mqttsn_pub_topic_map_t *get_mqttsn_pub_topic_map_with_packetid(uint16_t packetid)
{
    mqttsn_pub_topic_map_t *pub_topic_map = g_pub_topic_map;
    while (pub_topic_map)
    {
        if (pub_topic_map->packetid == packetid)
        {
            break;
        }
        pub_topic_map = pub_topic_map->next;
    }
    return pub_topic_map;
}

void mqttsn_free_pub_topic()
{
    while (g_pub_topic_map != NULL)
    {
        mqttsn_pub_topic_map_t *pub_topic_map;
        pub_topic_map = g_pub_topic_map;
        g_pub_topic_map = g_pub_topic_map->next;
        free(pub_topic_map);
    }
}

void mqttsn_free_sub_topic()
{
    while (g_sub_topic_map != NULL)
    {
        mqttsn_sub_topic_map_t *sub_topic_map;
        sub_topic_map = g_sub_topic_map;
        g_sub_topic_map = g_sub_topic_map->next;
        free(sub_topic_map);
    }
}

void mqttsn_add_sub_topic(uint16_t packetid, uint16_t topicid, char *topicname, uint8_t topictype, uint8_t msgstype)
{
    if (g_sub_topic_map == NULL)
    {
        g_sub_topic_map = (mqttsn_sub_topic_map_t *)malloc(sizeof(mqttsn_sub_topic_map_t));
        g_sub_topic_map->packetid = packetid;
        g_sub_topic_map->topicid = topicid;
        g_sub_topic_map->topicname = topicname;
        g_sub_topic_map->topictype = topictype;
        g_sub_topic_map->msgtype = msgstype;
        g_sub_topic_map->next = NULL;
        return;
    }

    mqttsn_sub_topic_map_t *sub_topic_map = g_sub_topic_map;
    while (sub_topic_map)
    {
        if (sub_topic_map->next == NULL)
        {
            mqttsn_sub_topic_map_t *temp = (mqttsn_sub_topic_map_t *)malloc(sizeof(mqttsn_sub_topic_map_t));
            temp->packetid = packetid;
            temp->topicid = topicid;
            temp->topicname = topicname;
            temp->topictype = topictype;
            temp->msgtype = msgstype;
            temp->next = NULL;
            sub_topic_map->next = temp;
            break;
        }
        sub_topic_map = sub_topic_map->next;
    }
}

void mqttsn_remove_sub_topic(char *topicname)
{
    if (g_sub_topic_map == NULL)
    {
        return;
    }

    mqttsn_sub_topic_map_t *sub_topic_map = g_sub_topic_map;
    if (strcmp(g_sub_topic_map->topicname, topicname))
    {
        g_sub_topic_map = g_sub_topic_map->next;
        free(sub_topic_map->topicname);
        free(sub_topic_map);
        return;
    }

    while (sub_topic_map->next)
    {
        if (strcmp(sub_topic_map->next->topicname, topicname))
        {
            mqttsn_sub_topic_map_t *temp = sub_topic_map->next;
            sub_topic_map->next = sub_topic_map->next->next;
            free(temp->topicname);
            free(temp);
        }
    }
}

mqttsn_sub_topic_map_t *mqttsn_get_sub_topic_with_packetid(uint16_t packetid)
{
    mqttsn_sub_topic_map_t *sub_topic_map = g_sub_topic_map;
    while (sub_topic_map)
    {
        if (sub_topic_map->packetid == packetid)
        {
            break;
        }
        sub_topic_map = sub_topic_map->next;
    }
    return sub_topic_map;
}

int mqttsn_send_packet(uint8_t *buf, int buflen)
{
    int rc = 0;
    if ((rc = sendto(g_mqttsn_context.sock, buf, buflen, 0,
                     (struct sockaddr *)&(g_mqttsn_context.addr), g_mqttsn_context.addrLen)) == SOCKET_ERROR)
    {
        sys_arch_printf("mqttsn_send_packet() - packet send fail");
    }
    else
    {
        rc = 0;
    }
    last_transmit = time(NULL);
    return rc;
}

int mqttsn_transport_getdata(unsigned char *buf, int count)
{
    int rc = recvfrom(g_mqttsn_context.sock, buf, count, 0, NULL, NULL);
    return rc;
}

const char *mqttsn_return_code_string(uint8_t return_code)
{
    switch (return_code)
    {
    case MQTTSN_RC_ACCEPTED:
        return "Accepted";
    case MQTTSN_RC_REJECTED_CONGESTED:
        return "Rejected: congestion";
    case MQTTSN_RC_REJECTED_INVALID_TOPIC_ID:
        return "Rejected: invalid topic ID";
    case MQTTSN_RC_NOT_SUPPORTED:
        return "Rejected: not supported";
    default:
        return "Rejected: unknown reason";
    }
}

void mqttsn_receive_advertise(uint8_t *buf, int buflen)
{
    uint8_t gatewayid;
    uint16_t duration;
    if (MQTTSNDeserialize_advertise(&gatewayid, &duration, buf, buflen) != 1)
    {
        sys_arch_printf("advertise fail, gatewayid: %d, duration: %d\n", gatewayid, duration);
    }
    else
    {
        // TODO whether need to report advertise info?
        sys_arch_printf("Deserialize advertise success");
    }
}

void mqttsn_receive_gwinfo(uint8_t *buf, int buflen)
{
    uint8_t gatewayid;
    uint16_t gatewayaddress_len;
    uint8_t *gatewayaddress = NULL;

    if (MQTTSNDeserialize_gwinfo(&gatewayid, &gatewayaddress_len, &gatewayaddress, buf, buflen) != 1)
    {
        sys_arch_printf("get gateway info fail, gatewayid: %d, gatewayaddress_len: %d\n", gatewayid, gatewayaddress_len);
        mqttsn_report_result(0);
    }
    else
    {
        mqttsn_report_result(1);
    }
}

void mqttsn_receive_connack(uint8_t *buf, int buflen)
{
    int connack_rc = -1;
    if (MQTTSNDeserialize_connack(&connack_rc, buf, buflen) != 1 || connack_rc != MQTTSN_RC_ACCEPTED)
    {
        sys_arch_printf("Unable to connect, error season: %s\n", mqttsn_return_code_string(connack_rc));
        mqttsn_report_result(0);
    }
    else
    {
        mqttsn_report_result(1);
    }
}

int mqttsn_receive_willtopicreq(uint8_t *buf, int buflen)
{
    int len;
    len = MQTTSNSerialize_willtopic(buf, buflen, g_mqttsn_will.willqos, g_mqttsn_will.willretain, g_mqttsn_will.willtopic);
    if (mqttsn_send_packet(buf, len) != 0)
    {
        sys_arch_printf("set willtopic fail");
        mqttsn_report_result(0);
        return -1;
    }
    else
    {
        return 0;
    }
}

void mqttsn_receive_willmsgreq(uint8_t *buf, int buflen)
{
    int len;
    len = MQTTSNSerialize_willmsg(buf, buflen, g_mqttsn_will.willmsg);
    if (mqttsn_send_packet(buf, len) != 0)
    {
        sys_arch_printf("mqttsn set willtopic fail");
        mqttsn_report_result(0);
    }
}

void mqttsn_receive_regack(uint8_t *buf, int buflen)
{
    uint16_t topicid;
    uint16_t packetid;
    uint8_t returncode;

    int rc = MQTTSNDeserialize_regack(&topicid, &packetid, &returncode, buf, buflen);
    if (rc != 1 || returncode != MQTTSN_RC_ACCEPTED)
    {
        sys_arch_printf("mqttsn register fail, error season: %s\n", mqttsn_return_code_string(returncode));
        mqttsn_report_result(0);
    }
    else
    {
        mqttsn_pub_topic_map_t *put_topic_map = get_mqttsn_pub_topic_map_with_packetid(packetid);
        if (put_topic_map)
        {
            int len;
            MQTTSN_topicid topicfilter;
            topicfilter.data.id = topicid;
            topicfilter.type = MQTTSN_TOPIC_TYPE_NORMAL;
            len = MQTTSNSerialize_publish(buf, buflen, put_topic_map->dup, put_topic_map->qos, put_topic_map->retained,
                                          g_packet_id++, topicfilter, put_topic_map->payload, put_topic_map->payloadlen);
            rc = mqttsn_send_packet(buf, len);
            if (rc != 0)
            {
                sys_arch_printf("mqttsn mqttsn_receive_regack send publish fail");
                mqttsn_report_result(0);
                return;
            }

            if (put_topic_map->qos == 0)
            {
                mqttsn_report_result(1);
            }
        }
        else
        {
            sys_arch_printf("mqttsn mqttsn_receive_regack don't publish");
            mqttsn_report_result(0);
        }
    }
}

void mqttsn_receive_puback(uint8_t *buf, int buflen)
{
    uint16_t packetid;
    uint16_t topicid;
    uint8_t returncode;

    if (MQTTSNDeserialize_puback(&topicid, &packetid, &returncode, buf, buflen) != 1 || returncode != MQTTSN_RC_ACCEPTED)
    {
        sys_arch_printf("mqttsn publish fail, error season: %s\n", mqttsn_return_code_string(returncode));
        mqttsn_report_result(0);
    }
    else
    {
        sys_arch_printf("mqttsn puback received, msgid %d topic id %d\n", packetid, topicid);
        mqttsn_report_result(1);
    }
}

void mqttsn_receive_pubrec(uint8_t *buf, int buflen)
{
    int rc;
    int len;
    uint8_t acktype;
    uint16_t packetid;

    rc = MQTTSNDeserialize_ack(&acktype, &packetid, buf, buflen);
    if (rc != 1)
    {
        sys_arch_printf("mqttsn mqttsn_receive_pubrec parse pubrec error");
        mqttsn_report_result(0);
    }
    else
    {
        len = MQTTSNSerialize_pubrel(buf, buflen, packetid);
        if (mqttsn_send_packet(buf, len) != 0)
        {
            sys_arch_printf("mqttsn mqttsn_receive_pubrec send pubrel error");
            mqttsn_report_result(0);
        }
    }
}

void mqttsn_receive_pubcomp(uint8_t *buf, int buflen)
{
    int rc;
    uint8_t acktype;
    uint16_t packetid;

    rc = MQTTSNDeserialize_ack(&acktype, &packetid, buf, buflen);
    if (rc != 1)
    {
        sys_arch_printf("mqttsn publish MQTTSN_PUBCOMP ack error, packetid: %d\n", packetid);
        mqttsn_report_result(0);
    }
    else
    {
        mqttsn_report_result(1);
    }
}

void mqttsn_receive_suback(uint8_t *buf, int buflen)
{
    sys_arch_printf("mqttsn_receive_suback()");

    uint16_t packetid;
    int qos;
    uint8_t returncode;
    uint16_t topicid;
    //char *name;

    int rc;
    rc = MQTTSNDeserialize_suback(&qos, &topicid, &packetid, &returncode, buf, buflen);
    if (rc != 1 && returncode != MQTTSN_RC_ACCEPTED)
    {
        sys_arch_printf("mqttsn subscribe fail, error season: %s\n", mqttsn_return_code_string(returncode));
        mqttsn_report_result(0);
    }
    else
    {
        mqttsn_report_result(1);
        mqttsn_sub_topic_map_t *sub_topic_map = mqttsn_get_sub_topic_with_packetid(packetid);
        if (topicid && sub_topic_map)
        {
            if (sub_topic_map->topictype == MQTTSN_TOPIC_TYPE_PREDEFINED)
            {
                sys_arch_printf("mqttsn suback predefined topic id");
            }
            else if (sub_topic_map->topictype == MQTTSN_TOPIC_TYPE_NORMAL || sub_topic_map->topictype == MQTTSN_TOPIC_TYPE_SHORT)
            {
                //name = sub_topic_map->topicname;
            }
            sub_topic_map->topicid = topicid;
        }
    }
}

void mqttsn_receive_unsuback(uint8_t *buf, int buflen)
{
    uint16_t packetid;

    int rc = MQTTSNDeserialize_unsuback(&packetid, buf, buflen);
    if (rc != 1)
    {
        sys_arch_printf("mqttsn unsub fail");
        mqttsn_report_result(0);
    }
    else
    {
        mqttsn_report_result(1);
    }
}

void mqttsn_receive_willtopicresp(uint8_t *buf, int buflen)
{
    int returncode;

    if (MQTTSNDeserialize_willtopicresp(&returncode, buf, buflen) != 1 && returncode != MQTTSN_RC_ACCEPTED)
    {
        sys_arch_printf("mqttsn topic update fial, error season: %s\n", mqttsn_return_code_string(returncode));
        mqttsn_report_result(0);
    }
    else
    {
        mqttsn_report_result(1);
    }
}

void mqttsn_receive_willmsgresp(uint8_t *buf, int buflen)
{
    int returncode;
    if (MQTTSNDeserialize_willmsgresp(&returncode, buf, buflen) != 1 && returncode != MQTTSN_RC_ACCEPTED)
    {
        sys_arch_printf("mqttsn topic update fial, error season: %s\n", mqttsn_return_code_string(returncode));
        mqttsn_report_result(0);
    }
    else
    {
        mqttsn_report_result(1);
    }
}

void mqttsn_receive_publish(uint8_t *buf, int buflen)
{
    uint8_t dup;
    int qos;
    uint8_t retained;
    uint16_t packetid;
    MQTTSN_topicid topic;
    uint8_t *payload;
    int payloadlen;

    if (MQTTSNDeserialize_publish(&dup, &qos, &retained, &packetid, &topic, &payload, &payloadlen, buf, buflen) != 1)
    {
        sys_arch_printf("mqttsn mqttsn_receive_publish receive publish error");
    }
    else
    {
        // TODO need to report data?
        sys_arch_printf("mqttsn mqttsn_receive_publish receive publish success, payload: %s", payload);
    }
}

void mqttsn_receive_disconnect(uint8_t *buf, int buflen)
{
    g_keep_running = false;

    int duration;
    if (MQTTSNDeserialize_disconnect(&duration, buf, buflen) != 1)
    {
        sys_arch_printf("mqttsn mqttsn_receive_disconnect fail");
    }
}

void mqttsn_set_connectinfo(char *host, char *port, uint8_t ipv4, char *clientID,
                            uint16_t duration, uint8_t cleansession, uint8_t willflag, uint8_t willqos, uint8_t willretain, char *willtopic, char *willmsg)
{
    g_keep_running = true;

    memset(&g_mqttsn_context, 0, sizeof(mqttsn_context_t));

    g_mqttsn_context.host = (char *)malloc(strlen(host) + 1);
    strcpy(g_mqttsn_context.host, host);
    g_mqttsn_context.port = (char *)malloc(strlen(port) + 1);
    strcpy(g_mqttsn_context.port, port);

    if (ipv4)
    {
        g_mqttsn_context.addressFamily = AF_INET;
    }
    else
    {
        g_mqttsn_context.addressFamily = AF_INET6;
    }

    MQTTSNPacket_connectData connectData = MQTTSNPacket_connectData_initializer;
    connectData.clientID.cstring = clientID;
    connectData.duration = duration;
    connectData.cleansession = cleansession;
    connectData.willFlag = willflag;

    g_connecData = connectData;
    mqttsn_set_timeout(duration / 2);

    if (willflag)
    {
        memset(&g_mqttsn_will, 0, sizeof(mqttsn_will_t));
        g_mqttsn_will.willqos = willqos;
        g_mqttsn_will.willretain = willretain;
        g_mqttsn_will.willtopic.cstring = willtopic;
        g_mqttsn_will.willmsg.cstring = willmsg;
    }
}

int mqttsn_create_socket(char *portStr, int addressFamily)
{
    int s = -1;

    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (0 != getaddrinfo(NULL, portStr, &hints, &res))
    {
        return -1;
    }

    for (p = res; p != NULL && s == -1; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            if (-1 == bind(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }

    freeaddrinfo(res);

    return s;
}

int mqttsn_connect_server()
{
    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;
    struct addrinfo *p;
    int s;
    struct sockaddr *sa = NULL;
    socklen_t sl = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = g_mqttsn_context.addressFamily;
    hints.ai_socktype = SOCK_DGRAM;

    if (0 != getaddrinfo(g_mqttsn_context.host, g_mqttsn_context.port, &hints, &servinfo) || servinfo == NULL)
        return -1;

    int result = 0;
    // we test the various addresses
    s = -1;
    for (p = servinfo; p != NULL && s == -1; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            sa = p->ai_addr;
            sl = p->ai_addrlen;
            if (-1 == connect(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }

    if (s >= 0)
    {
        memcpy(&(g_mqttsn_context.addr), sa, sl);
        g_mqttsn_context.addrLen = sl;
        close(s);
        result = 1;
    }

    if (NULL != servinfo)
    {
        free(servinfo);
    }

    return result;
}

int mqttsn_receive_packet()
{
    uint8_t buf[1024];
    int buflen = sizeof(buf);

    int type = MQTTSNPacket_read(buf, buflen, mqttsn_transport_getdata);
    sys_arch_printf("mqttsn mqttsn_receive_packet type = %d", type);
    switch (type)
    {
    case MQTTSN_ADVERTISE:
        mqttsn_receive_advertise(buf, buflen);
        break;
    case MQTTSN_GWINFO:
        mqttsn_receive_gwinfo(buf, buflen);
        break;
    case MQTTSN_REGACK:
        mqttsn_receive_regack(buf, buflen);
        break;
    case MQTTSN_PUBACK:
        mqttsn_receive_puback(buf, buflen);
        break;
    case MQTTSN_PUBREC:
        mqttsn_receive_pubrec(buf, buflen);
        break;
    case MQTTSN_PUBCOMP:
        mqttsn_receive_pubcomp(buf, buflen);
        break;
    case MQTTSN_SUBACK:
        mqttsn_receive_suback(buf, buflen);
        break;
    case MQTTSN_UNSUBACK:
        mqttsn_receive_unsuback(buf, buflen);
        break;
    case MQTTSN_WILLTOPICRESP:
        mqttsn_receive_willtopicresp(buf, buflen);
        break;
    case MQTTSN_WILLMSGRESP:
        mqttsn_receive_willmsgresp(buf, buflen);
        break;
    case MQTTSN_PUBLISH:
        mqttsn_receive_publish(buf, buflen);
        break;
    case MQTTSN_DISCONNECT:
        mqttsn_receive_disconnect(buf, buflen);
        return 2;
    default:
        return 0;
    }

    return 1;
}

void mqttsn_free_context()
{
    shutdown(g_mqttsn_context.sock, SHUT_WR);
    close(g_mqttsn_context.sock);
    free(g_mqttsn_context.host);
    free(g_mqttsn_context.port);
    //free(&g_mqttsn_context);
}

int mqttsn_wait()
{
    MQTTSNString clientid = MQTTSNString_initializer;
    while (g_keep_running)
    {
        time_t now = time(NULL);
        fd_set fdset;
        struct timeval tv;

        FD_ZERO(&fdset);
        FD_SET(g_mqttsn_context.sock, &fdset);

        tv.tv_sec = timeout;
        tv.tv_usec = 0;

        if (g_connecData.duration > 0 && (now - last_transmit) >= g_connecData.duration)
        {
            int rc;
            int len;
            uint8_t buf[500];
            int buflen = (sizeof(buf));
            len = MQTTSNSerialize_pingreq(buf, buflen, clientid);
            rc = mqttsn_send_packet(buf, len);
            if (rc != 0)
            {
                mqttsn_report_result(0);
                mqttsn_free_pub_topic();
                mqttsn_free_sub_topic();
                mqttsn_free_context();
                return 0;
            }
        }

        int result = select(g_mqttsn_context.sock + 1, &fdset, NULL, NULL, &tv);
        if (result < 0)
        {
            sys_arch_printf("Error in select()");
        }
        else if (result > 0)
        {
            if (FD_ISSET(g_mqttsn_context.sock, &fdset))
            {
                int receive = mqttsn_receive_packet();
                if (receive == 0)
                {
                    mqttsn_report_result(0);
                    mqttsn_free_pub_topic();
                    mqttsn_free_sub_topic();
                    mqttsn_free_context();
                    return 0;
                }
                else if (receive == 2) // disconnect server success
                {
                    mqttsn_report_result(1);
                    break;
                }
            }
        }
    }

    mqttsn_free_pub_topic();
    mqttsn_free_sub_topic();
    mqttsn_free_context();
    return 1;
}

int mqttsn_wait_for_connack()
{
    uint8_t buf[500];
    int buflen = sizeof(buf);
    int type = MQTTSNPacket_read(buf, buflen, mqttsn_transport_getdata);
    if (type == MQTTSN_WILLTOPICREQ)
    {
        mqttsn_receive_willtopicreq(buf, buflen);

        type = MQTTSNPacket_read(buf, buflen, mqttsn_transport_getdata);
        if (type == MQTTSN_WILLMSGREQ)
        {
            mqttsn_receive_willmsgreq(buf, buflen);
        }

        type = MQTTSNPacket_read(buf, buflen, mqttsn_transport_getdata);
    }

    if (type == MQTTSN_CONNACK)
    {
        mqttsn_receive_connack(buf, buflen);
        return 0;
    }
    else
    {
        return -1;
    }
}

int mqttsn_send_connect()
{
    int connresult;
    g_mqttsn_context.sock = mqttsn_create_socket(g_mqttsn_context.port, g_mqttsn_context.addressFamily);
    if (g_mqttsn_context.sock < 0)
    {
        goto ERROR;
    }

    connresult = mqttsn_connect_server();
    if (connresult != 1)
    {
        goto ERROR;
    }

    int rc;
    int len;
    uint8_t buf[500];
    int buflen = sizeof(buf);

    len = MQTTSNSerialize_connect(buf, buflen, &g_connecData);
    rc = mqttsn_send_packet(buf, len);
    if (rc != 0)
    {
        goto ERROR;
    }

    if (mqttsn_wait_for_connack() != 0)
    {
        goto ERROR;
    }

    int result = mqttsn_wait();
    return result;
ERROR:
    mqttsn_report_result(0);
    mqttsn_free_pub_topic();
    mqttsn_free_sub_topic();
    mqttsn_free_context();
    return 0;
}

int mqttsn_send_searchgw(uint8_t radius)
{
    int rc;
    int len;
    uint8_t buf[500];
    int buflen = sizeof(buf);

    len = MQTTSNSerialize_searchgw(buf, buflen, radius);
    rc = mqttsn_send_packet(buf, len);
    return rc;
}

int mqttsn_send_subscribe_topic_name(char *topicname, uint8_t dup, uint8_t qos)
{
    int rc;
    int len;
    uint8_t buf[500];
    int buflen = sizeof(buf);

    MQTTSN_topicid topicFilter;
    uint8_t topictype;
    if (strlen(topicname) == 2)
    {
        topictype = 1;
        topicFilter.type = MQTTSN_TOPIC_TYPE_SHORT;
        memcpy(topicFilter.data.short_name, topicname, 2);
    }
    else
    {
        topictype = 2;
        topicFilter.type = MQTTSN_TOPIC_TYPE_NORMAL;
        topicFilter.data.long_.name = (char *)topicname;
        topicFilter.data.long_.len = strlen(topicFilter.data.long_.name);
    }
    mqttsn_add_sub_topic(g_packet_id, 0, topicname, topictype, 0);

    len = MQTTSNSerialize_subscribe(buf, buflen, dup, qos, g_packet_id++, &topicFilter);
    rc = mqttsn_send_packet(buf, len);
    return rc;
}

int mqttsn_send_subscribe_topic_id(uint16_t topicid, uint8_t dup, uint8_t qos)
{
    int rc;
    int len;
    uint8_t buf[500];
    int buflen = sizeof(buf);

    MQTTSN_topicid topicFilter;
    topicFilter.type = MQTTSN_TOPIC_TYPE_PREDEFINED;
    topicFilter.data.id = topicid;

    mqttsn_add_sub_topic(g_packet_id, topicid, NULL, MQTTSN_TOPIC_TYPE_PREDEFINED, MQTTSN_SUBSCRIBE);

    len = MQTTSNSerialize_subscribe(buf, buflen, dup, qos, g_packet_id++, &topicFilter);
    sys_arch_printf("mqttsn mqttsn_send_subscribe_topic_id len: %d", len);
    rc = mqttsn_send_packet(buf, len);
    return rc;
}

int mqttsn_send_unsubscribe_topic_name(char *topicname)
{
    int rc;
    int len;
    uint8_t buf[500];
    int buflen = sizeof(buf);

    MQTTSN_topicid topicFilter;
    if (strlen(topicname) == 2)
    {
        topicFilter.type = MQTTSN_TOPIC_TYPE_SHORT;
        memcpy(topicFilter.data.short_name, topicname, 2);
    }
    else
    {
        topicFilter.type = MQTTSN_TOPIC_TYPE_NORMAL;
        topicFilter.data.long_.name = topicname;
        topicFilter.data.long_.len = strlen(topicFilter.data.long_.name);
    }
    mqttsn_remove_sub_topic(topicname);

    len = MQTTSNSerialize_unsubscribe(buf, buflen, g_packet_id++, &topicFilter);
    rc = mqttsn_send_packet(buf, len);
    return rc;
}

int mqttsn_send_unsubscribe_topic_id(uint16_t topicid)
{
    int rc;
    int len;
    uint8_t buf[500];
    int buflen = sizeof(buf);

    MQTTSN_topicid topicFilter;
    topicFilter.type = MQTTSN_TOPIC_TYPE_PREDEFINED;
    topicFilter.data.id = topicid;

    len = MQTTSNSerialize_unsubscribe(buf, buflen, g_packet_id++, &topicFilter);
    rc = mqttsn_send_packet(buf, len);
    return rc;
}

int mqttsn_send_publish(uint8_t dup, uint8_t qos, uint8_t retained, uint8_t *payload, char *topicname, uint16_t topicid)
{
    int rc = 0;
    int len = 0;
    uint8_t buf[1014];
    int buflen = sizeof(buf);

    MQTTSN_topicid topicFilter;
    if (topicid)
    {
        topicFilter.data.id = topicid;
        topicFilter.type = MQTTSN_TOPIC_TYPE_PREDEFINED;
    }
    else if (strlen(topicname) == 2)
    {
        topicid = (topicname[0] << 8) + topicname[1];
        topicFilter.type = MQTTSN_TOPIC_TYPE_SHORT;
    }
    else if (qos >= 0)
    {
        mqttsn_add_pub_topic(g_packet_id, topicid, topicname, dup, qos, retained, payload, strlen((const char *)payload));

        MQTTSNString topicstr;
        topicstr.cstring = topicname;
        len = MQTTSNSerialize_register(buf, buflen, topicid, g_packet_id++, &topicstr);
        rc = mqttsn_send_packet(buf, len);
        return rc;
    }

    len = MQTTSNSerialize_publish(buf, buflen, dup, qos, retained, g_packet_id++, topicFilter, payload, strlen((const char *)payload));
    rc = mqttsn_send_packet(buf, len);
    return rc;
}

int mqttsn_send_willtopicupd(char *willtopic)
{
    int rc;
    int len;
    uint8_t buf[500];
    int buflen = sizeof(buf);

    g_mqttsn_will.willtopic.cstring = willtopic;

    len = MQTTSNSerialize_willtopicupd(buf, buflen, g_mqttsn_will.willqos, g_mqttsn_will.willretain, g_mqttsn_will.willtopic);
    rc = mqttsn_send_packet(buf, len);
    return rc;
}

int mqttsn_send_willmsgupd(char *willmsg)
{
    int rc;
    int len;
    uint8_t buf[500];
    int buflen = sizeof(buf);

    g_mqttsn_will.willmsg.cstring = willmsg;

    len = MQTTSNSerialize_willmsgupd(buf, buflen, g_mqttsn_will.willmsg);
    rc = mqttsn_send_packet(buf, len);
    return rc;
}

int mqttsn_send_disconnect(uint16_t duration)
{
    int rc;
    int len;
    uint8_t buf[500];
    int buflen = sizeof(buf);

    len = MQTTSNSerialize_disconnect(buf, buflen, duration);
    rc = mqttsn_send_packet(buf, len);

    return rc;
}

#endif
