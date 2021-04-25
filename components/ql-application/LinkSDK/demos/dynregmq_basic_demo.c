/* 这个例程演示了用SDK配置动态注册会话实例的参数, 并发起请求和接收应答, 之后
 *
 * + 如果接收应答失败了, 销毁实例, 回收资源, 结束程序退出
 * + 如果接收应答成功, 在`demo_dynregmq_recv_handler()`的应答处理回调函数中, 演示解析获取服务端应答的内容
 *
 * 需要用户关注或修改的部分, 已用 `TODO` 在注释中标明
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_dynregmq_api.h"


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

#define dynregmq_mqtt_profile_idx 1

/* 白名单模式下用于保存deviceSecret的结构体定义 */
typedef struct {
    char device_secret[64];
} demo_devinfo_wl_t;

/* 免白名单模式下用于保存mqtt建连信息clientid, username和password的结构体定义 */
typedef struct {
    char conn_clientid[128];
    char conn_username[128];
    char conn_password[64];
} demo_devinfo_nwl_t;

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

/* 用户保存白名单模式动态注册, 服务器返回的deviceSecret */
static demo_devinfo_wl_t demo_devinfo_wl;

/* 用户保存免白名单模式动态注册, 服务器返回的mqtt建连信息 */
static demo_devinfo_nwl_t demo_devinfo_nwl;

