/**
 * @file    bt_mesh.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020-4-29
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_MESH_H__
#define __BT_MESH_H__

#define     UNISOC_ID      						(0x000c)
#define		PTS_TEST_ID							(0x01a8)

/**----------------------------MESH CONFIG-----------------------------*/

#define MESH_CFG_NET_KEY_COUNT						(2)
#define	MESH_CFG_APP_KEY_COUNT						(4)
#define MESH_CFG_GROUP_COUNT						(5)
#define MESH_CFG_MODEL_COUNT						(5)
#define MESH_CFG_RELAY_COUNT						(5)
#define MESH_CFG_HEALTH_FAULT_SIZE           		(3)
#define MESH_CFG_REPLAY_CACHE						(5)
#define MESH_CFG_SEQ_COUNT							(4)
/**--------------------------MESH CONFIG END---------------------------*/

#define MESH_ROLE_NODE								(1)
#define MESH_ROLE_PROV								(2)

#define     MESH_PROVISIONING_SERVICE_UUID      0x1827
#define     MESH_PROXY_SERVICE_UUID             0x1828

#define     MESH_PROVISION_DATA_IN_UUID         0x2adb
#define     MESH_PROVISION_DATA_OUT_UUID        0x2adc
#define     MESH_PROXY_DATA_IN_UUID             0x2add
#define     MESH_PROXY_DATA_OUT_UUID            0x2ade

#define GAP_MESH_PBADV      0x29
#define GAP_MESH_MESSAGE    0x2A
#define GAP_MESH_BEACON     0x2B

#define OPCODE_VENDOR_MESSAGE_GET           (0xC001A8)
#define OPCODE_VENDOR_MESSAGE_SET           (0xC101A8)
#define OPCODE_VENDOR_MESSAGE_SET_UNACK     (0xC201A8)
#define OPCODE_VENDOR_MESSAGE_STATUS        (0xC301A8)
#define OPCODE_VENDOR_OTA_GET               (0xC401A8)
#define OPCODE_VENDOR_OTA_SET               (0xC501A8)
#define OPCODE_VENDOR_OTA_SET_UNACK         (0xC601A8)
#define OPCODE_VENDOR_OTA_STATUS            (0xC701A8)

#define OPCODE_GENERIC_ONOFF_GET            (0x8201)
#define OPCODE_GENERIC_ONOFF_SET            (0x8202)
#define OPCODE_GENERIC_ONOFF_SET_UNACK      (0x8203)
#define OPCODE_GENERIC_ONOFF_STATUS         (0x8204)

#define OPCODE_LIGHT_LIGHTNESS_GET              (0x824b)
#define OPCODE_LIGHT_LIGHTNESS_SET              (0x824c)
#define OPCODE_LIGHT_LIGHTNESS_SET_UNACK        (0x824d)
#define OPCODE_LIGHT_LIGHTNESS_STATUS           (0x824e)
#define OPCODE_LIGHT_LIGHTNESS_LINEAR_GET       (0x824f)
#define OPCODE_LIGHT_LIGHTNESS_LINEAR_SET       (0x8250)
#define OPCODE_LIGHT_LIGHTNESS_LINEAR_SET_UNACK (0x8251)
#define OPCODE_LIGHT_LIGHTNESS_LINEAR_STATUS    (0x8252)
#define OPCODE_LIGHT_LIGHTNESS_LAST_GET         (0x8253)
#define OPCODE_LIGHT_LIGHTNESS_LAST_STATUS      (0x8254)
#define OPCODE_LIGHT_LIGHTNESS_DEFAULT_GET      (0x8255)
#define OPCODE_LIGHT_LIGHTNESS_DEFAULT_STATUS   (0x8256)
#define OPCODE_LIGHT_LIGHTNESS_RANGE_GET        (0x8257)
#define OPCODE_LIGHT_LIGHTNESS_RANGE_STATUS     (0x8258)

