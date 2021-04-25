/* 杩欎釜渚嬬▼婕旂ず浜嗙敤SDK閰嶇疆Bootstrap浼氳瘽瀹炰緥鐨勫弬鏁?, 骞跺彂璧疯姹傚拰鎺ユ敹搴旂瓟, 涔嬪悗
 *
 * + 濡傛灉鎺ユ敹搴旂瓟澶辫触浜?, 閿€姣佸疄渚?, 鍥炴敹璧勬簮, 缁撴潫绋嬪簭閫€鍑?
 * + 濡傛灉鎺ユ敹搴旂瓟鎴愬姛, 鍦╜demo_bootstrap_recv_handler()`鐨勫簲绛斿鐞嗗洖璋冨嚱鏁颁腑, 婕旂ず瑙ｆ瀽鑾峰彇鏈嶅姟绔簲绛旂殑鍐呭
 *
 * 闇€瑕佺敤鎴峰叧娉ㄦ垨淇敼鐨勯儴鍒?, 宸茬敤 `TODO` 鍦ㄦ敞閲婁腑鏍囨槑
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_bootstrap_api.h"
#include "aiot_mqtt_api.h"

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


#define bootstrap_profile_idx	1

typedef struct {
    uint32_t code;
    char *host;
    uint16_t port;
} demo_info_t;

/* 浣嶄簬portfiles/aiot_port鏂囦欢澶逛笅鐨勭郴缁熼€傞厤鍑芥暟闆嗗悎 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 浣嶄簬external/ali_ca_cert.c涓殑鏈嶅姟鍣ㄨ瘉涔? */
extern const char *ali_ca_cert;

ql_task_t ql_alisdk_bootstrap_task_ref;
ql_task_t ql_alisdk_bootstrap_process_task_ref;
ql_task_t ql_alisdk_bootstrap_recv_task_ref;
static uint8_t g_bootstrap_process_task_running = 0;
static uint8_t g_bootstrap_recv_task_running = 0;


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

    ql_set_data_call_asyn_mode(nSim, bootstrap_profile_idx, 0);

    uart_printf("===start data call====");
    ret = ql_start_data_call(nSim, bootstrap_profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0);
    uart_printf("===data call result:%d", ret);
    if (ret != 0)
    {
        uart_printf("====data call failure!!!!=====");
    }
    memset(&info, 0x00, sizeof(ql_data_call_info_s));

    ret = ql_get_data_call_info(nSim, bootstrap_profile_idx, &info);
    if (ret != 0)
    {
        uart_printf("ql_get_data_call_info ret: %d", ret);
        ql_stop_data_call(nSim, bootstrap_profile_idx);
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


/* TODO: 濡傛灉瑕佸叧闂棩蹇?, 灏辨妸杩欎釜鍑芥暟瀹炵幇涓虹┖, 濡傛灉瑕佸噺灏戞棩蹇?, 鍙牴鎹甤ode閫夋嫨涓嶆墦鍗?
 *
 * 渚嬪: [1580829318.855][LK-040B] > POST /auth/bootstrap HTTP/1.1
 *
 * 涓婇潰杩欐潯鏃ュ織鐨刢ode灏辨槸040B(鍗佸叚杩涘埗), code鍊肩殑瀹氫箟瑙乧omponents/bootstrap/aiot_bootstrap_api.h
 *
 */

/* 鏃ュ織鍥炶皟鍑芥暟, SDK鐨勬棩蹇椾細浠庤繖閲岃緭鍑? */
static int32_t demo_state_logcb(int32_t code, char *message)
{
    uart_printf("%s", message);
    return 0;
}

/* MQTT浜嬩欢鍥炶皟鍑芥暟, 褰撶綉缁滆繛鎺?/閲嶈繛/鏂紑鏃惰瑙﹀彂, 浜嬩欢瀹氫箟瑙乧ore/aiot_mqtt_api.h */
static void demo_mqtt_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    switch (event->type) {
        /* SDK鍥犱负鐢ㄦ埛璋冪敤浜哸iot_mqtt_connect()鎺ュ彛, 涓巑qtt鏈嶅姟鍣ㄥ缓绔嬭繛鎺ュ凡鎴愬姛 */
        case AIOT_MQTTEVT_CONNECT: {
            uart_printf("AIOT_MQTTEVT_CONNECT\n");
        }
        break;

        /* SDK鍥犱负缃戠粶鐘跺喌琚姩鏂繛鍚?, 鑷姩鍙戣捣閲嶈繛宸叉垚鍔? */
        case AIOT_MQTTEVT_RECONNECT: {
            uart_printf("AIOT_MQTTEVT_RECONNECT\n");
        }
        break;

        /* SDK鍥犱负缃戠粶鐨勭姸鍐佃€岃鍔ㄦ柇寮€浜嗚繛鎺?, network鏄簳灞傝鍐欏け璐?, heartbeat鏄病鏈夋寜棰勬湡寰楀埌鏈嶅姟绔績璺冲簲绛? */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            uart_printf("AIOT_MQTTEVT_DISCONNECT: %s\n", cause);
        }
        break;

        default: {

        }
    }
}

