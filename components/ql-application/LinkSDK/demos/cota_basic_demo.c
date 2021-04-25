/*
 *  杩欎釜渚嬪瓙涓昏鐢ㄤ簬婕旂ず濡備綍浠庤澶囩鑾峰彇瀛樺偍鍦ㄤ簯绔殑鍏充簬杩欎釜璁惧鐨勮繙绋嬮厤缃?
 *
 * 闇€瑕佺敤鎴峰叧娉ㄦ垨淇敼鐨勯儴鍒?, 宸茬敤 `TODO` 鍦ㄦ敞閲婁腑鏍囨槑
 *
 */


/* TODO: 鏈緥瀛愮敤鍒颁簡sleep鍑芥暟, 鎵€浠ュ寘鍚簡unistd.h. 濡傛灉鐢ㄦ埛鑷繁鐨勫簱涓湁鍙互鏇夸唬鐨勫嚱鏁?, 鍒欏彲浠ュ皢unistd.h鏇挎崲鎺?
 *
 */

#include <stdio.h>
#include <string.h>

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_ota_api.h"

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


#define cota_model_profile_idx	1

/* TODO: 鏇挎崲涓鸿嚜宸辫澶囩殑涓夊厓缁? */
char *product_key = "a1TSu2F29co";
char *device_name = "000001";
char *device_secret = "bd220c3a9c6a5b3f5921485d091497e1";

/* 浣嶄簬external/ali_ca_cert.c涓殑鏈嶅姟鍣ㄨ瘉涔? */
extern const char *ali_ca_cert;
void *g_dl_handle = NULL;
ql_task_t ql_alisdk_cota_task_ref;


/* 浣嶄簬portfiles/aiot_port鏂囦欢澶逛笅鐨勭郴缁熼€傞厤鍑芥暟闆嗗悎 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

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

    ql_set_data_call_asyn_mode(nSim, cota_model_profile_idx, 0);

    uart_printf("===start data call====");
    ret = ql_start_data_call(nSim, cota_model_profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0);
    uart_printf("===data call result:%d", ret);
    if (ret != 0)
    {
        uart_printf("====data call failure!!!!=====");
    }
    memset(&info, 0x00, sizeof(ql_data_call_info_s));

    ret = ql_get_data_call_info(nSim, cota_model_profile_idx, &info);
    if (ret != 0)
    {
        uart_printf("ql_get_data_call_info ret: %d", ret);
        ql_stop_data_call(nSim, cota_model_profile_idx);
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
 * 渚嬪: [1578463098.611][LK-0309] pub: /ota/device/upgrade/a13FN5TplKq/ota_demo
 *
 * 涓婇潰杩欐潯鏃ュ織鐨刢ode灏辨槸0309(鍗佸叚杩涘埗), code鍊肩殑瀹氫箟瑙乧ore/aiot_state_api.h
 *
 */

/* 鏃ュ織鍥炶皟鍑芥暟, SDK鐨勬棩蹇椾細浠庤繖閲岃緭鍑? */
static int32_t demo_state_logcb(int32_t code, char *message)
{
    /* 涓嬭浇杩滅▼閰嶇疆鐨勬椂鍊欎細鏈夊ぇ閲忕殑HTTP鏀跺寘鏃ュ織, 閫氳繃code绛涢€夊嚭鏉ュ叧闂? */
    if (STATE_HTTP_LOG_RECV_CONTENT != code) {
        uart_printf("%s", message);
    }
    return 0;
}