#define OPCODE_LIGHT_CTL_GET                        (0x825D)
#define OPCODE_LIGHT_CTL_SET                        (0x825E)
#define OPCODE_LIGHT_CTL_SET_UNACK                  (0x825F)
#define OPCODE_LIGHT_CTL_STATUS                     (0x8260)
#define OPCODE_LIGHT_CTL_TEMPERATURE_GET            (0x8261)
#define OPCODE_LIGHT_CTL_TEMPERATURE_RANGE_GET      (0x8262)
#define OPCODE_LIGHT_CTL_TEMPERATURE_RANGE_STATUS   (0x8263)
#define OPCODE_LIGHT_CTL_TEMPERATURE_SET            (0x8264)
#define OPCODE_LIGHT_CTL_TEMPERATURE_SET_UNACK      (0x8265)
#define OPCODE_LIGHT_CTL_TEMPERATURE_STATUS         (0x8266)
#define OPCODE_LIGHT_CTL_DEFAULT_GET                (0x8267)
#define OPCODE_LIGHT_CTL_DEFAULT_STATUS             (0x8268)
#define OPCODE_LIGHT_CTL_DEFAULT_SET                (0x8269)
#define OPCODE_LIGHT_CTL_DEFAULT_SET_UNACK          (0x826A)
#define OPCODE_LIGHT_CTL_TEMPERATURE_RANG_SET       (0x826B)
#define OPCODE_LIGHT_CTL_TEMPERATURE_RANG_SET_UNACK (0x826C)

#define OPCODE_LIGHT_HSL_GET                        (0x826D)
#define OPCODE_LIGHT_HSL_HUE_GET                    (0x826E)
#define OPCODE_LIGHT_HSL_HUE_SET                    (0x826F)
#define OPCODE_LIGHT_HSL_HUE_SET_UNACK              (0x8270)
#define OPCODE_LIGHT_HSL_HUE_STATUS                 (0x8271)
#define OPCODE_LIGHT_HSL_SATURATION_GET             (0x8272)
#define OPCODE_LIGHT_HSL_SATURATION_SET             (0x8273)
#define OPCODE_LIGHT_HSL_SATURATION_SET_UNACK       (0x8274)
#define OPCODE_LIGHT_HSL_SATURATION_STATUS          (0x8275)
#define OPCODE_LIGHT_HSL_SET                        (0x8276)
#define OPCODE_LIGHT_HSL_SET_UNACK                  (0x8277)
#define OPCODE_LIGHT_HSL_STATUS                     (0x8278)
#define OPCODE_LIGHT_HSL_TARGET_GET                 (0x8279)
#define OPCODE_LIGHT_HSL_TARGET_STATUS              (0x827A)
#define OPCODE_LIGHT_HSL_DEFAULT_GET                (0x827B)
#define OPCODE_LIGHT_HSL_DEFAULT_STATUS             (0x827C)
#define OPCODE_LIGHT_HSL_RANGE_GET                  (0x827D)
#define OPCODE_LIGHT_HSL_RANGE_STATUS               (0x827E)
#define OPCODE_LIGHT_HSL_DEFAULT_SET                (0x827F)
#define OPCODE_LIGHT_HSL_DEFAULT_SET_UNACK          (0x8280)
#define OPCODE_LIGHT_HSL_RANGE_SET                  (0x8281)
#define OPCODE_LIGHT_HSL_RANGE_SET_ANACK            (0x8282)

#define CONFIGURATION_SERVER_MODEL            	(0x0000)
#define CONFIGURATION_CLIENT_MODEL            	(0x0001)
#define HEALTH_SERVER_MODEL                   	(0x0002)
#define GENERIC_ONOFF_SERVER                  	(0x1000)
#define GENERIC_LEVEL_SERVER                  	(0x1002)
#define GENERIC_LEVEL_CLIENT                  	(0x1003)
#define LIGHT_LIGHTNESS_SERVER_MODEL          	(0x1300)
#define LIGHT_LIGHTNESS_SETUP_SERVER_MODEL    	(0x1301)
#define LIGHT_CTL_SERVER_MODEL                	(0x1303)
#define LIGHT_CTL_SETUP_SERVER_MODEL          	(0x1304)
#define LIGHT_CTL_TEMPERATURE_SERVER_MODEL    	(0x1306)
#define LIGHT_HSL_SERVER_MODEL                	(0x1307)
#define LIGHT_HSL_SETUP_SERVER_MODEL          	(0x1308)
#define LIGHT_HSL_HUE_SERVER_MODEL            	(0x130A)
#define LIGHT_HSL_SATURATION_SERVER_MODEL     	(0x130B)
#define VENDOR_MODEL                          	(0x01A80000)

