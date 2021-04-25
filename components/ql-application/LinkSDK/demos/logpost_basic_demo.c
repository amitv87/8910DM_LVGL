/*
 * 这个例程适用于`Linux`这类支持pthread的POSIX设备, 它演示了用SDK配置MQTT参数并建立连接, 之后创建2个线程
 *
 * + 一个线程用于保活长连接
 * + 一个线程用于接收消息, 并在有消息到达时进入默认的数据回调, 在连接状态变化时进入事件回调
 *
 * 接着在MQTT连接上发送设备端日志到云端, 注意日志上云通道默认是关闭的, 用户需要到云端控制台打开设备端日志服务开关。
 *
 * 需要用户关注或修改的部分, 已经用 TODO 在注释中标明
 *
 */
#include <stdio.h>
#include <string.h>

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_logpost_api.h"

#include "ali_demo.h"
#include "ql_api_osi.h"
#include "ql_api_nw.h"
#include "ali_demo.h"
#include "ql_log.h"
#include "ql_api_datacall.h"
#include "sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"

#include "lwip/netdb.h"
#include "lwip/netif.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"

#define QL_SOCKET_LOG_LEVEL QL_LOG_LEVEL_INFO
#define uart_printf(msg, ...) QL_LOG(QL_SOCKET_LOG_LEVEL, "linkSDK ", msg, ##__VA_ARGS__)

#define logpost_model_profile_idx	1

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

ql_task_t ql_alisdk_logpost_task_ref;
static ql_task_t ql_alisdk_logpost_process_task_ref;
static ql_task_t ql_alisdk_logpost_recv_task_ref;
static uint8_t g_logpost_process_task_running = 0;
static uint8_t g_logpost_recv_task_running = 0;


static int datacall_satrt(void)
{
    int ret = 0;
    int i = 0;
    ql_data_call_info_s info;
    char ip4_addr_str[16] = {0};
    uint8_t nSim = 0;

    ql_rtos_task_sleep_s(10);
    uart_printf("========== socket demo start ==========");
    uart_printf("wait for network register done");

    while ((ret = ql_network_register_wait(nSim, 120)) != 0 && i < 10)
    {
        i++;
        ql_rtos_task_sleep_s(1);
    }
    if (ret == 0)
    {
        i = 0;
        uart_printf("====network registered!!!!====");
    }
    else
    {
        uart_printf("====network register failure!!!!!====");
        goto exit;
    }

    ql_set_data_call_asyn_mode(nSim, logpost_model_profile_idx, 0);

    uart_printf("===start data call====");
    ret = ql_start_data_call(nSim, logpost_model_profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0);
    uart_printf("===data call result:%d", ret);
    if (ret != 0)
    {
        uart_printf("====data call failure!!!!=====");
    }
    memset(&info, 0x00, sizeof(ql_data_call_info_s));

    ret = ql_get_data_call_info(nSim, logpost_model_profile_idx, &info);
    if (ret != 0)
    {
        uart_printf("ql_get_data_call_info ret: %d", ret);
        ql_stop_data_call(nSim, logpost_model_profile_idx);
        goto exit;
    }
    uart_printf("info->mqtt_profile_idx: %d", info.profile_idx);
    uart_printf("info->ip_version: %d", info.ip_version);

    uart_printf("info->v4.state: %d", info.v4.state);
    inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
    uart_printf("info.v4.addr.ip: %s\r\n", ip4_addr_str);

    inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
    uart_printf("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

    inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
    uart_printf("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);

    return 0;
exit:
    return -1;
}


/* TODO: 如果要关闭日志, 就把这个函数实现为空, 如果要减少日志, 可根据code选择不打印
 *
 * 例如: [1581501698.455][LK-0309] pub: /sys/a13FN5TplKq/logpost_basic_demo/thing/deviceinfo/update
 *
 * 上面这条日志的code就是0309(十六进制), code值的定义见core/aiot_state_api.h
 *
 */

/* 日志回调函数, SDK的日志会从这里输出, 禁止在此函数中调用SDK API */
static int32_t demo_state_logcb(int32_t code, char *message)
{
    uart_printf("%s", message);
    return 0;
}

/* MQTT事件回调函数, 当网络连接/重连/断开时被触发, 事件定义见core/aiot_mqtt_api.h */
static void demo_mqtt_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            uart_printf("AIOT_MQTTEVT_CONNECT\r\n");
        }
        break;

        /* SDK因为网络状况被动断连后, 自动发起重连已成功 */
        case AIOT_MQTTEVT_RECONNECT: {
            uart_printf("AIOT_MQTTEVT_RECONNECT\r\n");
        }
        break;

        /* SDK因为网络的状况而被动断开了连接, network是底层读写失败, heartbeat是没有按预期得到服务端心跳应答 */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            uart_printf("AIOT_MQTTEVT_DISCONNECT: %s\r\n", cause);
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
            uart_printf("heartbeat response\r\n");
        }
        break;

        case AIOT_MQTTRECV_SUB_ACK: {
            uart_printf("suback, res: -0x%04X, packet id: %d, max qos: %d\r\n",
                   -packet->data.sub_ack.res, packet->data.sub_ack.packet_id, packet->data.sub_ack.max_qos);
        }
        break;

        case AIOT_MQTTRECV_PUB: {
            uart_printf("pub, qos: %d, topic: %d %s\r\n", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            uart_printf("pub, payload: %d %s\r\n", packet->data.pub.payload_len, packet->data.pub.payload);
        }
        break;

        case AIOT_MQTTRECV_PUB_ACK: {
            uart_printf("puback, packet id: %d\r\n", packet->data.pub_ack.packet_id);
        }
        break;

        default: {

        }
    }
}

