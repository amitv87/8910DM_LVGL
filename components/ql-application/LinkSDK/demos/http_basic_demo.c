/*
 * 这个例程适用于`RTOS`这类支持Task的设备, 它演示了用SDK配置HTTP参数并成功鉴权, 然后
 *
 * + 如果放开被注释掉的 demo_http_post_lightswitch() 函数调用, 会向平台上报1条HTTP消息
 *
 * 需要用户关注或修改的部分, 已经用 TODO 在注释中标明
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aiot_http_api.h"
#include "core_http.h"
#include "aiot_sysdep_api.h"
#include "aiot_state_api.h"


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

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;
/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

ql_task_t ql_alisdk_http_client_task_ref;

#define http_profile_idx	1

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

    ql_set_data_call_asyn_mode(nSim, http_profile_idx, 0);

    uart_printf("===start data call====");
    ret = ql_start_data_call(nSim, http_profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0);
    uart_printf("===data call result:%d", ret);
    if (ret != 0)
    {
        uart_printf("====data call failure!!!!=====");
    }
    memset(&info, 0x00, sizeof(ql_data_call_info_s));

    ret = ql_get_data_call_info(nSim, http_profile_idx, &info);
    if (ret != 0)
    {
        uart_printf("ql_get_data_call_info ret: %d", ret);
        ql_stop_data_call(nSim, http_profile_idx);
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
 * 例如: [1578127395.399][LK-0409] > POST /auth HTTP/1.1
 *
 * 上面这条日志的code就是0409(十六进制), code值的定义见core/aiot_state_api.h
 *
 */

/* 日志回调函数, SDK的日志会从这里输出 */
static int32_t demo_state_logcb(int32_t code, char *message)
{
    uart_printf("%s", message);
    return 0;
}

/* HTTP事件回调函数, 当内部Token变化时被触发, 事件定义见core/aiot_http_api.h */
static void demo_http_event_handler(void *handle, const aiot_http_event_t *event, void *user_data)
{
    int32_t res;

    /* token失效事件处理 */
    if (event->type == AIOT_HTTPEVT_TOKEN_INVALID) {
        uart_printf("token invalid, invoke iot_http_auth to get new token\n");
        res = aiot_http_auth(handle);
        uart_printf("aiot_http_auth in callback, res = -0x%04x\r\n", -res);
    }
}

/* HTTP事件回调函数, 当SDK读取到网络报文时被触发, 报文描述类型见core/aiot_http_api.h */
static void demo_http_recv_handler(void *handle, const aiot_http_recv_t *packet, void *userdata)
{
    switch (packet->type) {
        case AIOT_HTTPRECV_STATUS_CODE: {
            /* TODO: 以下代码如果不被注释, SDK收到HTTP报文时, 会通过这个用户回调打印HTTP状态码, 如404, 200, 302等 */
            /* uart_printf("status code: %d\n", packet->data.status_code.code); */
			uart_printf("status code: %d\n", packet->data.status_code.code); 
        }
        break;

        case AIOT_HTTPRECV_HEADER: {
            /* TODO: 以下代码如果不被注释, SDK收到HTTP报文时, 会通过这个用户回调打印HTTP首部, 如Content-Length等 */
            /* uart_printf("key: %s, value: %s\n", packet->data.header.key, packet->data.header.value); */
			uart_printf("key: %s, value: %s\n", packet->data.header.key, packet->data.header.value);
        }
        break;

        /* TODO: 如果需要处理云平台的HTTP回应报文, 修改这里, 现在只是将回应打印出来 */
        case AIOT_HTTPRECV_BODY: {
            uart_printf("%d %s\r\n", packet->data.body.len, packet->data.body.buffer);
        }
        break;

        default: {
        }
        break;

    }
}

/* 用HTTP通道上报业务数据给云平台, 例如: 灯已关闭 */
static int32_t demo_http_post_lightswitch(void *handle)
{
    /* TODO: 如果要修改上报的数据内容, 就修改 data[] 数组 */
    char data[] = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";
    int32_t res;

    /* 用HTTP通道向云平台上报1条业务数据, 接口是aiot_http_send() */
    res = aiot_http_send(handle,
                         "/sys/a1E6c7I1GoN/product1_test_device1/thing/event/property/post",
                         (uint8_t *)data,
                         strlen(data));
    if (res < 0) {
        uart_printf("aiot_http_send res = -0x%04X\r\n", -res);
        return res;
    }

    res = aiot_http_recv(handle);
    if (res >= 0) {
        /* 成功接收到服务器应答, 且业务应答码为0, 说明数据上报成功 */
        return 0;
    } else {
        uart_printf("aiot_http_recv res = -0x%04X\r\n", -res);
        return -1;
    }
}

