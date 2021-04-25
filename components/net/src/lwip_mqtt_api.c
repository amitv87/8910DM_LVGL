#include "lwip_mqtt_api.h"
#include "lwip/tcpip.h"

struct mqtt_api_msg
{
    mqtt_client_t *client;
    ip_addr_t *ip_addr;
    struct mqtt_connect_client_info_t *client_info;
    const char *topic;
    u8_t qos;
    u8_t sub;
    u8_t dup;
    u8_t retain;
    u16_t port;
    u16_t payload_length;
    const void *payload;
    mqtt_connection_cb_t connect_cb;
    mqtt_request_cb_t request_cb;
    void *arg;
    sys_sem_t *sem;
    err_t *err;
};

struct mqtt_set_inpub_api_msg
{
    mqtt_client_t *client;
    mqtt_incoming_publish_cb_t pub_cb;
    mqtt_incoming_data_cb_t data_cb;
    void *arg;
    sys_sem_t *sem;
};

static void
lwip_do_mqtt_connect(void *arg)
{
    struct mqtt_api_msg *msg = (struct mqtt_api_msg *)arg;

    *(msg->err) = mqtt_client_connect(msg->client, msg->ip_addr, msg->port, msg->connect_cb, msg->arg, msg->client_info);
    sys_sem_signal(msg->sem);
}

static void
lwip_do_mqtt_publish(void *arg)
{
    struct mqtt_api_msg *msg = (struct mqtt_api_msg *)arg;

    *(msg->err) = mqtt_publish(msg->client, msg->topic, msg->payload, msg->payload_length, msg->dup, msg->qos,
                               msg->retain, msg->request_cb, msg->arg);
    sys_sem_signal(msg->sem);
}

static void
lwip_do_mqtt_sub_unsub(void *arg)
{
    struct mqtt_api_msg *msg = (struct mqtt_api_msg *)arg;

    *(msg->err) = mqtt_sub_unsub(msg->client, msg->topic, msg->qos, msg->request_cb, msg->arg, msg->sub);
    sys_sem_signal(msg->sem);
}

static void
lwip_do_mqtt_disconnect(void *arg)
{
    struct mqtt_api_msg *msg = (struct mqtt_api_msg *)arg;

    mqtt_disconnect(msg->client);
    sys_sem_signal(msg->sem);
}

static void
lwip_do_mqtt_set_inpub_callback(void *arg)
{
    struct mqtt_set_inpub_api_msg *msg = (struct mqtt_set_inpub_api_msg *)arg;

    mqtt_set_inpub_callback(msg->client, msg->pub_cb, msg->data_cb, msg->arg);
    sys_sem_signal(msg->sem);
}

static void
lwip_do_mqtt_client_is_connected(void *arg)
{
    struct mqtt_api_msg *msg = (struct mqtt_api_msg *)arg;

    *(msg->err) = mqtt_client_is_connected(msg->client);
    sys_sem_signal(msg->sem);
}

err_t lwip_mqtt_connect(mqtt_client_t *client, const ip_addr_t *ip_addr, u16_t port, mqtt_connection_cb_t cb, void *arg,
                        const struct mqtt_connect_client_info_t *client_info)
{
    struct mqtt_api_msg msg = {0};
    err_t err;
    err_t cberr;
    sys_sem_t sem;

    msg.err = &err;
    msg.sem = &sem;
    msg.client = client;
    msg.ip_addr = (ip_addr_t *)ip_addr;
    msg.port = port;
    msg.connect_cb = cb;
    msg.arg = arg;
    msg.client_info = (struct mqtt_connect_client_info_t *)client_info;
    err = sys_sem_new(msg.sem, 0);
    if (err != ERR_OK)
    {
        return err;
    }
    cberr = tcpip_callback(lwip_do_mqtt_connect, &msg);
    if (cberr != ERR_OK)
    {
        sys_sem_free(msg.sem);
        return cberr;
    }
    sys_sem_wait(msg.sem);
    sys_sem_free(msg.sem);
    return err;
}