/* MQTT榛樿娑堟伅澶勭悊鍥炶皟, 褰揝DK浠庢湇鍔″櫒鏀跺埌MQTT娑堟伅鏃?, 涓旀棤瀵瑰簲鐢ㄦ埛鍥炶皟澶勭悊鏃惰璋冪敤 */
static void demo_mqtt_default_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    switch (packet->type) {
        case AIOT_MQTTRECV_HEARTBEAT_RESPONSE: {
            uart_printf("heartbeat response\n");
        }
        break;

        case AIOT_MQTTRECV_SUB_ACK: {
            uart_printf("suback, res: -0x%04X, packet id: %d, max qos: %d\n",
                   -packet->data.sub_ack.res, packet->data.sub_ack.packet_id, packet->data.sub_ack.max_qos);
        }
        break;

        case AIOT_MQTTRECV_PUB: {
            uart_printf("pub, qos: %d, topic: %.*s\n", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            uart_printf("pub, payload: %.*s\n", packet->data.pub.payload_len, packet->data.pub.payload);
        }
        break;

        case AIOT_MQTTRECV_PUB_ACK: {
            uart_printf("puback, packet id: %d\n", packet->data.pub_ack.packet_id);
        }
        break;

        default: {

        }
    }
}

/* 鎵цaiot_mqtt_process鐨勭嚎绋?, 鍖呭惈蹇冭烦鍙戦€佸拰QoS1娑堟伅閲嶅彂 */
static void demo_mqtt_process_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_bootstrap_process_task_running) {
        res = aiot_mqtt_process(args);
        if (res == STATE_USER_INPUT_EXEC_DISABLED) {
            break;
        }
        ql_rtos_task_sleep_s(1);
    }
}

/* 鎵цaiot_mqtt_recv鐨勭嚎绋?, 鍖呭惈缃戠粶鑷姩閲嶈繛鍜屼粠鏈嶅姟鍣ㄦ敹鍙朚QTT娑堟伅 */
static void demo_mqtt_recv_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_bootstrap_recv_task_running) {
        res = aiot_mqtt_recv(args);
        if (res < STATE_SUCCESS) {
            if (res == STATE_USER_INPUT_EXEC_DISABLED) {
                break;
            }
            ql_rtos_task_sleep_s(1);
        }
    }
}