#define BT_MESH_USER_DATA_MAGIC				  	(0x20200710)

typedef void (*mesh_xcpu_callback)(void *p_ctx);
typedef bt_status_t (*prov_filter_func)(void *data);
typedef UINT32 (*mesh_client_filter_func)(UINT8 *uuid);

typedef struct
{
	UINT16 sub_size;
	UINT16 *sub_list;
	UINT16 *sub_default_group_addr;
    UINT16 *sub_group_addr;
    UINT8 element_num;
}SUBSCRIPTION_INFO;

typedef struct
{
	UINT8 elements_num;
	UINT16 algorithms;
	UINT8 public_key_type;
	UINT8 static_oob_type;
	UINT8 output_oob_size;
	UINT16 output_oob_action;
	UINT8 input_oob_size;
	UINT16 input_oob_action;
	UINT8 public_key_x[32];
	UINT8 public_key_y[32];
	UINT8 private_key[32];
    UINT8 random[16];
}NODE_CAPABILITIES_INFO;

typedef struct{
    UINT8 *data_buff;
    UINT16 total_length;
    UINT16 buff_index;
    UINT8  last_seg;
    UINT8  seg_index;
	UINT8 seg_handle;
}SEG_INFO;

typedef struct
{
	UINT32 link_id;
	UINT8 peer_transaction_num;
    UINT8 transaction_num;
	UINT8 provision_state;
	UINT8 node_uuid[16];
	UINT8 timeout_handle;
	UINT8 provision_timeout_handle;
	UINT8 ack_handle;
	UINT8 public_key_x[32];
	UINT8 public_key_y[32];
	UINT8 private_key[32];
	UINT8 ecdh[32];
    UINT8 dev_key[16];
    UINT8 random[16];
    UINT8 invite_pdu;
    UINT8 start_pdu[5];
    UINT8 capabilites_pdu[11];
    UINT16 net_index;
    UINT16 app_index;
	SEG_INFO seg_input;
    SEG_INFO seg_output;
	mesh_client_filter_func filter_func;
	NODE_CAPABILITIES_INFO node_info;
}MESH_CLIENT_PROVISION_INFO;

typedef struct _MESH_NET_INDEX_LIST
{
    UINT16 index;
    struct _MESH_NET_INDEX_LIST *next;
}MESH_NET_INDEX_LIST;

typedef struct _MESH_APP_INDEX_LIST
{
    UINT16 net_index;
    UINT16 app_index;
    struct _MESH_APP_INDEX_LIST *next;
}MESH_APP_INDEX_LIST;

typedef struct _MESH_NODE_CTX
{
    UINT16 dst_addr;
    MESH_NET_INDEX_LIST *net_list;
    MESH_APP_INDEX_LIST *app_list;
    UINT8 dev_key[16];
	UINT16 node_location;
	UINT16 node_label;
	UINT16 group_addr;
    struct _MESH_NODE_CTX *next;
}MESH_NODE_CTX;

typedef struct ST_MESHSEGMENT{
    UINT8 *data_buff;
    UINT16 total_length;
    UINT16 buff_index;
    UINT8  last_segment;
    UINT8  segment_index;
    struct ST_MESHSEGMENT *next;
}MESH_SEGMENT;

typedef struct {
    UINT8 public_x[32];
    UINT8 public_y[32];
    UINT8 private[32];
}P256_KEY;

