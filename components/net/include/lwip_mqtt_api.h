#ifndef _LWIP_MQTT_API_H
#define _LWIP_MQTT_API_H
#include "sockets.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"

#define MQTT_STRING_SIZE MQTT_OUTPUT_RINGBUF_SIZE

err_t lwip_mqtt_connect(mqtt_client_t *client, const ip_addr_t *ip_addr, u16_t port, mqtt_connection_cb_t cb, void *arg,
                        const struct mqtt_connect_client_info_t *client_info);
err_t lwip_mqtt_publish(mqtt_client_t *client, const char *topic, const void *payload, u16_t payload_length, u8_t dup, u8_t qos, u8_t retain,
                        mqtt_request_cb_t cb, void *arg);

err_t lwip_mqtt_sub_unsub(mqtt_client_t *client, const char *topic, u8_t qos, mqtt_request_cb_t cb, void *arg, u8_t sub);

void lwip_mqtt_disconnect(mqtt_client_t *client);

void lwip_mqtt_set_inpub_callback(mqtt_client_t *client, mqtt_incoming_publish_cb_t pub_cb,
                                  mqtt_incoming_data_cb_t data_cb, void *arg);

u8_t lwip_mqtt_client_is_connected(mqtt_client_t *client);

#define lwip_mqtt_client_new() mqtt_client_new()
#define lwip_mqtt_subscribe(client, topic, qos, cb, arg) lwip_mqtt_sub_unsub(client, topic, qos, cb, arg, 1)
#define lwip_mqtt_unsubscribe(client, topic, cb, arg) lwip_mqtt_sub_unsub(client, topic, 0, cb, arg, 0)
#endif