ql_task_t ql_alisdk_dynregmq_task_ref;

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

    ql_set_data_call_asyn_mode(nSim, dynregmq_mqtt_profile_idx, 0);

    uart_printf("===start data call====");
    ret = ql_start_data_call(nSim, dynregmq_mqtt_profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0);
    uart_printf("===data call result:%d", ret);
    if (ret != 0)
    {
        uart_printf("====data call failure!!!!=====");
    }
    memset(&info, 0x00, sizeof(ql_data_call_info_s));

    ret = ql_get_data_call_info(nSim, dynregmq_mqtt_profile_idx, &info);
    if (ret != 0)
    {
        uart_printf("ql_get_data_call_info ret: %d", ret);
        ql_stop_data_call(nSim, dynregmq_mqtt_profile_idx);
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
 * 例如: [1580995015.811][LK-040B] > POST /auth/register/device HTTP/1.1
 *
 * 上面这条日志的code就是040B(十六进制), code值的定义见components/dynregmq/aiot_dynregmq_api.h
 *
 */

/* 日志回调函数, SDK的日志会从这里输出 */
static int32_t demo_state_logcb(int32_t code, char *message)
{
    uart_printf("%s", message);
    return 0;
}

/* 数据处理回调, 当SDK从网络上收到dynregmq消息时被调用 */
static void demo_dynregmq_recv_handler(void *handle, const aiot_dynregmq_recv_t *packet, void *userdata)
{
    switch (packet->type) {
        /* TODO: 回调中需要将packet指向的空间内容复制保存好, 因为回调返回后, 这些空间就会被SDK释放 */
        case AIOT_DYNREGMQRECV_DEVICEINFO_WL: {
            if (strlen(packet->data.deviceinfo_wl.device_secret) >= sizeof(demo_devinfo_wl.device_secret)) {
                break;
            }

            /* 白名单模式, 用户务必要对device_secret进行持久化保存 */
            memset(&demo_devinfo_wl, 0, sizeof(demo_devinfo_wl_t));
            memcpy(demo_devinfo_wl.device_secret, packet->data.deviceinfo_wl.device_secret,
                   strlen(packet->data.deviceinfo_wl.device_secret));
        }
        break;
        /* TODO: 回调中需要将packet指向的空间内容复制保存好, 因为回调返回后, 这些空间就会被SDK释放 */
        case AIOT_DYNREGMQRECV_DEVICEINFO_NWL: {
            if (strlen(packet->data.deviceinfo_nwl.clientid) >= sizeof(demo_devinfo_nwl.conn_clientid) ||
                strlen(packet->data.deviceinfo_nwl.username) >= sizeof(demo_devinfo_nwl.conn_username) ||
                strlen(packet->data.deviceinfo_nwl.password) >= sizeof(demo_devinfo_nwl.conn_password)) {
                break;
            }

            /* 免白名单模式, 用户务必要对MQTT的建连信息clientid, username和password进行持久化保存 */
            memset(&demo_devinfo_nwl, 0, sizeof(demo_devinfo_nwl_t));
            memcpy(demo_devinfo_nwl.conn_clientid, packet->data.deviceinfo_nwl.clientid,
                   strlen(packet->data.deviceinfo_nwl.clientid));
            memcpy(demo_devinfo_nwl.conn_username, packet->data.deviceinfo_nwl.username,
                   strlen(packet->data.deviceinfo_nwl.username));
            memcpy(demo_devinfo_nwl.conn_password, packet->data.deviceinfo_nwl.password,
                   strlen(packet->data.deviceinfo_nwl.password));
        }
        break;
        default: {

        }
        break;
    }
}

static void ali_dynregmq_test(void *arg)
{
    int32_t     res = STATE_SUCCESS;
    void       *dynregmq_handle = NULL;
    char       *host = "a18wPzZJzNG.iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台动态注册国内站点, TODO: 前缀为设备的productKey */
    uint16_t    port = 443;            /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t
    cred;    /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */
    uint8_t     nowhitelist = 1;        /* 是否使用免白名单功能 */

    /* TODO: 替换为自己设备的productKey, productSecret和deviceName */
    char *product_key = "a1ATIg2Dgt3";
    char *device_name = "product1_test_device1";
    char *product_secret = "sRwayrNWiGwWYgEr";

	/*执行拨号操作，设备联网*/
	if(datacall_satrt()!=0)
		return ;

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb);

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;  /* 使用RSA证书校验DYNREGMQ服务端 */
    cred.max_tls_fragment = 16384; /* 最大的分片长度为16K, 其它可选值还有4K, 2K, 1K, 0.5K */
    cred.sni_enabled = 1;                               /* TLS建连时, 支持Server Name Indicator */
    cred.x509_server_cert = ali_ca_cert;                 /* 用来验证服务端的RSA根证书 */
    cred.x509_server_cert_len = strlen(ali_ca_cert);     /* 用来验证服务端的RSA根证书长度 */

    /* 创建1个dynregmq客户端实例并内部初始化默认参数 */
    dynregmq_handle = aiot_dynregmq_init();
    if (dynregmq_handle == NULL) {
        uart_printf("aiot_dynregmq_init failed\n");
        return ;
    }

    /* 配置连接的服务器地址 */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_HOST, (void *)host);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_HOST failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }

    /* 配置连接的服务器端口 */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_PORT, (void *)&port);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_PORT failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }
	
	/* 配置拨号profile_idx信息(quectel平台必须执行此操作)*/
	int pdp = dynregmq_mqtt_profile_idx;	
	res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_PROFILE_INDEX, (void *)&pdp);
		if (res < STATE_SUCCESS) {
			uart_printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_PORT failed, res: -0x%04X\n", -res);
			aiot_dynregmq_deinit(&dynregmq_handle);
			return ;
	}

    /* 配置设备productKey */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_PRODUCT_KEY, (void *)product_key);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_PRODUCT_KEY failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }

    /* 配置设备productSecret */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_PRODUCT_SECRET, (void *)product_secret);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_PRODUCT_SECRET failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }

    /* 配置设备deviceName */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_DEVICE_NAME, (void *)device_name);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_DEVICE_NAME failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }

    /* 配置网络连接的安全凭据, 上面已经创建好了 */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_NETWORK_CRED, (void *)&cred);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_NETWORK_CRED failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }

    /* 配置DYNREGMQ默认消息接收回调函数 */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_RECV_HANDLER, (void *)demo_dynregmq_recv_handler);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_RECV_HANDLER failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }
	
	int time_delay_ms = 5000;
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_CONNECT_TIMEOUT_MS, (void *)&time_delay_ms);

    /* 配置DYNREGMQ动态注册模式,
    1. 配置为0则为白名单模式, 用户必须提前在控制台录入deviceName, 动态注册完成后服务会返回deviceSecret, 用户可通过
       AIOT_DYNREGMQRECV_DEVICEINFO_WL类型数据回调获取到deviceSecret.
    2. 配置为1则为免白名单模式, 用户无需提前在控制台录入deviceName, 动态注册完成后服务会返回MQTT建连信息, 用户可通过
       AIOT_DYNREGMQRECV_DEVICEINFO_NWL类型数据回调获取到clientid, username, password. 用户需要将这三个参数通过
       aiot_mqtt_setopt接口以AIOT_MQTTOPT_CLIENTID, AIOT_MQTTOPT_USERNAME, AIOT_MQTTOPT_PASSWORD配置选项
       配置到MQTT句柄中。
    */
    res = aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_NO_WHITELIST, (void *)&nowhitelist);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_setopt AIOT_DYNREGMQOPT_NO_WHITELIST failed, res: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }

    /* 发送动态注册请求 */
    res = aiot_dynregmq_send_request(dynregmq_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_send_request failed: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }

    /* 接收动态注册请求 */
    res = aiot_dynregmq_recv(dynregmq_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_recv failed: -0x%04X\n", -res);
        aiot_dynregmq_deinit(&dynregmq_handle);
        return ;
    }

    /* 把服务应答中的信息打印出来 */
    if (nowhitelist == 0) {
        uart_printf("device secret: %s\n", demo_devinfo_wl.device_secret);
    } else {
        uart_printf("clientid: %s\n", demo_devinfo_nwl.conn_clientid);
        uart_printf("username: %s\n", demo_devinfo_nwl.conn_username);
        uart_printf("password: %s\n", demo_devinfo_nwl.conn_password);
    }

    /* 销毁动态注册会话实例 */
    res = aiot_dynregmq_deinit(&dynregmq_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_dynregmq_deinit failed: -0x%04X\n", -res);
        return ;
    }

    return;
}

#define QL_ALI_TASK_STACK_SIZE 10240
#define QL_ALI_TASK_PRIO APP_PRIORITY_NORMAL
#define QL_ALI_TASK_EVENT_CNT 5

int ql_ali_sdk_dynregmq_test_init(void)
{
	QlOSStatus   status;
    int count = 0;
    /* 主循环进入休眠 */
    while (count < 10)
    {
        count++;
        ql_rtos_task_sleep_s(1);
        uart_printf("ql_ali_sdk_dynregmq_test_init  -0x%04X\n", count);
    }
	status =ql_rtos_task_create(&ql_alisdk_dynregmq_task_ref, QL_ALI_TASK_STACK_SIZE, APP_PRIORITY_NORMAL, "dynregmq_test", ali_dynregmq_test, NULL, QL_ALI_TASK_EVENT_CNT);
    if (status != QL_OSI_SUCCESS)
    {
        uart_printf("ql_ali_sdk_dynregmq_test_init error");
        return -1;
    }
    return 0;
}

