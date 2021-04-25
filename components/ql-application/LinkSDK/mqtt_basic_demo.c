/*
 * 这个例程适用于`RTOS`这类支持task的设备, 它演示了用SDK配置MQTT参数并建立连接, 之后创建2个线程
 *
 * + 一个任务用于保活长连接
 * + 一个任务用于接收消息, 并在有消息到达时进入默认的数据回调, 在连接状态变化时进入事件回调
 *
 * 需要用户关注或修改的部分, 已经用 TODO 在注释中标明
 *
 */
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "ali_demo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_api_nw.h"

#include "ql_log.h"
#include "ql_api_datacall.h"
#include "sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"

#include "lwip/netdb.h"
#include "lwip/netif.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"

#include "ql_ssl.h"

#define QL_SOCKET_LOG_LEVEL	                QL_LOG_LEVEL_INFO
#define QL_SOCKET_LOG(msg, ...)			    QL_LOG(QL_SOCKET_LOG_LEVEL, "ql_Sock", msg, ##__VA_ARGS__)
#define QL_SOCKET_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_SSL", msg, ##__VA_ARGS__)


#define mqtt_profile_idx 1

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;


/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

ql_task_t ql_alisdk_mqtt_client_task_ref;
ql_task_t ql_alisdk_mqtt_process_task_ref;
ql_task_t ql_alisdk_mqtt_recv_task_ref;
static uint8_t g_mqtt_process_task_running = 0;
static uint8_t g_mqtt_recv_task_running = 0;

/* TODO: 如果要关闭日志, 就把这个函数实现为空, 如果要减少日志, 可根据code选择不打印
 *
 * 例如: [1577589489.033][LK-0317] mqtt_basic_demo&a13FN5TplKq
 *
 * 上面这条日志的code就是0317(十六进制), code值的定义见core/aiot_state_api.h
 *
 */

static int datacall_satrt(void)
{
   	int ret = 0;
	int i = 0;
    ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};
	uint8_t nSim = 0;
	
	ql_rtos_task_sleep_s(10);
	QL_SOCKET_LOG("========== socket demo start ==========");
	QL_SOCKET_LOG("wait for network register done");
		
	while((ret = ql_network_register_wait(nSim, 120)) != 0 && i < 10){
    	i++;
		ql_rtos_task_sleep_s(1);
	}
	if(ret == 0){
		i = 0;
		QL_SOCKET_LOG("====network registered!!!!====");
	}else{
		QL_SOCKET_LOG("====network register failure!!!!!====");
		return -1;
	}

	ql_set_data_call_asyn_mode(nSim, mqtt_profile_idx, 0);

	QL_SOCKET_LOG("===start data call====");
	ret=ql_start_data_call(nSim, mqtt_profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0); 
	QL_SOCKET_LOG("===data call result:%d", ret);
	if(ret != 0){
		QL_SOCKET_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(nSim, mqtt_profile_idx, &info);
	if(ret != 0){
		QL_SOCKET_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(nSim, mqtt_profile_idx);
		return -1;
	}
    QL_SOCKET_LOG("info->mqtt_profile_idx: %d", info.profile_idx);
	QL_SOCKET_LOG("info->ip_version: %d", info.ip_version);
            
	QL_SOCKET_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_SOCKET_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_SOCKET_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_SOCKET_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);

	return 0;
}


/* 日志回调函数, SDK的日志会从这里输出 */
static int32_t demo_state_logcb(int32_t code, char *message)
{
    QL_SOCKET_LOG("%s", message);
    return 0;
}