static int32_t demo_mqtt_start(void *mqtt_handle, char *product_key, char *device_name, char *device_secret, char *host, uint16_t port)
{
    int32_t     res = STATE_SUCCESS;
    aiot_sysdep_network_cred_t cred; /* 瀹夊叏鍑嵁缁撴瀯浣?, 濡傛灉瑕佺敤TLS, 杩欎釜缁撴瀯浣撲腑閰嶇疆CA璇佷功绛夊弬鏁? */

    /* 鍒涘缓SDK鐨勫畨鍏ㄥ嚟鎹?, 鐢ㄤ簬寤虹珛TLS杩炴帴 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;  /* 浣跨敤RSA璇佷功鏍￠獙MQTT鏈嶅姟绔? */
    cred.max_tls_fragment = 16384; /* 鏈€澶х殑鍒嗙墖闀垮害涓?16K, 鍏跺畠鍙€夊€艰繕鏈?4K, 2K, 1K, 0.5K */
    cred.sni_enabled = 1;                               /* TLS寤鸿繛鏃?, 鏀寔Server Name Indicator */
    cred.x509_server_cert = ali_ca_cert;                 /* 鐢ㄦ潵楠岃瘉MQTT鏈嶅姟绔殑RSA鏍硅瘉涔? */
    cred.x509_server_cert_len = strlen(ali_ca_cert);     /* 鐢ㄦ潵楠岃瘉MQTT鏈嶅姟绔殑RSA鏍硅瘉涔﹂暱搴? */

    /* TODO: 濡傛灉浠ヤ笅浠ｇ爜涓嶈娉ㄩ噴, 鍒欎緥绋嬩細鐢═CP鑰屼笉鏄疶LS杩炴帴浜戝钩鍙? */
    
    {
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
    }
    

    /* 閰嶇疆MQTT鏈嶅姟鍣ㄥ湴鍧€ */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_HOST, (void *)host);
    /* 閰嶇疆MQTT鏈嶅姟鍣ㄧ鍙? */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PORT, (void *)&port);
    /* 閰嶇疆璁惧productKey */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PRODUCT_KEY, (void *)product_key);
    /* 閰嶇疆璁惧deviceName */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_DEVICE_NAME, (void *)device_name);
    /* 閰嶇疆璁惧deviceSecret */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_DEVICE_SECRET, (void *)device_secret);
    /* 閰嶇疆缃戠粶杩炴帴鐨勫畨鍏ㄥ嚟鎹?, 涓婇潰宸茬粡鍒涘缓濂戒簡 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_NETWORK_CRED, (void *)&cred);
    /* 閰嶇疆MQTT榛樿娑堟伅鎺ユ敹鍥炶皟鍑芥暟 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_RECV_HANDLER, (void *)demo_mqtt_default_recv_handler);
    /* 閰嶇疆MQTT浜嬩欢鍥炶皟鍑芥暟 */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_EVENT_HANDLER, (void *)demo_mqtt_event_handler);

	/* 閰嶇疆MQTT杩炴帴瓒呮椂鏃堕棿 */
	int time_delay_ms = 5000;
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_CONNECT_TIMEOUT_MS, (void *)&time_delay_ms);
	/* 閰嶇疆鎷ㄥ彿profile_idx淇℃伅(quectel骞冲彴蹇呴』鎵ц姝ゆ搷浣?)*/
	int pdp = bootstrap_profile_idx;
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PROFILE_IDX, (void *)&pdp);


    /* 涓庢湇鍔″櫒寤虹珛MQTT杩炴帴 */
    res = aiot_mqtt_connect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        /* 灏濊瘯寤虹珛杩炴帴澶辫触, 閿€姣丮QTT瀹炰緥, 鍥炴敹璧勬簮 */
        aiot_mqtt_deinit(&mqtt_handle);
        uart_printf("aiot_mqtt_connect failed: -0x%04X\n", -res);
        return -1;
    }

    /* 鍒涘缓涓€涓崟鐙殑绾跨▼, 涓撶敤浜庢墽琛宎iot_mqtt_process, 瀹冧細鑷姩鍙戦€佸績璺充繚娲?, 浠ュ強閲嶅彂QoS1鐨勬湭搴旂瓟鎶ユ枃 */
    g_bootstrap_process_task_running = 1;
    res = ql_rtos_task_create(&ql_alisdk_bootstrap_process_task_ref, 16 * 1024, 24, "client_proc", demo_mqtt_process_thread, (void*)mqtt_handle, 5);

    if (res < 0) {
        uart_printf("pthread_create demo_mqtt_process_thread failed: %d\n", res);
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    /* 鍒涘缓涓€涓崟鐙殑绾跨▼鐢ㄤ簬鎵цaiot_mqtt_recv, 瀹冧細寰幆鏀跺彇鏈嶅姟鍣ㄤ笅鍙戠殑MQTT娑堟伅, 骞跺湪鏂嚎鏃惰嚜鍔ㄩ噸杩? */
    g_bootstrap_recv_task_running = 1;
    res = ql_rtos_task_create(&ql_alisdk_bootstrap_recv_task_ref, 16 * 1024, 25, "recv_thread", demo_mqtt_recv_thread, (void*)mqtt_handle, 5);
    if (res < 0) {
        uart_printf("pthread_create demo_mqtt_recv_thread failed: %d\n", res);
        g_bootstrap_process_task_running = 0;
        ql_rtos_task_delete(ql_alisdk_bootstrap_process_task_ref);
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    return 0;
}

static int32_t demo_mqtt_stop(void **handle)
{
    int32_t res = STATE_SUCCESS;
    void *mqtt_handle = NULL;

    mqtt_handle = *handle;

    g_bootstrap_process_task_running = 0;
    g_bootstrap_recv_task_running = 0;
    ql_rtos_task_delete(ql_alisdk_bootstrap_process_task_ref);
    ql_rtos_task_delete(ql_alisdk_bootstrap_recv_task_ref);

    /* 鏂紑MQTT杩炴帴 */
    res = aiot_mqtt_disconnect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&mqtt_handle);
        uart_printf("aiot_mqtt_disconnect failed: -0x%04X\n", -res);
        return -1;
    }

    /* 閿€姣丮QTT瀹炰緥 */
    res = aiot_mqtt_deinit(&mqtt_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_mqtt_deinit failed: -0x%04X\n", -res);
        return -1;
    }

    return 0;
}