/* 执行aiot_mqtt_process的线程, 包含心跳发送和QoS1消息重发 */
static void demo_mqtt_process_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_logpost_process_task_running) {
        res = aiot_mqtt_process(args);
        if (res == STATE_USER_INPUT_EXEC_DISABLED) {
            break;
        }
        ql_rtos_task_sleep_s(1);
    }
    return;
}

/* 执行aiot_mqtt_recv的线程, 包含网络自动重连和从服务器收取MQTT消息 */
static void demo_mqtt_recv_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_logpost_recv_task_running) {
        res = aiot_mqtt_recv(args);
        if (res < STATE_SUCCESS) {
            if (res == STATE_USER_INPUT_EXEC_DISABLED) {
                break;
            }
            ql_rtos_task_sleep_s(1);
        }
    }
    return;
}

static int32_t demo_mqtt_start(void **handle, char *product_key, char *device_name, char *device_secret)
{
    int32_t     res = STATE_SUCCESS;
    void       *mqtt_handle = NULL;
    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台上海站点的域名后缀 */
    char        host[100] = {0}; /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 443;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

	/*执行拨号操作，设备联网*/
	if(datacall_satrt() != 0)
		return -1;

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb);

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;  /* 使用RSA证书校验MQTT服务端 */
    cred.max_tls_fragment = 16384; /* 最大的分片长度为16K, 其它可选值还有4K, 2K, 1K, 0.5K */
    cred.sni_enabled = 1;                               /* TLS建连时, 支持Server Name Indicator */
    cred.x509_server_cert = ali_ca_cert;                 /* 用来验证MQTT服务端的RSA根证书 */
    cred.x509_server_cert_len = strlen(ali_ca_cert);     /* 用来验证MQTT服务端的RSA根证书长度 */

    /* 创建1个MQTT客户端实例并内部初始化默认参数 */
    mqtt_handle = aiot_mqtt_init();
    if (mqtt_handle == NULL) {
        uart_printf("aiot_mqtt_init failed\r\n");
        return -1;
    }

    /* 如果以下代码不被注释, 则例程会用TCP而不是TLS连接云平台 */
    /*
    {
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
    }
    */

    snprintf(host, 100, "%s.%s", product_key, url);
    /* 配置MQTT服务器地址 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_HOST, (void *)host);
    /* 配置MQTT服务器端口 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PORT, (void *)&port);
    /* 配置设备productKey */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PRODUCT_KEY, (void *)product_key);
    /* 配置设备deviceName */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_DEVICE_NAME, (void *)device_name);
    /* 配置设备deviceSecret */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_DEVICE_SECRET, (void *)device_secret);
    /* 配置网络连接的安全凭据, 上面已经创建好了 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_NETWORK_CRED, (void *)&cred);
    /* 配置MQTT默认消息接收回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_RECV_HANDLER, (void *)demo_mqtt_default_recv_handler);
    /* 配置MQTT事件回调函数 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_EVENT_HANDLER, (void *)demo_mqtt_event_handler);
	
	/* 配置MQTT连接超时时间 */
	int time_delay_ms = 5000;
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_CONNECT_TIMEOUT_MS, (void *)&time_delay_ms);
	/* 配置拨号profile_idx信息(quectel平台必须执行此操作)*/
	int pdp = logpost_model_profile_idx;
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PROFILE_IDX, (void *)&pdp);

    /* 与服务器建立MQTT连接 */
    res = aiot_mqtt_connect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        /* 尝试建立连接失败, 销毁MQTT实例, 回收资源 */
        aiot_mqtt_deinit(&mqtt_handle);
        uart_printf("aiot_mqtt_connect failed: -0x%04X\r\n", -res);
        return -1;
    }

    /* 创建一个单独的线程, 专用于执行aiot_mqtt_process, 它会自动发送心跳保活, 以及重发QoS1的未应答报文 */
    g_logpost_process_task_running = 1;
    res = ql_rtos_task_create(&ql_alisdk_logpost_process_task_ref, 24576, 24, "client_task", demo_mqtt_process_thread, (void *)mqtt_handle, 5);
    if (res < 0) {
        uart_printf("pthread_create demo_mqtt_process_thread failed: %d\r\n", res);
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    /* 创建一个单独的线程用于执行aiot_mqtt_recv, 它会循环收取服务器下发的MQTT消息, 并在断线时自动重连 */
    g_logpost_recv_task_running = 1;
    res = ql_rtos_task_create(&ql_alisdk_logpost_recv_task_ref, 24576, 25, "recv_task", demo_mqtt_recv_thread, (void *)mqtt_handle, 5);
    if (res < 0) {
        uart_printf("pthread_create demo_mqtt_recv_thread failed: %d\r\n", res);
        g_logpost_process_task_running = 0;
        ql_rtos_task_delete(ql_alisdk_logpost_process_task_ref);
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    *handle = mqtt_handle;

    return 0;
}

static int32_t demo_mqtt_stop(void **handle)
{
    int32_t res = STATE_SUCCESS;
    void *mqtt_handle = NULL;

    mqtt_handle = *handle;

    g_logpost_process_task_running = 0;
    g_logpost_recv_task_running = 0;
    ql_rtos_task_delete(ql_alisdk_logpost_process_task_ref);
    ql_rtos_task_delete(ql_alisdk_logpost_recv_task_ref);

    /* 断开MQTT连接 */
    res = aiot_mqtt_disconnect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&mqtt_handle);
        uart_printf("aiot_mqtt_disconnect failed: -0x%04X\r\n", -res);
        return -1;
    }

    /* 销毁MQTT实例 */
    res = aiot_mqtt_deinit(&mqtt_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_mqtt_deinit failed: -0x%04X\r\n", -res);
        return -1;
    }

    return 0;
}