/* MQTT事件回调函数, 当网络连接/重连/断开时被触发, 事件定义见core/aiot_mqtt_api.h */
static void demo_mqtt_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            QL_SOCKET_LOG("AIOT_MQTTEVT_CONNECT\n");
            /* TODO: 处理SDK建连成功, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        /* SDK因为网络状况被动断连后, 自动发起重连已成功 */
        case AIOT_MQTTEVT_RECONNECT: {
            QL_SOCKET_LOG("AIOT_MQTTEVT_RECONNECT\n");
            /* TODO: 处理SDK重连成功, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        /* SDK因为网络的状况而被动断开了连接, network是底层读写失败, heartbeat是没有按预期得到服务端心跳应答 */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            QL_SOCKET_LOG("AIOT_MQTTEVT_DISCONNECT: %s\n", cause);
            /* TODO: 处理SDK被动断连, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        default: {

        }
    }
}

/* MQTT默认消息处理回调, 当SDK从服务器收到MQTT消息时, 且无对应用户回调处理时被调用 */
static void demo_mqtt_default_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    switch (packet->type) {
        case AIOT_MQTTRECV_HEARTBEAT_RESPONSE: {
            QL_SOCKET_LOG("heartbeat response\n");
            /* TODO: 处理服务器对心跳的回应, 一般不处理 */
        }
        break;

        case AIOT_MQTTRECV_SUB_ACK: {
            QL_SOCKET_LOG("suback, res: -0x%04X, packet id: %d, max qos: %d\n",
                   -packet->data.sub_ack.res, packet->data.sub_ack.packet_id, packet->data.sub_ack.max_qos);
            /* TODO: 处理服务器对订阅请求的回应, 一般不处理 */
        }
        break;

        case AIOT_MQTTRECV_PUB: {
            QL_SOCKET_LOG("pub, qos: %d, topic: %.*s\n", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            QL_SOCKET_LOG("pub, payload: %.*s\n", packet->data.pub.payload_len, packet->data.pub.payload);
            /* TODO: 处理服务器下发的业务报文 */
        }
        break;

        case AIOT_MQTTRECV_PUB_ACK: {
            QL_SOCKET_LOG("puback, packet id: %d\n", packet->data.pub_ack.packet_id);
            /* TODO: 处理服务器对QoS1上报消息的回应, 一般不处理 */
        }
        break;

        default: {

        }
    }
}

/* 执行aiot_mqtt_process的线程, 包含心跳发送和QoS1消息重发 */
static void  demo_mqtt_process_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_mqtt_process_task_running) {
        res = aiot_mqtt_process(args);
        if (res == STATE_USER_INPUT_EXEC_DISABLED) {
            break;
        }
        ql_rtos_task_sleep_s(1);
    }
    return ;
}

/* 执行aiot_mqtt_recv的线程, 包含网络自动重连和从服务器收取MQTT消息 */
static void demo_mqtt_recv_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_mqtt_recv_task_running) {
        res = aiot_mqtt_recv(args);
        if (res < STATE_SUCCESS) {
            if (res == STATE_USER_INPUT_EXEC_DISABLED) {
                break;
            }
            ql_rtos_task_sleep_s(1);
        }
    }
    return ;
}