typedef struct ST_MESH_APP_KEY{
    UINT16 AppIndex;
    UINT8 KeyState;
    UINT8 Key[16];
    UINT8 KeyOld[16];
    UINT8 AID;
    UINT8 AIDOld;
    struct ST_MESH_APP_KEY *next;
}MESH_APP_KEY;

typedef struct ST_MESH_NET_KEY{
    UINT16 Index;
    UINT8 NID;
    UINT8 NIDOld;
    UINT8 Key[16];
    UINT8 KeyOld[16];
    UINT8 KeyState;
    UINT8 EncryptionKeyPrivacyKey[32];
    UINT8 EncryptionKeyPrivacyKeyOld[32];
    UINT8 NetKeyID[8];
    UINT8 NetKeyIDOld[8];
    UINT8 Node_Id_State;
    MESH_APP_KEY *AppKey;
    struct ST_MESH_NET_KEY *next;
}MESH_NET_KEY;

typedef struct st_MeshFriendKey{
    UINT16 Index; // Index of NetKey
    UINT16 LPNAddr;
    UINT16 FriendAddr;
    UINT16 LPNCnt;
    UINT16 FriendCnt;
    UINT8 FNID;
    UINT8 FNIDOld;
    UINT8 FEncryptionKeyPrivacyKey[32];
    UINT8 FEncryptionKeyPrivacyKeyOld[32];
    struct st_MeshFriendKey *next;
}t_MeshFriendKey;

typedef struct
{
	UINT16 client_addr;
	UINT8 iv_index[4];
	UINT16 mesh_last_node_addr;
	UINT16 max_node_cnt;
	UINT16 node_cnt;
	UINT32 pending_cmd;
	MESH_NET_KEY **net_key;
	MESH_NODE_CTX *last_node_info;
	MESH_NODE_CTX *mesh_provisioner_node_info;
}MESH_CLIENT_INFO;

typedef struct
{
    UINT16 src;
    UINT16 dst;
    UINT32 seq;
}MESH_NETWORK_CACHE;

typedef struct
{
	UINT32 seq_num;
	UINT16 addr;
} MESH_NETWORK_SEQ;

typedef struct
{
	UINT8 ivi;
    UINT8 nid;
    UINT8 ctl;
    UINT8 ttl;
    UINT32 seq;
    UINT16 src;
    UINT16 dst;
	UINT16 virtual_addr;
	UINT8 encode_flag;
	UINT8 friend_flag;
	UINT8 mic_size;
	UINT16 netindex;
	UINT8 *tx_pdu;
	UINT16 tx_pdu_len;
	UINT8 *rx_pdu;
	UINT16 rx_pdu_len;
	UINT8 node_feature;
	SUBSCRIPTION_INFO sub;
	UINT8 rly_cnt;
	UINT8 rly_index;
	MESH_NETWORK_CACHE relay_cache[MESH_CFG_RELAY_COUNT];
	MESH_NETWORK_CACHE replay_cache[MESH_CFG_RELAY_COUNT];
	MESH_NETWORK_SEQ peer_seq[MESH_CFG_SEQ_COUNT];
} MESH_NETWORK_CTX_T;

typedef struct
{
	UINT16 src;
	UINT16 auth;
	UINT32 seq;
} MESH_REPLAY_CACHE;

typedef struct
{
	UINT8 seg;
	UINT8 akf;
	UINT8 aid;
	UINT16 seq_zero;
	UINT8 seg_o;
	UINT8 seg_n;
	UINT8 sz_mic;
	UINT8 obo;
	MESH_REPLAY_CACHE seq_auth[MESH_CFG_REPLAY_CACHE];
	UINT32 last_block;
	UINT32 block_ack;
	UINT32 ack_value;
	osi_timer_t *incomplete_timer;
	osi_timer_t *ack_timer;
	UINT8 *tx_pdu;
	UINT16 tx_pdu_len;
	UINT8 *rx_pdu;
	UINT16 rx_pdu_len;
	UINT8 *upper_pdu;
    UINT16 upper_pdu_len;
} MESH_LOWER_CTX_T;