static void ql_ali_sdk_http_client_test(void *arg)
{
    void       *http_handle = NULL;
    int32_t     res;
    char       *url = "iot-as-http.cn-shanghai.aliyuncs.com";
    uint16_t    port = 443;
    aiot_sysdep_network_cred_t cred;

    /* TODO: 替换为自己设备的三元组 */
    char *product_key = "a1TSu2F29co";
    char *device_name = "000001";
    char *device_secret = "bd220c3a9c6a5b3f5921485d091497e1";

	/*执行拨号操作，设备联网*/
	if(datacall_satrt()!=0)
		goto exit;

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb);

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.max_tls_fragment = 16384;
    cred.x509_server_cert = ali_ca_cert;
    cred.x509_server_cert_len = strlen(ali_ca_cert);
	cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;

    /* 创建1个HTTP客户端实例并内部初始化默认参数 */
    http_handle = aiot_http_init();
	if(!http_handle)
	{
		uart_printf("aiot_http_init failed !!\n");
		goto exit;
	}
    /* 配置服务器域名 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_HOST, (void *)url);
    /* 配置服务器端口 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, (void *)&port);

	/* 配置连接超时时间 */
//	int timeout = 5000;
//	aiot_http_setopt(http_handle, AIOT_HTTPOPT_CONNECT_TIMEOUT_MS, (void *)&timeout);
//	uart_printf("http_handle   AIOT_HTTPOPT_CONNECT_TIMEOUT_MS :  %d \r\n", http_handle->connect_timeout_ms);
    /* 配置设备安全凭证 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, (void *)&cred);
    /* 配置设备三元组 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_PRODUCT_KEY, (void *)product_key);
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_DEVICE_NAME, (void *)device_name);
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_DEVICE_SECRET, (void *)device_secret);
    /* 配置数据到达时, SDK应调用的用户回调函数 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_RECV_HANDLER, demo_http_recv_handler);
    /* 配置内部状态变化时, SDK应调用的用户回调函数 */
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_EVENT_HANDLER, demo_http_event_handler);

	/* 配置拨号profile_idx信息(quectel平台必须执行此操作)*/
	uint16_t pdp = http_profile_idx;
    aiot_http_setopt(http_handle, AIOT_HTTPOPT_PROFILE_IDX, (void *)&pdp);
	uint32_t timeout = 5000;
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_CONNECT_TIMEOUT_MS, (void *)&timeout);
    /*
     * TODO: 以下代码如果不被注释, 会演示用TCP(而不是TLS)连接云平台
     */

    /*

    {
        int32_t     tcp_port = 80;
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
        aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);
        aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, &tcp_port);
    }

    */
	
    /* 设备认证, 获取token */
	uart_printf(" aiot_http_auth !!! \r\n");
    res = aiot_http_auth(http_handle);
    if (res == 0) {
        uart_printf("aiot_http_auth succeed\r\n\r\n");
    } else {
        /* 如果认证失败, 就销毁实例, 回收资源, 结束程序退出 */
        uart_printf("aiot_http_auth failed, res = -0x%04X\r\n\r\n", -res);
        aiot_http_deinit(&http_handle);
        goto exit;
    }

    /* TODO: 以下代码如果不被注释, 会演示上报 1 次数据到物联网平台 */
    /*
    demo_http_post_lightswitch(http_handle);
    uart_printf("\r\n");
     */
    uart_printf(" demo_http_post_lightswitch !!! \r\n");
	demo_http_post_lightswitch(http_handle);
	uart_printf("\r\n");

    /* 销毁HTTP实例 */
    aiot_http_deinit(&http_handle);
    uart_printf("program exit as normal return\r\n");
    uart_printf("\r\n");

    return ;
exit:
	ql_rtos_task_delete(NULL);

}

#define QL_ALI_TASK_STACK_SIZE 10240
#define QL_ALI_TASK_PRIO APP_PRIORITY_NORMAL
#define QL_ALI_TASK_EVENT_CNT 5

int ql_ali_sdk_http_client_test_init(void)
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
	status =ql_rtos_task_create(&ql_alisdk_http_client_task_ref, QL_ALI_TASK_STACK_SIZE, APP_PRIORITY_NORMAL, "http_test", ql_ali_sdk_http_client_test, NULL, QL_ALI_TASK_EVENT_CNT);
    if (status != QL_OSI_SUCCESS)
    {
        uart_printf("ql_ali_sdk_devinfo_test_init error");
        return -1;
    }
    return 0;
}