/* Bootstrap浜嬩欢鍥炶皟鍑芥暟, 褰揝DK鍙戠敓鍐呴儴浜嬩欢鏃惰璋冪敤 */
static void demo_bootstrap_event_handler(void *handle, const aiot_bootstrap_event_t *event, void *userdata)
{
    switch (event->type) {
        case AIOT_BOOTSTRAPEVT_INVALID_RESPONSE: {
            uart_printf("AIOT_BOOTSTRAPEVT_INVALID_RESPONSE\n");
        }
        break;
        case AIOT_BOOTSTRAPEVT_INVALID_CMD: {
            uart_printf("AIOT_BOOTSTRAPEVT_INVALID_CMD\n");
        }
        break;
        default: {

        }
        break;
    }
}

/* Bootstrap鎺ユ敹娑堟伅澶勭悊鍥炶皟, 褰揝DK浠庢湇鍔″櫒鏀跺埌Bootstrap娑堟伅鏃惰璋冪敤 */
static void demo_bootstrap_recv_handler(void *handle, const aiot_bootstrap_recv_t *packet, void *userdata)
{
    demo_info_t *demo_info = (demo_info_t *)userdata;

    switch (packet->type) {
        case AIOT_BOOTSTRAPRECV_STATUS_CODE: {
            demo_info->code = packet->data.status_code.code;
        }
        break;
        case AIOT_BOOTSTRAPRECV_CONNECTION_INFO: {
            demo_info->host = malloc(strlen(packet->data.connection_info.host) + 1);
            if (demo_info->host != NULL) {
                memset(demo_info->host, 0, strlen(packet->data.connection_info.host) + 1);
                /* TODO: 鍥炶皟涓渶瑕佸皢packet鎸囧悜鐨勭┖闂村唴瀹瑰鍒朵繚瀛樺ソ, 鍥犱负鍥炶皟杩斿洖鍚?, 杩欎簺绌洪棿灏变細琚玈DK閲婃斁 */
                memcpy(demo_info->host, packet->data.connection_info.host, strlen(packet->data.connection_info.host));
                demo_info->port = packet->data.connection_info.port;
            }
        }
        break;
        case AIOT_BOOTSTRAPRECV_NOTIFY: {
            uart_printf("AIOT_BOOTSTRAPRECV_NOTIFY, cmd: %d\n", packet->data.notify.cmd);

        }
        default: {

        }
        break;
    }
}

