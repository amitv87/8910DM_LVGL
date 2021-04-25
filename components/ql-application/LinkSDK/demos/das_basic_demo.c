/*
 * 杩欎釜渚嬬▼閫傜敤浜巂Linux`杩欑被鏀寔pthread鐨凱OSIX璁惧, 瀹冩紨绀轰簡鐢⊿DK閰嶇疆MQTT鍙傛暟骞跺缓绔嬭繛鎺?, 涔嬪悗鍒涘缓2涓嚎绋?
 *
 * + 涓€涓嚎绋嬬敤浜庝繚娲婚暱杩炴帴
 * + 涓€涓嚎绋嬬敤浜庢帴鏀舵秷鎭?, 骞跺湪鏈夋秷鎭埌杈炬椂杩涘叆榛樿鐨勬暟鎹洖璋?, 鍦ㄨ繛鎺ョ姸鎬佸彉鍖栨椂杩涘叆浜嬩欢鍥炶皟
 *
 * 鎺ョ潃鍦∕QTT杩炴帴涓婂彂閫丯TP鏌ヨ璇锋眰, 濡傛灉浜戝钩鍙扮殑鍥炲簲鎶ユ枃鍒拌揪, 浠庢帴鏀剁嚎绋嬩細璋冪敤NTP娑堟伅澶勭悊鐨勫洖璋冨嚱鏁?, 鎶婂鏃跺悗鐨勬湰鍦版椂闂存墦鍗板嚭鏉?
 *
 * 闇€瑕佺敤鎴峰叧娉ㄦ垨淇敼鐨勯儴鍒?, 宸茬粡鐢? TODO 鍦ㄦ敞閲婁腑鏍囨槑
 *
 */
#include <stdio.h>
#include <string.h>

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_das_api.h"

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


#define das_mqtt_profile_idx 1