err_t lwip_mqtt_publish(mqtt_client_t *client, const char *topic, const void *payload, u16_t payload_length, u8_t dup, u8_t qos, u8_t retain,
                        mqtt_request_cb_t cb, void *arg)
{
    struct mqtt_api_msg msg = {0};
    err_t err;
    err_t cberr;
    sys_sem_t sem;

    msg.err = &err;
    msg.sem = &sem;
    msg.client = client;
    msg.topic = topic;
    msg.payload = payload;
    msg.payload_length = payload_length;
    msg.dup = dup;
    msg.qos = qos;
    msg.retain = retain;
    msg.request_cb = cb;
    msg.arg = arg;
    err = sys_sem_new(msg.sem, 0);
    if (err != ERR_OK)
    {
        return err;
    }
    cberr = tcpip_callback(lwip_do_mqtt_publish, &msg);
    if (cberr != ERR_OK)
    {
        sys_sem_free(msg.sem);
        return cberr;
    }
    sys_sem_wait(msg.sem);
    sys_sem_free(msg.sem);
    return err;
}

err_t lwip_mqtt_sub_unsub(mqtt_client_t *client, const char *topic, u8_t qos, mqtt_request_cb_t cb, void *arg, u8_t sub)
{
    struct mqtt_api_msg msg = {0};
    err_t err;
    err_t cberr;
    sys_sem_t sem;

    msg.err = &err;
    msg.sem = &sem;
    msg.client = client;
    msg.topic = topic;
    msg.qos = qos;
    msg.sub = sub;
    msg.request_cb = cb;
    msg.arg = arg;
    err = sys_sem_new(msg.sem, 0);
    if (err != ERR_OK)
    {
        return err;
    }
    cberr = tcpip_callback(lwip_do_mqtt_sub_unsub, &msg);
    if (cberr != ERR_OK)
    {
        sys_sem_free(msg.sem);
        return cberr;
    }
    sys_sem_wait(msg.sem);
    sys_sem_free(msg.sem);
    return err;
}

void lwip_mqtt_disconnect(mqtt_client_t *client)
{
    struct mqtt_api_msg msg = {0};
    err_t err;
    err_t cberr;
    sys_sem_t sem;

    msg.err = &err;
    msg.sem = &sem;
    msg.client = client;
    err = sys_sem_new(msg.sem, 0);
    if (err != ERR_OK)
    {
        return;
    }
    cberr = tcpip_callback(lwip_do_mqtt_disconnect, &msg);
    if (cberr != ERR_OK)
    {
        sys_sem_free(msg.sem);
        return;
    }
    sys_sem_wait(msg.sem);
    sys_sem_free(msg.sem);
    return;
}

void lwip_mqtt_set_inpub_callback(mqtt_client_t *client, mqtt_incoming_publish_cb_t pub_cb,
                                  mqtt_incoming_data_cb_t data_cb, void *arg)
{
    struct mqtt_set_inpub_api_msg msg = {0};
    err_t err;
    err_t cberr;
    sys_sem_t sem;

    msg.sem = &sem;
    msg.client = client;
    msg.pub_cb = pub_cb;
    msg.data_cb = data_cb;
    msg.arg = arg;
    err = sys_sem_new(msg.sem, 0);
    if (err != ERR_OK)
    {
        return;
    }
    cberr = tcpip_callback(lwip_do_mqtt_set_inpub_callback, &msg);
    if (cberr != ERR_OK)
    {
        sys_sem_free(msg.sem);
        return;
    }
    sys_sem_wait(msg.sem);
    sys_sem_free(msg.sem);
    return;
}

u8_t lwip_mqtt_client_is_connected(mqtt_client_t *client)
{
    struct mqtt_api_msg msg = {0};
    err_t err;
    err_t cberr;
    sys_sem_t sem;

    msg.err = &err;
    msg.sem = &sem;
    msg.client = client;
    err = sys_sem_new(msg.sem, 0);
    if (err != ERR_OK)
    {
        return 0;
    }
    cberr = tcpip_callback(lwip_do_mqtt_client_is_connected, &msg);
    if (cberr != ERR_OK)
    {
        sys_sem_free(msg.sem);
        return 0;
    }
    sys_sem_wait(msg.sem);
    sys_sem_free(msg.sem);
    return err;
}