static void ql_alisdk_mqtt_client_task(void *arg)
{
    int32_t     res = STATE_SUCCESS;
    void       *mqtt_handle = NULL;
    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台上海站点的域名后缀 */
    char        host[100] = {0}; /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 443;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "a1q5JbHXaCp";
    char *device_name       = "jiji";
    char *device_secret     = "23f2bf0ab7ffff2057e174c5e12b02c3";


	/*执行拨号操作，设备联网*/
	if(datacall_satrt() != 0)
		goto exit;
	
    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb);

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;  
    cred.max_tls_fragment = 16384; 
    cred.sni_enabled = 1;                               
    cred.x509_server_cert = ali_ca_cert;                 
    cred.x509_server_cert_len = strlen(ali_ca_cert);     

    mqtt_handle = aiot_mqtt_init();
    if (mqtt_handle == NULL) {
        QL_SOCKET_LOG("aiot_mqtt_init failed\n");
        goto exit;
    }     

    /* TODO: 如果以下代码不被注释, 则例程会用TCP而不是TLS连接云平台 */

    {
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
    }
    

    snprintf(host, 100, "%s.%s", product_key, url);
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_HOST, (void *)host);
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PORT, (void *)&port);
	uint16_t pdp = mqtt_profile_idx;
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PROFILE_IDX, (void *)&pdp);
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PRODUCT_KEY, (void *)product_key);
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_DEVICE_NAME, (void *)device_name);
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_DEVICE_SECRET, (void *)device_secret);
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_NETWORK_CRED, (void *)&cred);
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_RECV_HANDLER, (void *)demo_mqtt_default_recv_handler);
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_EVENT_HANDLER, (void *)demo_mqtt_event_handler);
	int time_delay_ms = 5000;
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_CONNECT_TIMEOUT_MS, (void *)&time_delay_ms);

    /* 与服务器建立MQTT连接 */
    res = aiot_mqtt_connect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&mqtt_handle);
        QL_SOCKET_LOG("aiot_mqtt_connect failed: -0x%04X\n", -res);
        goto exit;
    }  

    /* MQTT 订阅topic功能示例, 请根据自己的业务需求进行使用 */
	{
        char *sub_topic = "/a1q5JbHXaCp/jiji/user/get";
                          

        res = aiot_mqtt_sub(mqtt_handle, sub_topic, NULL, 1, NULL);
        if (res < 0) {
            QL_SOCKET_LOG("aiot_mqtt_sub failed, res: -0x%04X\n", -res);
            goto exit;
        }
    }  

    /* MQTT 发布消息功能示例, 请根据自己的业务需求进行使用 */
    {
        char *pub_topic = "/a1q5JbHXaCp/jiji/user/yuanxk";
        char *pub_payload = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";

        res = aiot_mqtt_pub(mqtt_handle, pub_topic, (uint8_t *)pub_payload, (uint32_t)strlen(pub_payload), 0);
        if (res < 0) {
            QL_SOCKET_LOG("aiot_mqtt_sub failed, res: -0x%04X\n", -res);
            goto exit;
        }
    }   

    /* 创建一个单独的线程, 专用于执行aiot_mqtt_process, 它会自动发送心跳保活, 以及重发QoS1的未应答报文 */
    g_mqtt_process_task_running = 1;

    res = ql_rtos_task_create(&ql_alisdk_mqtt_process_task_ref, 16*1024, 24, "client_process", demo_mqtt_process_thread, (void *)mqtt_handle,5);
    if (res < 0) {
        QL_SOCKET_LOG("pthread_create demo_mqtt_process_thread failed: %d\n", res);
        goto exit;
    }  

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    g_mqtt_recv_task_running = 1;
	
    res = ql_rtos_task_create(&ql_alisdk_mqtt_recv_task_ref, 16*1024, 25, "client_recv", demo_mqtt_recv_thread, (void *)mqtt_handle,5);
    if (res < 0) {
        QL_SOCKET_LOG("pthread_create demo_mqtt_recv_thread failed: %d\n", res);
        goto exit;
    }  

    /* 主循环进入休眠 */
    while (1) {
        ql_rtos_task_sleep_s(1);
    }

    /* 断开MQTT连接, 一般不会运行到这里 */
    res = aiot_mqtt_disconnect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&mqtt_handle);
        QL_SOCKET_LOG("aiot_mqtt_disconnect failed: -0x%04X\n", -res);
        goto exit;
    }

    /* 销毁MQTT实例, 一般不会运行到这里 */
    res = aiot_mqtt_deinit(&mqtt_handle);
    if (res < STATE_SUCCESS) {
        QL_SOCKET_LOG("aiot_mqtt_deinit failed: -0x%04X\n", -res);
        goto exit;
    }

    g_mqtt_process_task_running = 0;
    g_mqtt_recv_task_running = 0;
    ql_rtos_task_delete(ql_alisdk_mqtt_process_task_ref);
    ql_rtos_task_delete(ql_alisdk_mqtt_recv_task_ref);

    return;
exit:
	ql_rtos_task_delete(NULL);
}

#define QL_ALI_TASK_STACK_SIZE     		4096
#define QL_ALI_TASK_PRIO          	 	    APP_PRIORITY_NORMAL
#define QL_ALI_TASK_EVENT_CNT      		5


void ql_ali_sdk_mqtt_client_test_init(void)
{
    int err;
    ql_task_t ali_task = NULL;
	
	err = ql_rtos_task_create(&ali_task, QL_ALI_TASK_STACK_SIZE, QL_ALI_TASK_PRIO, "QALI", ql_alisdk_mqtt_client_task, NULL, QL_ALI_TASK_EVENT_CNT);
	
	if (err != QL_OSI_SUCCESS)
	{
        return;
	}

}


