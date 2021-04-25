/*============================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------

=============================================================================*/

#ifndef QL_MQTTLCLIENT_H
#define QL_MQTTLCLIENT_H

typedef int mqtt_client_t;

#define MQTT_SSL_VERIFY_NONE            0
#define MQTT_SSL_VERIFY_SERVER          1
#define MQTT_SSL_VERIFY_SERVER_CLIENT   2


struct mqtt_ssl_config_t{
	int    ssl_ctx_id;
	int    verify_level;
	char  *cacert_path;
	char  *client_cert_path;
	char  *client_key_path;
	char  *client_key_pwd;
};
struct mqtt_connect_client_info_t {
  const char *client_id;
  /** User name, set to NULL if not used */
  const char* client_user;
  /** Password, set to NULL if not used */
  const char* client_pass;
  /** keep alive time in seconds, 0 to disable keep alive functionality*/
  unsigned short keep_alive;
  /** will topic, set to NULL if will is not to be used,
      will_msg, will_qos and will retain are then ignored */
  const char* will_topic;
  /** will_msg, see will_topic */
  const char* will_msg;
  /** will_qos, see will_topic */
  unsigned char will_qos;
  /** will_retain, see will_topic */
  unsigned char will_retain;
  unsigned char clean_session; 
  struct mqtt_ssl_config_t  *ssl_cfg;
};

typedef enum
{
  /** Accepted */
  MQTT_CONNECT_ACCEPTED                 = 0,
  /** Refused protocol version */
  MQTT_CONNECT_REFUSED_PROTOCOL_VERSION = 1,
  /** Refused identifier */
  MQTT_CONNECT_REFUSED_IDENTIFIER       = 2,
  /** Refused server */
  MQTT_CONNECT_REFUSED_SERVER           = 3,
  /** Refused user credentials */
  MQTT_CONNECT_REFUSED_USERNAME_PASS    = 4,
  /** Refused not authorized */
  MQTT_CONNECT_REFUSED_NOT_AUTHORIZED_  = 5,

  MQTT_CONNECT_TCP_CONNECTED_FAILURE   = 254,

  MQTT_CONNECT_OUT_OF_MEMORY           = 255,
  /** Disconnected */
  MQTT_CONNECT_DISCONNECTED             = 256,
  /** Timeout */
  MQTT_CONNECT_TIMEOUT                  = 257
} mqtt_connection_status_e;

typedef enum{
	MQTTCLIENT_SUCCESS                  = 0,
	MQTTCLIENT_INVALID_PARAM            = -1,
	MQTTCLIENT_WOUNDBLOCK               = -2,
	MQTTCLIENT_OUT_OF_MEM               = -3,
	MQTTCLIENT_ALLOC_FAIL               = -4,
	MQTTCLIENT_TCP_CONNECT_FAIL         = -5,
	MQTTCLIENT_NOT_CONNECT              = -6,
	MQTTCLIENT_SEND_PKT_FAIL            = -7,
	MQTTCLIENT_BAD_REQUEST              = -8,
	MQTTCLIENT_TIMEOUT                  = -9,
}mqtt_error_code_e;

typedef void (*mqtt_connection_cb_t)(mqtt_client_t *client, void *arg, mqtt_connection_status_e status);

typedef void (*mqtt_request_cb_t)(mqtt_client_t *client, void *arg,int err);

typedef void (*mqtt_incoming_publish_cb_t)(mqtt_client_t *client, void *arg, int pkt_id, const char *topic, const unsigned char *payload, unsigned short payload_len);

typedef void (*mqtt_disconnect_cb_t)(mqtt_client_t *client, void *arg,int err);

typedef void(*mqtt_state_exception_cb_t)(mqtt_client_t *client);

int ql_mqtt_client_init(mqtt_client_t *client, int cid);

/*
host:
mqtt:  mqtt://220.180.239.212:8306
mqtts: mqtts://220.180.239.212:8307
*/