typedef struct
{
	UINT16 appindex;
	UINT16 virtual_addr;
    UINT8 label[16];
	UINT8 *tx_pdu;
	UINT16 tx_pdu_len;
	UINT8 *rx_pdu;
	UINT16 rx_pdu_len;
	UINT8 *additional_data;
    UINT8 additional_data_len;
	mesh_xcpu_callback device_cb;
	mesh_xcpu_callback data_cb;
	mesh_xcpu_callback access_filter_cb;
} MESH_ACCESS_CTX_T;

typedef struct
{
	osi_timer_t *timer_handle;
    UINT8 ttl;
    UINT8 count_log;
    UINT8 period_log;
	UINT16 dst_addr;
    UINT16 features;
    UINT16 net_key;
	UINT32 count;
	UINT8 enable;
	
	UINT8 sub_period_log;
    UINT8 sub_min_hops;
    UINT8 sub_max_hops;
    osi_timer_t *sub_period_handle;
	UINT16 sub_dst_addr;
    UINT16 sub_src_addr;
	UINT16 sub_count;
    UINT16 sub_period;
}MESH_HEARTBEAT_CTX_T;

typedef struct
{
	UINT8 feature;
	UINT8 max_node;
	UINT8 ttl;
	UINT8 cache_size;
	UINT8 sn_beacon;//secure network beacon
	osi_timer_t *sn_timer;
	UINT8 proxy_state;
	osi_timer_t *proxy_adv_timer;
	UINT8 friend_state;
    UINT8 relay;
    UINT8 relay_cnt;
    UINT8 relay_interval;
	UINT8 transmit_cnt;
    UINT8 transmit_interval;

	//health config
	UINT8 attention;
    UINT8 divisor;
    UINT8 fault_cnt;
    osi_timer_t *timer_handle;
    UINT16 ht_src;
    UINT16 ht_dst;
	UINT32 period;
    UINT8 fault_array[MESH_CFG_HEALTH_FAULT_SIZE];
} MESH_STACK_CONFIG_CTX_T;

typedef struct
{
	UINT8 role;
	prov_filter_func prov_filter;
	UINT8 state;
    UINT32 link_id;
    UINT8 transaction_num;
    UINT8 peer_transaction_num;
    UINT8 mesh_state;
    UINT8 error_code;
    int provisioning_complete;
    UINT8 pdu_type; // control, data transaction, ack
    UINT8 cmd_type; // provisioning command
    UINT8 last_cmd;
	UINT8 last_peer_tn;
    osi_timer_t *timer_handler;
    MESH_SEGMENT segmentInput;
    MESH_SEGMENT segmentOutput;
    P256_KEY provisioner_key; // public key of provisioner
    P256_KEY device_key; // public and private key of device
    UINT8 ecdh[32];
    UINT8 InvitePDU;
    UINT8 CapabilitiesPDU[11];
    UINT8 StartPDU[5];
    UINT8 ConfirmationProvisioner[16];
    UINT8 ConfirmationDevice[16];
    UINT8 ProvisioningRandom[16];
    UINT8 DeviceRandom[16];
    UINT8 DevKey[16];
    UINT8 DevKeyAID;
    UINT8 EncryptionKeyPrivacyKey[32];
    UINT8 SessionKey[16];
    UINT8 SessionNonce[13];
    UINT16 UnicastAddr;
    UINT16 ProvisionAddr;	
    UINT16 primary_netindex;
	UINT8 provision_state;
	UINT8 node_uuid[16];
	UINT8 timeout_handle;
	UINT8 provision_timeout_handle;
	UINT8 ack_handle;
	//client prov used
	UINT8 public_key_x[32];
	UINT8 public_key_y[32];
	UINT8 private_key[32];
    UINT8 dev_key[16];
    UINT8 random[16];
	UINT8 auth[16];
    UINT8 invite_pdu;
    UINT8 start_pdu[5];
    UINT8 capabilites_pdu[11];
    UINT16 net_index;
    UINT16 app_index;
	UINT16 client_addr;
	UINT16 max_node_cnt;
	UINT16 node_cnt;
	UINT16 mesh_last_node_addr;
	MESH_NODE_CTX *node_list;
	MESH_NODE_CTX *last_node;
	SEG_INFO seg_input;
    SEG_INFO seg_output;
	mesh_client_filter_func filter_func;
	NODE_CAPABILITIES_INFO node_info;
}MESH_PROV_CTX_T;