/* 事件处理回调, 用户可通过此回调获取日志上报通道的开关状态 */
static void demo_logpost_event_handler(void *handle, const aiot_logpost_event_t *event, void *userdata)
{
    switch (event->type) {
        /* 日志配置事件, 当设备连云成功或者用户在控制台页面控制日志开关时会收到此事件 */
        case AIOT_LOGPOSTEVT_CONFIG_DATA: {
            uart_printf("log switch state is: %d\r\n", event->data.config_data.on_off);
        }
        default:
            break;
    }
}

/* 上报日志到云端 */
static void demo_send_log(void *handle, char *log)
{
    int32_t res = 0;
    aiot_logpost_msg_t msg;

    memset(&msg, 0, sizeof(aiot_logpost_msg_t));
    msg.timestamp = 0;                          /* 单位为ms的时间戳, 填写0则SDK将使用当前的时间戳 */
    msg.loglevel = AIOT_LOGPOST_LEVEL_DEBUG;    /* 日志级别 */
    msg.module_name = "APP";                    /* 日志对应的模块 */
    msg.code = 200;                             /* 状态码 */
    msg.msg_id = 0;                             /* 云端下行报文的消息标示符, 若无对应消息可直接填0 */
    msg.content = log;                          /* 日志内容 */

    res = aiot_logpost_send(handle, &msg);
    if (res < 0) {
        uart_printf("aiot_logpost_send failed: -0x%04X\r\n", -res);
    }
}