/* 浣嶄簬portfiles/aiot_port鏂囦欢澶逛笅鐨勭郴缁熼€傞厤鍑芥暟闆嗗悎 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

ql_task_t ql_alisdk_das_task_ref;
static ql_task_t g_das_mqtt_process_task_ref;
static ql_task_t g_das_mqtt_recv_task_ref;
static uint8_t g_das_mqtt_process_task_ref_running = 0;
static uint8_t g_das_mqtt_recv_task_ref_running = 0;

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

    ql_set_data_call_asyn_mode(nSim, das_mqtt_profile_idx, 0);

    uart_printf("===start data call====");
    ret = ql_start_data_call(nSim, das_mqtt_profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0);
    uart_printf("===data call result:%d", ret);
    if (ret != 0)
    {
        uart_printf("====data call failure!!!!=====");
    }
    memset(&info, 0x00, sizeof(ql_data_call_info_s));

    ret = ql_get_data_call_info(nSim, das_mqtt_profile_idx, &info);
    if (ret != 0)
    {
        uart_printf("ql_get_data_call_info ret: %d", ret);
        ql_stop_data_call(nSim, das_mqtt_profile_idx);
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
 * 渚嬪: [1581501698.455][LK-0309] pub: /ext/ntp/a13FN5TplKq/ntp_basic_demo/request
 *
 * 涓婇潰杩欐潯鏃ュ織鐨刢ode灏辨槸0309(鍗佸叚杩涘埗), code鍊肩殑瀹氫箟瑙乧ore/aiot_state_api.h
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
            uart_printf("pub, qos: %d, topic: %d %s\n", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            uart_printf("pub, payload: %d %s\n", packet->data.pub.payload_len, packet->data.pub.payload);
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

    while (g_das_mqtt_process_task_ref_running) {
        res = aiot_mqtt_process(args);
        if (res == STATE_USER_INPUT_EXEC_DISABLED) {
            break;
        }
        ql_rtos_task_sleep_s(1);
    }
    return;
}

/* 鎵цaiot_mqtt_recv鐨勭嚎绋?, 鍖呭惈缃戠粶鑷姩閲嶈繛鍜屼粠鏈嶅姟鍣ㄦ敹鍙朚QTT娑堟伅 */
static void demo_mqtt_recv_thread(void *args)
{
    int32_t res = STATE_SUCCESS;

    while (g_das_mqtt_recv_task_ref_running) {
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

static int32_t demo_mqtt_start(void **handle)
{
    int32_t     res = STATE_SUCCESS;
    void       *mqtt_handle = NULL;
    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 闃块噷浜戝钩鍙颁笂娴风珯鐐圭殑鍩熷悕鍚庣紑 */
    char        host[100] = {0}; /* 鐢ㄨ繖涓暟缁勬嫾鎺ヨ澶囪繛鎺ョ殑浜戝钩鍙扮珯鐐瑰叏鍦板潃, 瑙勫垯鏄? ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 443;      /* 鏃犺璁惧鏄惁浣跨敤TLS杩炴帴闃块噷浜戝钩鍙?, 鐩殑绔彛閮芥槸443 */
    aiot_sysdep_network_cred_t cred; /* 瀹夊叏鍑嵁缁撴瀯浣?, 濡傛灉瑕佺敤TLS, 杩欎釜缁撴瀯浣撲腑閰嶇疆CA璇佷功绛夊弬鏁? */

    /* TODO: 鏇挎崲涓鸿嚜宸辫澶囩殑涓夊厓缁? */
    char *product_key = "a1TSu2F29co";
    char *device_name = "000001";
    char *device_secret = "bd220c3a9c6a5b3f5921485d091497e1";

	/*鎵ц鎷ㄥ彿鎿嶄綔锛岃澶囪仈缃?*/
	if(datacall_satrt()!=0)
		return -1;

    /* 閰嶇疆SDK鐨勫簳灞備緷璧? */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 閰嶇疆SDK鐨勬棩蹇楄緭鍑? */
    aiot_state_set_logcb(demo_state_logcb);

    /* 鍒涘缓SDK鐨勫畨鍏ㄥ嚟鎹?, 鐢ㄤ簬寤虹珛TLS杩炴帴 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;  /* 浣跨敤RSA璇佷功鏍￠獙MQTT鏈嶅姟绔? */
    cred.max_tls_fragment = 16384; /* 鏈€澶х殑鍒嗙墖闀垮害涓?16K, 鍏跺畠鍙€夊€艰繕鏈?4K, 2K, 1K, 0.5K */
    cred.sni_enabled = 1;                               /* TLS寤鸿繛鏃?, 鏀寔Server Name Indicator */

    /* 鍒涘缓1涓狹QTT瀹㈡埛绔疄渚嬪苟鍐呴儴鍒濆鍖栭粯璁ゅ弬鏁? */
    mqtt_handle = aiot_mqtt_init();
    if (mqtt_handle == NULL) {
        uart_printf("aiot_mqtt_init failed\n");
        return -1;
    }

    /* TODO: 濡傛灉浠ヤ笅浠ｇ爜涓嶈娉ㄩ噴, 鍒欎緥绋嬩細鐢═CP鑰屼笉鏄疶LS杩炴帴浜戝钩鍙? */
    /*
    {
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
    }
    */

    snprintf(host, 100, "%s.%s", product_key, url);
    /* 閰嶇疆MQTT鏈嶅姟鍣ㄥ湴鍧€ */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_HOST, (void *)host);
    /* 閰嶇疆MQTT鏈嶅姟鍣ㄧ鍙? */
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PORT, (void *)&port);

	int pdp = das_mqtt_profile_idx;
	aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PROFILE_IDX, (void *)&pdp);
	
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

    /* 涓庢湇鍔″櫒寤虹珛MQTT杩炴帴 */
    res = aiot_mqtt_connect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        /* 灏濊瘯寤虹珛杩炴帴澶辫触, 閿€姣丮QTT瀹炰緥, 鍥炴敹璧勬簮 */
        aiot_mqtt_deinit(&mqtt_handle);
        uart_printf("aiot_mqtt_connect failed: -0x%04X\n", -res);
        return -1;
    }

    /* 鍒涘缓涓€涓崟鐙殑绾跨▼, 涓撶敤浜庢墽琛宎iot_mqtt_process, 瀹冧細鑷姩鍙戦€佸績璺充繚娲?, 浠ュ強閲嶅彂QoS1鐨勬湭搴旂瓟鎶ユ枃 */
    g_das_mqtt_process_task_ref_running = 1;
	res = ql_rtos_task_create(&g_das_mqtt_process_task_ref, 24000, 24, "client_proc", demo_mqtt_process_thread, (void *)mqtt_handle, 5);
    if (res < 0) {
        uart_printf("pthread_create demo_mqtt_process_thread failed: %d\n", res);
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    /* 鍒涘缓涓€涓崟鐙殑绾跨▼鐢ㄤ簬鎵цaiot_mqtt_recv, 瀹冧細寰幆鏀跺彇鏈嶅姟鍣ㄤ笅鍙戠殑MQTT娑堟伅, 骞跺湪鏂嚎鏃惰嚜鍔ㄩ噸杩? */
    g_das_mqtt_recv_task_ref_running = 1;
	res = ql_rtos_task_create(&g_das_mqtt_recv_task_ref, 24000, 25, "recv_test", demo_mqtt_recv_thread, (void*)mqtt_handle, 5);
    if (res < 0) {
        uart_printf("pthread_create demo_mqtt_recv_thread failed: %d\n", res);
        g_das_mqtt_process_task_ref_running = 0;
        aiot_mqtt_deinit(&mqtt_handle);
        return -1;
    }

    *handle = mqtt_handle;

    return 0;
}