typedef struct
{
    void (*prov_success_cb) (void);
	void (*prov_failed_cb) (UINT8 reason);
	void (*prov_closed_cb) (UINT8 reason);
	void (*user_data_save_cb) (void);
} MESH_CALLBACK_T;

typedef struct
{
    MESH_NETWORK_CTX_T net_ctx;
	MESH_LOWER_CTX_T lower_ctx;
	MESH_ACCESS_CTX_T access_ctx;
	MESH_STACK_CONFIG_CTX_T cfg_ctx;
	MESH_HEARTBEAT_CTX_T hb_ctx;
	MESH_PROV_CTX_T prov_ctx;
	MESH_CALLBACK_T *cb_ctx;
	UINT8 IVFlag;
	UINT8 IVIndex[4];
	UINT8 IVIndexOld[4];
} MESH_STACK_CTX_T;

typedef struct
{
	bt_status_t (*open)(UINT8 role);
    bt_status_t (*un_bind)(UINT16 unicast_addr);
    bt_status_t (*prov_enable)(void);
	bt_status_t (*close)(void);
	bt_status_t (*gatt_init)(void);
	bt_status_t (*reset)(void);
} MESH_INTERFACE_T;



#define BT_MESH_MODEL_DEF(ID, PUB, OP, CB, USER_DATA)	\
{														\
	.id 		= ID,									\
	.pub 		= PUB,									\
	.op 		= OP,									\
	.cb 		= CB,									\
	.user_data 	= USER_DATA								\
}														\

/*
#define BT_MESH_ELEMENT_DEF(LOC, MODELS, VENDOR_MODELS)	\
{														\
	.loc              = LOC,                 			\
	.model_count      = (sizeof(MODELS)/sizeof((MODELS)[0])),      		\
	.models           = (MODELS),                		\
	.vnd_model_count  = (sizeof(VENDOR_MODELS)/sizeof((VENDOR_MODELS)[0])),  	\
	.vnd_models       = (VENDOR_MODELS),            	\
}														\
*/

typedef struct {
	UINT16 addr;        /**< Publish Address. */
	UINT16 key:12;      /**< Publish AppKey Index. */
	UINT16 cred:1;      /**< Friendship Credentials Flag. */
	UINT16 rfu:3;

	UINT8  ttl;         /**< Publish Time to Live. */
	UINT8  period;      /**< Publish Period. */
	UINT8  cnt:3;		/**< Retransmissions count. */
	UINT8  step:5;		/**< Retransmissions Step, 50ms. */
} mesh_model_pub_t;

/** Model opcode handler. */
typedef struct {
	const UINT32  opcode;
	const int min_len;
	void (*func)(MESH_STACK_CTX_T *p_ctx);
} mesh_model_op_t;

typedef struct {
	int (*init)(void *model);
	void (*reset)(void *model);
	void (*pub)(void *);
} mesh_model_cb_t;

/** Mesh Model instance */
typedef struct {
	UINT32 id;/**< Model ID */
	/** Model Publication */
	mesh_model_pub_t *pub;
	osi_timer_t *pub_handle;
	/** AppKey List */
	UINT8 key_cnt;
	UINT16 keys[MESH_CFG_APP_KEY_COUNT];

	/** Subscription List (group or virtual addresses) */
	UINT8 sub_cnt;
	UINT16 groups[MESH_CFG_GROUP_COUNT];
	mesh_model_op_t *op;
	mesh_model_cb_t *cb;
	void *user_data;
} mesh_model_t;



/** Mesh Element */
typedef struct {
	UINT16 addr;
	UINT16 loc;
	UINT8 model_count;
	UINT8 vnd_model_count;
	mesh_model_t *models;
	mesh_model_t *vnd_models;
} mesh_element_t;