static void  ql_alisdk_boostrap_task(void *arg)
{
    int32_t res = STATE_SUCCESS;
    void *bootstrap_handle = NULL, *mqtt_handle = NULL;
    aiot_sysdep_network_cred_t cred;
    demo_info_t demo_info;

    /* TODO: 鏇挎崲涓鸿嚜宸辫澶囩殑productKey鍜宒eviceName */
    char *product_key = "a1TSu2F29co";
    char *device_name = "000001";
    char *device_secret = "bd220c3a9c6a5b3f5921485d091497e1";

    memset(&demo_info, 0, sizeof(demo_info_t));

	/*鎵ц鎷ㄥ彿鎿嶄綔锛岃澶囪仈缃?*/
	if(datacall_satrt() != 0)
		goto exit;
	

    /* 閰嶇疆SDK鐨勫簳灞備緷璧? */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 閰嶇疆SDK鐨勬棩蹇楄緭鍑? */
    aiot_state_set_logcb(demo_state_logcb);

    /* 鍒涘缓SDK鐨勫畨鍏ㄥ嚟鎹?, 鐢ㄤ簬寤虹珛TLS杩炴帴 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;  /* 浣跨敤RSA璇佷功鏍￠獙MQTT鏈嶅姟绔? */
    cred.max_tls_fragment = 16384; /* 鏈€澶х殑鍒嗙墖闀垮害涓?16K, 鍏跺畠鍙€夊€艰繕鏈?4K, 2K, 1K, 0.5K */
    cred.sni_enabled = 1;                               /* TLS寤鸿繛鏃?, 鏀寔Server Name Indicator */
    cred.x509_server_cert = ali_ca_cert;                 /* 鐢ㄦ潵楠岃瘉MQTT鏈嶅姟绔殑RSA鏍硅瘉涔? */
    cred.x509_server_cert_len = strlen(ali_ca_cert);     /* 鐢ㄦ潵楠岃瘉MQTT鏈嶅姟绔殑RSA鏍硅瘉涔﹂暱搴? */

    /* 鍒涘缓1涓狟ootstrap浼氳瘽瀹炰緥骞跺唴閮ㄥ垵濮嬪寲榛樿鍙傛暟 */
    bootstrap_handle =  aiot_bootstrap_init();
    if (bootstrap_handle == NULL) {
        uart_printf("aiot_bootstrap_init failed\n");
        goto exit;
    }

    /* 閰嶇疆Bootstrap浼氳瘽瀹炰緥
     *
     * 閰嶇疆椤硅缁嗚鏄庡彲鍙傝€?: http://gaic.alicdn.com/ztms/linkkit/html/aiot__bootstrap__api_8h.html#a9cc177610fa0842f8287ed4fa438d56d
     *
     */
    uint16_t bootstrap_pdp = bootstrap_profile_idx;
	res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_PROFILE_IDX, (void *)&bootstrap_pdp);
		if (res < STATE_SUCCESS) {
			uart_printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_PROFILE_IDX failed, res: -0x%04X\n", -res);
			aiot_bootstrap_deinit(&bootstrap_handle);
			goto exit;
		}
    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_PRODUCT_KEY, (void *)product_key);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_PRODUCT_KEY failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        goto exit;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_DEVICE_NAME, (void *)device_name);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_DEVICE_NAME failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        goto exit;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_NETWORK_CRED, (void *)&cred);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_NETWORK_CRED failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        goto exit;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_RECV_HANDLER, (void *)demo_bootstrap_recv_handler);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_RECV_HANDLER failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        goto exit;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_EVENT_HANDLER, (void *)demo_bootstrap_event_handler);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_EVENT_HANDLER failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        goto exit;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_USERDATA, (void *)&demo_info);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_USERDATA failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        goto exit;
    }

    /* 鍙戦€丅ootstrap鏈嶅姟璇锋眰 */
    res = aiot_bootstrap_send_request(bootstrap_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_bootstrap_send_request failed, res: -0x%04X\n", -res);
        goto exit;
    }

    /* 鎺ユ敹Bootstrap鏈嶅姟搴旂瓟 */
    res = aiot_bootstrap_recv(bootstrap_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_bootstrap_recv failed, res: -0x%04X\n", -res);
        goto exit;
    }

    uart_printf("status code: %d\n", demo_info.code);

    /* 鎶婃湇鍔″簲绛斾腑鐨凪QTT鍩熷悕鍦板潃鍜岀鍙ｅ彿鎵撳嵃鍑烘潵 */
    if (demo_info.host != NULL) {
        uart_printf("host: %s, port: %d\n", demo_info.host, demo_info.port);
    }else{
       goto exit;
    }

    /* 鍒涘缓1涓狹QTT瀹㈡埛绔疄渚嬪苟鍐呴儴鍒濆鍖栭粯璁ゅ弬鏁? */
    mqtt_handle = aiot_mqtt_init();
    if (mqtt_handle == NULL) {
        uart_printf("aiot_mqtt_init failed\n");
       goto exit;
    }

    /* 寤虹珛MQTT杩炴帴, 骞跺紑鍚繚娲荤嚎绋嬪拰鎺ユ敹绾跨▼ */
    res = demo_mqtt_start(mqtt_handle, product_key, device_name, device_secret, demo_info.host, demo_info.port);
    free(demo_info.host);
    if (res < 0) {
        uart_printf("demo_mqtt_start failed\n");
        goto exit;
    }

    res = aiot_bootstrap_setopt(bootstrap_handle, AIOT_BOOTSTRAPOPT_MQTT_HANDLE, (void *)mqtt_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_bootstrap_setopt AIOT_BOOTSTRAPOPT_MQTT_HANDLE failed, res: -0x%04X\n", -res);
        aiot_bootstrap_deinit(&bootstrap_handle);
        demo_mqtt_stop(&mqtt_handle);
        goto exit;
    }

    while(1) {
        ql_rtos_task_sleep_s(1);
    }

    /* 閿€姣丅ootstrap浼氳瘽瀹炰緥, 涓€鑸笉浼氳繍琛屽埌杩欓噷 */
    res = aiot_bootstrap_deinit(&bootstrap_handle);
    if (res < 0) {
        uart_printf("demo_start_stop failed\n");
        goto exit;
    }

    /* 閿€姣丮QTT瀹炰緥, 閫€鍑虹嚎绋?, 涓€鑸笉浼氳繍琛屽埌杩欓噷 */
    res = demo_mqtt_stop(&mqtt_handle);
    if (res < 0) {
        uart_printf("demo_start_stop failed\n");
        goto exit;
    }

    return;
		
exit:
	ql_rtos_task_delete(NULL);

}

#define QL_ALI_TASK_STACK_SIZE 10240
#define QL_ALI_TASK_PRIO APP_PRIORITY_NORMAL
#define QL_ALI_TASK_EVENT_CNT 5

int ql_ali_sdk_bootstrap_test_init(void)
{
	QlOSStatus   status;
    int count = 0;
    /* 涓诲惊鐜繘鍏ヤ紤鐪? */
    while (count < 10)
    {
        count++;
        ql_rtos_task_sleep_s(1);
        uart_printf("ql_ali_sdk_mqtt_client_test_init  -0x%04X\n", count);
    }
	status =ql_rtos_task_create(&ql_alisdk_bootstrap_task_ref, QL_ALI_TASK_STACK_SIZE, APP_PRIORITY_NORMAL, "ali_bootstart", ql_alisdk_boostrap_task, NULL, QL_ALI_TASK_EVENT_CNT);
	
    if (status != QL_OSI_SUCCESS)
    {
        uart_printf("ql_ali_sdk_bootstrap_test_init error");
        return -1;
    }
    
    return 0;
}