/*****************************************************************
* Function: ql_mqtt_connect
*
* Description: 完成mqtt client上下文的配置,确定mqtt 连接属性,建立mqtt connect连接
* 
* Parameters:
*	client        [in]    mqtt client上下文指针.
*   host          [in]    mqtt 通信对端IP地址或域名地址.
*   cb            [in]    关联此mqtt client上下文与此上下文的事件回调函数
*   arg           [in]    mqtt 连接控制,自定义传参
*   client_info   [in]    mqtt client上下文连接配置info信息
*   exp_cb        [in]    关联此mqtt client上下文与此上下文的异常事件处理回调函数
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int ql_mqtt_connect(mqtt_client_t *client, const char *host, mqtt_connection_cb_t cb, void *arg, const struct mqtt_connect_client_info_t *client_info, mqtt_state_exception_cb_t exp_cb);

/*****************************************************************
* Function: ql_mqtt_publish
*
* Description:mqtt发布消息函数,完成向mqtt服务器的消息推送
* 
* Parameters:
*	client          [in]    mqtt client上下文指针.
*   topic           [in]    所要发布消息的主题名称.
*   payload         [in]    有效载荷,包含将被发布的应用消息
*   payload_length  [in]    有效载荷的长度
*   qos             [in]    QoS质量等级
*   retain          [in]    服务器保留这次推送的信息,有新的订阅者，把这消息推送给它
*   cb              [in]    请求消息响应的回调处理函数
*   arg             [in]    mqtt 发布控制,自定义传参
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int ql_mqtt_publish(mqtt_client_t *client, const char *topic, const void *payload,  unsigned short payload_length, unsigned char qos, unsigned char retain, mqtt_request_cb_t cb, void *arg);

/*****************************************************************
* Function: ql_mqtt_sub_unsub
*
* Description: mqtt客户端 订阅/取消订阅所选定的topic
* 
* Parameters:
*	client        [in]    mqtt client上下文指针.
*   topic         [in]    订阅或取消订阅的主题名.
*   qos           [in]    QoS质量等级
*   cb            [in]    请求消息响应的回调处理函数
*   arg           [in]    mqtt 订阅/取消订阅主题,自定义传参
*   sub           [in]    sub为0取消订阅topic,sub非0订阅topic
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int ql_mqtt_sub_unsub(mqtt_client_t *client, const char *topic, unsigned char qos, mqtt_request_cb_t cb, void *arg,unsigned char sub);

/*****************************************************************
* Function: ql_mqtt_disconnect
*
* Description: mqtt 关闭连接函数,完成mqtt 连接关闭,更改mqtt client上下文相关状态信息,关联关闭事件回调处理函数
* 
* Parameters:
*	client        [in]    mqtt client上下文指针.
*   cb            [in]    mqtt client关闭连接事件回调处理函数.
*   arg           [in]    mqtt 关闭连接,自定义传参
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int ql_mqtt_disconnect(mqtt_client_t *client, mqtt_disconnect_cb_t cb, void *arg);

/*****************************************************************
* Function: ql_mqtt_set_inpub_callback
*
* Description: 关联mqtt client上下文与inpub_cb 主题,载荷信息显示回调函数
* 
* Parameters:
*	client        [in]    mqtt client上下文指针.
*   inpub_cb      [in]    订阅/取消订阅的topic,以及有效载荷,信息显示回调函数.
*   arg           [in]    自定义传参
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int ql_mqtt_set_inpub_callback(mqtt_client_t *client, mqtt_incoming_publish_cb_t inpub_cb, void *arg);

/*****************************************************************
* Function: ql_mqtt_client_is_connected
*
* Description: mqtt client上下文合法性检查,以及判断mqtt的连接状态是否成功成功连接对端
* 
* Parameters:
*	client        [in]    mqtt client上下文指针.
*
* Return:
* 	1           成功
*	0           失败
*
*****************************************************************/
int ql_mqtt_client_is_connected(mqtt_client_t *client);

/*****************************************************************
* Function: ql_mqtt_client_deinit
*
* Description: mqtt client反初始化函数,将此上下文所有信息全部置NULL
* 
* Parameters:
*	client        [in]    mqtt client上下文指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int ql_mqtt_client_deinit(mqtt_client_t *client);

#endif