/** Node Composition */
typedef struct {
	UINT16 cid; /**< Company ID */
	UINT16 pid; /**< Product ID */
	UINT16 vid; /**< Version ID */
	UINT16 crpl;
	UINT16 feature;
	UINT8 elem_count; /**< The number of elements in this device. */
	mesh_element_t *elem; /**< List of elements. */
} mesh_comp_t;

#pragma pack(1) 

typedef struct {
    UINT8 length;
    UINT8 type;
    UINT8 beacon_type;
    UINT8 uuid[16];
    UINT8 oob[2];
}mesh_unprovision_beacon_t;

typedef struct
{
    UINT8 flags[3];
    UINT8 uuid_list[4];
    UINT8 service_data[22];
} GATT_Unprovisioned_Beacon_PDU;

#pragma pack()

typedef struct 
{
    bdaddr_t address;
    UINT8 result;
}mesh_gatt_connect_cnf_t;

typedef struct
{
    UINT16 in_uuid;
    UINT16 out_uuid;
}mesh_gatt_info;

typedef struct
{
    UINT16 net_key_index;
	UINT8 node_id_state;
    UINT8 net_key[16];
	UINT8 net_old_key[16];
}mesh_net_key_t;

typedef struct
{
    UINT16 net_key_index;
    UINT16 app_key_index;
    UINT8 app_key[16];
	UINT8 app_old_key[16];
}mesh_app_key_t;

typedef struct
{
	UINT8 dev_key[16];
    UINT16 unicast_addr;
    UINT16 primary_netindex;
    UINT8 iv_index[4];
    UINT16 provision_addr;
}mesh_prov_t;

typedef struct
{
	UINT32 id;
	UINT16 app_list[MESH_CFG_APP_KEY_COUNT];
	UINT16 group_list[MESH_CFG_GROUP_COUNT];
}mesh_model_key_t;

typedef struct
{
	UINT32 magic;
	mesh_prov_t prov;
	UINT16 net_key_cnt;
	UINT16 app_key_cnt;
	mesh_net_key_t net_key[MESH_CFG_NET_KEY_COUNT];
	mesh_app_key_t app_key[MESH_CFG_APP_KEY_COUNT];
	mesh_model_key_t model[MESH_CFG_MODEL_COUNT];
	MESH_NETWORK_SEQ seq[MESH_CFG_SEQ_COUNT];
}mesh_user_data_t;

bt_status_t bt_mesh_open(UINT8 role);
bt_status_t bt_mesh_un_bind(UINT16 unicast_addr);
bt_status_t bt_mesh_start_prov(void);
bt_status_t bt_mesh_reset(void);
bt_status_t bt_mesh_gatt_init(void);
bt_status_t bt_mesh_close(void);

extern void mesh_node_identitiy_hash(UINT8 netkey[16], UINT8 random[8], UINT16 addr, UINT8 output[8]);
MESH_STACK_CTX_T *bt_mesh_get_ctx(void);
UINT8 bt_mesh_get_element_num(void);
mesh_element_t *bt_mesh_get_element(void);
void ble_mesh_start_proxy_nid_adv(UINT16 net_index);
const mesh_comp_t *bt_mesh_get_comp(void);
UINT16 bt_mesh_get_current_acl(void);
bt_status_t bt_mesh_main(void);
void bt_mesh_start_unprovision_beacon(void);
void bt_mesh_start_secure_network_beacon(UINT8 key_refresh, UINT8 iv_update, UINT16 index);
void bt_mesh_save_user_data(void);
void mesh_revert(UINT8 *input, UINT32 len);
extern void mesh_fn_init(void);

//#define	MESH_DEBUG_INFO
//#define MESH_PTS_TEST
//#define MESH_FN_TIMECHECK

#ifdef MESH_DEBUG_INFO

#define MESH_DEBUG_NETWORK
#define MESH_DEBUG_LOWER

#endif

//#define	MESH_USE_SEGMENT


#endif