/* MQTT浜嬩欢鍥炶皟鍑芥暟, 褰撶綉缁滆繛鎺?/閲嶈繛/鏂紑鏃惰瑙﹀彂, 浜嬩欢瀹氫箟瑙乧ore/aiot_mqtt_api.h */
static void demo_mqtt_event_handler(void *handle, const aiot_mqtt_event_t *const event, void *userdata)
{
    switch (event->type) {
        /* SDK鍥犱负鐢ㄦ埛璋冪敤浜哸iot_mqtt_connect()鎺ュ彛, 涓巑qtt鏈嶅姟鍣ㄥ缓绔嬭繛鎺ュ凡鎴愬姛 */
        case AIOT_MQTTEVT_CONNECT: {
            uart_printf("AIOT_MQTTEVT_CONNECT\r\n");
            /* TODO: 澶勭悊SDK寤鸿繛鎴愬姛, 涓嶅彲浠ュ湪杩欓噷璋冪敤鑰楁椂杈冮暱鐨勯樆濉炲嚱鏁? */
        }
        break;

        /* SDK鍥犱负缃戠粶鐘跺喌琚姩鏂繛鍚?, 鑷姩鍙戣捣閲嶈繛宸叉垚鍔? */
        case AIOT_MQTTEVT_RECONNECT: {
            uart_printf("AIOT_MQTTEVT_RECONNECT\r\n");
            /* TODO: 澶勭悊SDK閲嶈繛鎴愬姛, 涓嶅彲浠ュ湪杩欓噷璋冪敤鑰楁椂杈冮暱鐨勯樆濉炲嚱鏁? */
        }
        break;

        /* SDK鍥犱负缃戠粶鐨勭姸鍐佃€岃鍔ㄦ柇寮€浜嗚繛鎺?, network鏄簳灞傝鍐欏け璐?, heartbeat鏄病鏈夋寜棰勬湡寰楀埌鏈嶅姟绔績璺冲簲绛? */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            uart_printf("AIOT_MQTTEVT_DISCONNECT: %s\r\n", cause);
            /* TODO: 澶勭悊SDK琚姩鏂繛, 涓嶅彲浠ュ湪杩欓噷璋冪敤鑰楁椂杈冮暱鐨勯樆濉炲嚱鏁? */
        }
        break;
        default: {

        }
    }
}