int32_t demo_mqtt_stop(void **handle)
{
    int32_t res = STATE_SUCCESS;
    void *mqtt_handle = NULL;

    mqtt_handle = *handle;

    g_das_mqtt_process_task_ref_running = 0;
    g_das_mqtt_recv_task_ref_running = 0;
    ql_rtos_task_delete(g_das_mqtt_process_task_ref);
    ql_rtos_task_delete(g_das_mqtt_recv_task_ref);

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

static void ali_das_test(void *arg)
{
    int32_t     res = STATE_SUCCESS;
    void       *mqtt_handle = NULL, *das_handle = NULL;

	uart_printf("==========%s entry !!==============\r\n",__func__);
    /* 寤虹珛MQTT杩炴帴, 骞跺紑鍚繚娲荤嚎绋嬪拰鎺ユ敹绾跨▼ */
    res = demo_mqtt_start(&mqtt_handle);
    if (res < 0) {
        uart_printf("demo_mqtt_start failed\n");
        return;
    }

    /* 鍒涘缓1涓猲tp瀹㈡埛绔疄渚嬪苟鍐呴儴鍒濆鍖栭粯璁ゅ弬鏁? */
    das_handle = aiot_das_init();
    if (das_handle == NULL) {
        demo_mqtt_stop(&mqtt_handle);
        uart_printf("aiot_das_init failed\n");
        return;
    }

    res = aiot_das_setopt(das_handle, AIOT_DASOPT_MQTT_HANDLE, mqtt_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_das_setopt AIOT_DASOPT_MQTT_HANDLE failed, res: -0x%04X\n", -res);
        aiot_das_deinit(&das_handle);
        demo_mqtt_stop(&mqtt_handle);
        return;
    }

    res = aiot_das_setopt(das_handle, AIOT_DASOPT_SET_FIRMWARE_VERSION, (void *)"mzf_firmware_version");
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_das_setopt AIOT_DASOPT_SET_FIRMWARE_VERSION failed, res: -0x%04X\n", -res);
    }

    while (1) {
        aiot_das_stepping(das_handle);
        ql_rtos_task_sleep_s(1);
    }

    /* 閿€姣丯TP瀹炰緥, 涓€鑸笉浼氳繍琛屽埌杩欓噷 */
    res = aiot_das_deinit(&das_handle);
    if (res < STATE_SUCCESS) {
        demo_mqtt_stop(&mqtt_handle);
        uart_printf("aiot_das_deinit failed: -0x%04X\n", -res);
        return;
    }

    /* 閿€姣丮QTT瀹炰緥, 閫€鍑虹嚎绋?, 涓€鑸笉浼氳繍琛屽埌杩欓噷 */
    res = demo_mqtt_stop(&mqtt_handle);
    if (res < 0) {
        uart_printf("demo_start_stop failed\n");
        return;
    }

    return;
}

#define QL_ALI_TASK_STACK_SIZE 10240
#define QL_ALI_TASK_PRIO APP_PRIORITY_NORMAL
#define QL_ALI_TASK_EVENT_CNT 5

int ql_ali_sdk_das_test_init(void)
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
	status =ql_rtos_task_create(&ql_alisdk_das_task_ref, QL_ALI_TASK_STACK_SIZE, APP_PRIORITY_NORMAL, "das_test", ali_das_test, NULL, QL_ALI_TASK_EVENT_CNT);

    if (status != QL_OSI_SUCCESS)
    {
        uart_printf("ql_ali_sdk_das_test_init error");
        return -1;
    }
    return 0;
}