static void  ql_alisdk_logpost_task(void *arg)
{
    int32_t     res = STATE_SUCCESS;

    /* TODO: 替换为自己设备的三元组 */
    char *product_key = "a1TSu2F29co";
    char *device_name = "000001";
    char *device_secret = "bd220c3a9c6a5b3f5921485d091497e1";

    void *mqtt_handle = NULL, *logpost_handle = NULL;

    /* 建立MQTT连接, 并开启保活线程和接收线程 */
    res = demo_mqtt_start(&mqtt_handle, product_key, device_name, device_secret);
    if (res < 0) {
        uart_printf("demo_mqtt_start failed\r\n");
        goto exit;
    }

    /* 创建1个logpost客户端实例并内部初始化默认参数 */
    logpost_handle = aiot_logpost_init();
    if (logpost_handle == NULL) {
        demo_mqtt_stop(&mqtt_handle);
        uart_printf("aiot_logpost_init failed\r\n");
        goto exit;
    }

    /* 配置logpost会话, 把它和MQTT会话的句柄关联起来 */
    res = aiot_logpost_setopt(logpost_handle, AIOT_LOGPOSTOPT_MQTT_HANDLE, mqtt_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_logpost_setopt AIOT_LOGPOSTOPT_MQTT_HANDLE failed, res: -0x%04X\r\n", -res);
        aiot_logpost_deinit(&logpost_handle);
        demo_mqtt_stop(&mqtt_handle);
        goto exit;
    }
    res = aiot_logpost_setopt(logpost_handle, AIOT_LOGPOSTOPT_EVENT_HANDLER, (void *)demo_logpost_event_handler);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_logpost_setopt AIOT_LOGPOSTOPT_EVENT_HANDLER failed, res: -0x%04X\r\n", -res);
        aiot_logpost_deinit(&logpost_handle);
        demo_mqtt_stop(&mqtt_handle);
        goto exit;
    }

    /* 主线程进入休眠, 等云平台的logpost回应到达时, 接收线程会调用 demo_logpost_recv_handler() */
    while (1) {
        ql_rtos_task_sleep_s(10);

        /* TODO: 用户可取消注释上报日志到云端, 注意: 日志模块完成初始化后上报通道默认为关闭状态, 日志模块会在收到设备建连内部事件后立即同步云端控制台的开关状态。*/
        demo_send_log(logpost_handle, "log in while(1)");
        
    }

    /* 销毁logpost实例, 一般不会运行到这里 */
    res = aiot_logpost_deinit(&logpost_handle);
    if (res < STATE_SUCCESS) {
        demo_mqtt_stop(&mqtt_handle);
        uart_printf("aiot_logpost_deinit failed: -0x%04X\r\n", -res);
		goto exit;
    }

    /* 销毁MQTT实例, 退出线程, 一般不会运行到这里 */
    res = demo_mqtt_stop(&mqtt_handle);
    if (res < 0) {
        uart_printf("demo_start_stop failed\r\n");
        goto exit;
    }

    return ;
exit:
	ql_rtos_task_delete(NULL);

}

#define QL_ALI_TASK_STACK_SIZE 10240
#define QL_ALI_TASK_PRIO APP_PRIORITY_NORMAL
#define QL_ALI_TASK_EVENT_CNT 5

int ql_ali_sdk_logpost_test_init(void)
{
	QlOSStatus   status;
    int count = 0;
    /* 主循环进入休眠 */
    while (count < 10)
    {
        count++;
        ql_rtos_task_sleep_s(1);
        uart_printf("ql_ali_sdk_mqtt_client_test_init  -0x%04X\n", count);
    }
	status =ql_rtos_task_create(&ql_alisdk_logpost_task_ref, QL_ALI_TASK_STACK_SIZE, APP_PRIORITY_NORMAL, "log_task", ql_alisdk_logpost_task, NULL, QL_ALI_TASK_EVENT_CNT);
    if (status != QL_OSI_SUCCESS)
    {
        uart_printf("ql_ali_sdk_devinfo_test_init error");
        return -1;
    }
    return 0;
}