/* MQTT榛樿娑堟伅澶勭悊鍥炶皟, 褰揝DK浠庢湇鍔″櫒鏀跺埌MQTT娑堟伅鏃?, 涓旀棤瀵瑰簲鐢ㄦ埛鍥炶皟澶勭悊鏃惰璋冪敤 */
static void demo_mqtt_default_recv_handler(void *handle, const aiot_mqtt_recv_t *const packet, void *userdata)
{
    switch (packet->type) {
        case AIOT_MQTTRECV_HEARTBEAT_RESPONSE: {
            uart_printf("heartbeat response\r\n");
            /* TODO: 澶勭悊鏈嶅姟鍣ㄥ蹇冭烦鐨勫洖搴?, 涓€鑸笉澶勭悊 */
        }
        break;
        case AIOT_MQTTRECV_SUB_ACK: {
            uart_printf("suback, res: -0x%04X, packet id: %d, max qos: %d\r\n",
                   -packet->data.sub_ack.res, packet->data.sub_ack.packet_id, packet->data.sub_ack.max_qos);
            /* TODO: 澶勭悊鏈嶅姟鍣ㄥ璁㈤槄璇锋眰鐨勫洖搴?, 涓€鑸笉澶勭悊 */
        }
        break;
        case AIOT_MQTTRECV_PUB: {
            uart_printf("pub, qos: %d, topic: %d %s\r\n", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            uart_printf("pub, payload: %d %s\r\n", packet->data.pub.payload_len, packet->data.pub.payload);
            /* TODO: 澶勭悊鏈嶅姟鍣ㄤ笅鍙戠殑涓氬姟鎶ユ枃 */
        }
        break;
        case AIOT_MQTTRECV_PUB_ACK: {
            uart_printf("puback, packet id: %d\r\n", packet->data.pub_ack.packet_id);
            /* TODO: 澶勭悊鏈嶅姟鍣ㄥQoS1涓婃姤娑堟伅鐨勫洖搴?, 涓€鑸笉澶勭悊 */
        }
        break;
        default: {

        }
    }
}

/* 涓嬭浇鏀跺寘鍥炶皟, 鐢ㄦ埛璋冪敤 aiot_download_recv() 鍚?, SDK鏀跺埌鏁版嵁浼氳繘鍏ヨ繖涓嚱鏁?, 鎶婁笅杞藉埌鐨勬暟鎹氦缁欑敤鎴? */
/* TODO: 鍦ㄧ敤鎴锋敹鍒拌繙绋嬮厤缃殑鏁版嵁鍚?, 闇€瑕佽嚜宸卞喅瀹氬浣曞鐞? */
static void demo_download_recv_handler(void *handle, const aiot_download_recv_t *packet, void *userdata)
{
    /* 鐩墠鍙敮鎸? packet->type 涓? AIOT_DLRECV_HTTPBODY 鐨勬儏鍐? */
    if (!packet || AIOT_DLRECV_HTTPBODY != packet->type) {
        return;
    }
    int32_t percent = packet->data.percent;
    uint8_t *src_buffer = packet->data.buffer;
    uint32_t src_buffer_len = packet->data.len;

    /* 濡傛灉 percent 涓鸿礋鏁?, 璇存槑鍙戠敓浜嗘敹鍖呭紓甯告垨鑰卍igest鏍￠獙閿欒 */
    if (percent < 0) {
        /* 鏀跺寘寮傚父鎴栬€卍igest鏍￠獙閿欒 */
        uart_printf("exception happend, percent is %d\r\n", percent);
        return;
    }
    aiot_download_report_progress(handle, percent);
    uart_printf("config len is %d, config content is %.*s\r\n", src_buffer_len, src_buffer_len, (char *)src_buffer);
}

/* 鐢ㄦ埛閫氳繃 aiot_ota_setopt() 娉ㄥ唽鐨凮TA娑堟伅澶勭悊鍥炶皟, 濡傛灉SDK鏀跺埌浜哋TA鐩稿叧鐨凪QTT娑堟伅, 浼氳嚜鍔ㄨ瘑鍒?, 璋冪敤杩欎釜鍥炶皟鍑芥暟 */
static void demo_ota_recv_handler(void *ota_handle, aiot_ota_recv_t *ota_msg, void *userdata)
{
    switch (ota_msg->type) {
        case AIOT_OTARECV_COTA: {
            uint32_t res = 0;
            uint16_t port = 443;
            uint32_t max_buffer_len = 2048;
            aiot_sysdep_network_cred_t cred;
            void *dl_handle = aiot_download_init();

            if (NULL == dl_handle || NULL == ota_msg->task_desc) {
                return;
            }

            uart_printf("configId: %s, configSize: %u Bytes\r\n", ota_msg->task_desc->version,
                   ota_msg->task_desc->size_total);

            memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
            cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;
            cred.max_tls_fragment = 16384;
            cred.x509_server_cert = ali_ca_cert;
            cred.x509_server_cert_len = strlen(ali_ca_cert);

            /* 璁剧疆涓嬭浇鏃朵负TLS涓嬭浇 */
            if ((STATE_SUCCESS != aiot_download_setopt(dl_handle, AIOT_DLOPT_NETWORK_CRED, (void *)(&cred))) ||
                /* 璁剧疆涓嬭浇鏃惰闂殑鏈嶅姟鍣ㄧ鍙ｅ彿 */
                (STATE_SUCCESS != aiot_download_setopt(dl_handle, AIOT_DLOPT_NETWORK_PORT, (void *)(&port))) ||
                /* 璁剧疆涓嬭浇鐨勪换鍔′俊鎭?, 閫氳繃杈撳叆鍙傛暟 ota_msg 涓殑 task_desc 鎴愬憳寰楀埌, 鍐呭惈涓嬭浇鍦板潃, 杩滅▼閰嶇疆鐨勫ぇ灏忓拰鐗堟湰鍙?*/
                (STATE_SUCCESS != aiot_download_setopt(dl_handle, AIOT_DLOPT_TASK_DESC, (void *)(ota_msg->task_desc))) ||
                /* 璁剧疆涓嬭浇鍐呭鍒拌揪鏃?, SDK灏嗚皟鐢ㄧ殑鍥炶皟鍑芥暟 */
                (STATE_SUCCESS != aiot_download_setopt(dl_handle, AIOT_DLOPT_RECV_HANDLER, (void *)(demo_download_recv_handler))) ||
                /* 璁剧疆鍗曟涓嬭浇鏈€澶х殑buffer闀垮害, 姣忓綋杩欎釜闀垮害鐨勫唴瀛樿婊′簡鍚庝細閫氱煡鐢ㄦ埛 */
                (STATE_SUCCESS != aiot_download_setopt(dl_handle, AIOT_DLOPT_BODY_BUFFER_MAX_LEN, (void *)(&max_buffer_len))) ||
                /* 鍙戦€乭ttp鐨凣ET璇锋眰缁檋ttp鏈嶅姟鍣? */
                (STATE_SUCCESS != aiot_download_send_request(dl_handle))) {
                if (res != STATE_SUCCESS) {
                    aiot_download_deinit(&dl_handle);
                    break;
                }
            }
            g_dl_handle = dl_handle;
            break;
        }
        default:
            break;
    }
}


static void  ql_alisdk_cota_task(void *arg)
{
    int32_t res = STATE_SUCCESS;
    void *mqtt_handle = NULL;
    char *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 闃块噷浜戝钩鍙颁笂娴风珯鐐圭殑鍩熷悕鍚庣紑 */
    char host[100] = {0}; /* 鐢ㄨ繖涓暟缁勬嫾鎺ヨ澶囪繛鎺ョ殑浜戝钩鍙扮珯鐐瑰叏鍦板潃, 瑙勫垯鏄? ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t port = 443; /* 鏃犺璁惧鏄惁浣跨敤TLS杩炴帴闃块噷浜戝钩鍙?, 鐩殑绔彛閮芥槸443 */
    aiot_sysdep_network_cred_t cred; /* 瀹夊叏鍑嵁缁撴瀯浣?, 濡傛灉瑕佺敤TLS, 杩欎釜缁撴瀯浣撲腑閰嶇疆CA璇佷功绛夊弬鏁? */
    void *ota_handle = NULL;
    uint32_t timeout_ms = 0;

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

    /* 鍒涘缓1涓狹QTT瀹㈡埛绔疄渚嬪苟鍐呴儴鍒濆鍖栭粯璁ゅ弬鏁? */
    mqtt_handle = aiot_mqtt_init();
    if (mqtt_handle == NULL) {
        return;
    }

    snprintf(host, 100, "%s.%s", product_key, url);
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
	int pdp = cota_model_profile_idx;
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PROFILE_IDX, (void *)&pdp);

    /* 涓嶮QTT渚嬬▼涓嶅悓鐨勬槸, 杩欓噷闇€瑕佸鍔犲垱寤篛TA浼氳瘽瀹炰緥鐨勮鍙? */
    ota_handle = aiot_ota_init();
    if (NULL == ota_handle) {
        goto exit;
    }

    /* 鐢ㄤ互涓嬭鍙?, 鎶奜TA浼氳瘽鍜孧QTT浼氳瘽鍏宠仈璧锋潵 */
    aiot_ota_setopt(ota_handle, AIOT_OTAOPT_MQTT_HANDLE, mqtt_handle);
    /* 鐢ㄤ互涓嬭鍙?, 璁剧疆OTA浼氳瘽鐨勬暟鎹帴鏀跺洖璋?, SDK鏀跺埌OTA鐩稿叧鎺ㄩ€佹椂, 浼氳繘鍏ヨ繖涓洖璋冨嚱鏁? */
    aiot_ota_setopt(ota_handle, AIOT_OTAOPT_RECV_HANDLER, demo_ota_recv_handler);

    /* 涓庢湇鍔″櫒寤虹珛MQTT杩炴帴 */
    res = aiot_mqtt_connect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_mqtt_connect failed: -0x%04X\r\n", -res);
        /* 灏濊瘯寤虹珛杩炴帴澶辫触, 閿€姣丮QTT瀹炰緥, 鍥炴敹璧勬簮 */
        goto exit;
    }

    /* 鍙戦€佽姹?, 鑾峰彇浜戠鐨勮繙绋嬮厤缃? */
    {
        char *topic_string = "/sys/a1dhWKuKqX5/cota_basic_demo/thing/config/get";
        char *payload_string = "{\"id\":\"123\",\"params\":{\"configScope\":\"product\",\"getType\":\"file\"}}";

        res = aiot_mqtt_pub(mqtt_handle, topic_string, (uint8_t *)payload_string, strlen(payload_string), 0);
        if (res < STATE_SUCCESS) {
            uart_printf("aiot_mqtt_pub failed: -0x%04X\r\n", -res);
            /* 灏濊瘯寤虹珛杩炴帴澶辫触, 閿€姣丮QTT瀹炰緥, 鍥炴敹璧勬簮 */
            goto exit;
        }
    }

    while (1) {
        aiot_mqtt_process(mqtt_handle);
        res = aiot_mqtt_recv(mqtt_handle);

        if (res == STATE_SYS_DEPEND_NWK_CLOSED) {
            ql_rtos_task_sleep_s(1);
        }
        if (NULL != g_dl_handle) {
            /* 瀹屾垚杩滅▼閰嶇疆鐨勬帴鏀跺墠, 灏唌qtt鐨勬敹鍖呰秴鏃惰皟鏁村埌100ms, 浠ュ噺灏戜袱娆¤繙绋嬮厤缃殑涓嬭浇闂撮殧*/
            int32_t ret = aiot_download_recv(g_dl_handle);
            timeout_ms = 100;

            if (STATE_DOWNLOAD_FINISHED == ret) {
                aiot_download_deinit(&g_dl_handle);
                /* 瀹屾垚杩滅▼閰嶇疆鐨勬帴鏀跺悗, 灏唌qtt鐨勬敹鍖呰秴鏃惰皟鏁村洖鍒伴粯璁ゅ€?5000ms */
                timeout_ms = 5000;
            }
            aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_RECV_TIMEOUT_MS, (void *)&timeout_ms);
        }
    }

    /* 鏂紑MQTT杩炴帴, 涓€鑸笉浼氳繍琛屽埌杩欓噷 */
    res = aiot_mqtt_disconnect(mqtt_handle);
    if (res < STATE_SUCCESS) {
        uart_printf("aiot_mqtt_disconnect failed: -0x%04X\r\n", -res);
        goto exit;
    }

exit:
    while (1) {
        /* 閿€姣丮QTT瀹炰緥, 涓€鑸笉浼氳繍琛屽埌杩欓噷 */
        res = aiot_mqtt_deinit(&mqtt_handle);

        if (res < STATE_SUCCESS) {
            uart_printf("aiot_mqtt_deinit failed: -0x%04X\r\n", -res);
            return;
        } else {
            break;
        }
    }

    /* 閿€姣丱TA瀹炰緥, 涓€鑸笉浼氳繍琛屽埌杩欓噷 */
    aiot_ota_deinit(&ota_handle);

    return ;
}

#define QL_ALI_TASK_STACK_SIZE 10240
#define QL_ALI_TASK_PRIO APP_PRIORITY_NORMAL
#define QL_ALI_TASK_EVENT_CNT 5

int ql_ali_sdk_cota_test_init(void)
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
    status =ql_rtos_task_create(&ql_alisdk_cota_task_ref, QL_ALI_TASK_STACK_SIZE, APP_PRIORITY_NORMAL, "ali_cota_task", ql_alisdk_cota_task, NULL, QL_ALI_TASK_EVENT_CNT);
	
    if (status != QL_OSI_SUCCESS)
    {
        uart_printf("ql_ali_sdk_cota_test_init error");
        return -1;
    }

    return 0;
}



