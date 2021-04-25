#ifndef _LWM2M_API_H
#define _LWM2M_API_H
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include "liblwm2m_config.h"

//zhangyi del for porting 20180709
//#include "netutils.h"

#define MAX_REF_NUM 0x01

#define LWM2M_EVENT_BASE 0
#define LWM2M_EVENT_BOOTSTRAP_START LWM2M_EVENT_BASE + 1
#define LWM2M_EVENT_BOOTSTRAP_SUCCESS LWM2M_EVENT_BASE + 2
#define LWM2M_EVENT_BOOTSTRAP_FAILED LWM2M_EVENT_BASE + 3
#define LWM2M_EVENT_CONNECT_SUCCESS LWM2M_EVENT_BASE + 4
#define LWM2M_EVENT_CONNECT_FAILED LWM2M_EVENT_BASE + 5

#define LWM2M_EVENT_REG_SUCCESS LWM2M_EVENT_BASE + 6
#define LWM2M_EVENT_REG_FAILED LWM2M_EVENT_BASE + 7
#define LWM2M_EVENT_REG_TIMEOUT LWM2M_EVENT_BASE + 8

#define LWM2M_EVENT_LIFETIME_TIMEOUT LWM2M_EVENT_BASE + 9
#define LWM2M_EVENT_STATUS_HALT LWM2M_EVENT_BASE + 10
#define LWM2M_EVENT_UPDATE_SUCCESS LWM2M_EVENT_BASE + 11
#define LWM2M_EVENT_UPDATE_FAILED LWM2M_EVENT_BASE + 12
#define LWM2M_EVENT_UPDATE_TIMEOUT LWM2M_EVENT_BASE + 13

#define LWM2M_EVENT_RESPONSE_FAILED LWM2M_EVENT_BASE + 20
#define LWM2M_EVENT_NOTIFY_FAILED LWM2M_EVENT_BASE + 21

#define LWM2M_CASE(nEvId) #nEvId

typedef enum
{
    lwm2m_string = 1,
    lwm2m_opaque = 2,
    lwm2m_integer = 3,
    lwm2m_float = 4,
    lwm2m_bool = 5,
} lwm2m_tpye_t;

typedef struct
{
    uint16_t objid;
    uint16_t insid;
    uint16_t resid;
    uint16_t count;
} read_param_t;

typedef struct _write_param_t
{
    uint16_t objid;
    uint16_t insid;
    uint16_t resid;
    lwm2m_tpye_t type;
    uint16_t len;
    uint8_t *data;
    struct _write_param_t *next;
} write_param_t;

typedef struct
{
    uint16_t objid;
    uint16_t insid;
    uint16_t resid;
    int length;
    uint8_t *buffer;
} exec_param_t;

typedef struct
{
    uint16_t objid;
    uint16_t insid;
    uint16_t resid;
    uint8_t *param;
} setParam_t;

typedef enum
{
    EVETN_IND,
    OBJ_READ_IND,
    OBJ_WRITE_IND,
    OBJ_EXE_IND,
    OBJ_OBSERVE_IND,
    OBJ_DISCOVER_IND,
    OBJ_SETPARAM_IND,
    OBJ_OPERATE_RSP_IND,
    SERVER_REGCMD_RESULT_IND,
    SERVER_QUIT_IND,
    FOTA_DOWNLOADING_IND,
    FOTA_DOWNLOAD_FAILED_IND,
    FOTA_DOWNLOAD_SUCCESS_IND,
    FOTA_PACKAGE_CHECK_IND,
    FOTA_UPGRADE_OK_IND
} LWM2M_EVENT_IND_T;

typedef enum
{
    LWM2M_RET_ERROR = -1,
    LWM2M_RET_OK,
} lwm2m_ret_t;

#define MAX_INIT_OBJECTS 10

#if defined(CONFIG_LWM2M_REDUCE_MEMORY)
#define MAX_DYNAMIC_OBJECTS 5
#define MAX_RESOURCE 10
#else
#define MAX_DYNAMIC_OBJECTS 20
#define MAX_RESOURCE 50
#endif

typedef enum
{
    TYPE_RW,
    TYPE_R,
    TYPE_W,
    TYPE_EXE,
} operate_type_t;

typedef struct
{
    uint16_t resId;
    operate_type_t operate_type;
    lwm2m_tpye_t value_type;
} ipso_res_t;

typedef struct
{
    uint16_t objId;
    bool isMultInst;
    uint8_t resCount;
    bool isResReady;
    bool isDiscover;
    ipso_res_t resArray[MAX_RESOURCE];
} ipso_obj_t;

typedef struct
{
    uint16_t objId;
    uint16_t resCount;
    uint8_t *bitmap;
    uint32_t instCount;
} obj_arry_t;

typedef struct
{
    uint8_t version;
    uint16_t mtu;
    uint8_t debugmode;
    uint8_t extendinfomode;
    uint8_t outputmode;
    uint8_t debug_level;
    uint8_t cmdline[256];
    uint8_t **argv;
    uint8_t argc;
    uint8_t isregisted;
    int8_t ipc_socket;
    int8_t ipc_data_socket;
    uint32_t lifetime;
    uint32_t timeout;
    uint32_t this;
    uint32_t event_handle;
    osiCallback_t callback;
    void *userparam;
    obj_arry_t objects[MAX_INIT_OBJECTS];
    ipso_obj_t obj_type_table[MAX_DYNAMIC_OBJECTS];
    bool pending_regcmd;
    bool use_dynamic_ipso;
    uint8_t nDLCI;
    bool observer;
    int isquit;
} lwm2m_config_t;

typedef struct
{
    uint8_t ver;
    uint8_t cc;
    uint16_t size; // Config Size of Byte
    uint8_t cfgid;
    uint8_t cfgidsize;
    uint32_t lifetime;
    uint16_t mtu;
    uint8_t *apn;
    uint16_t apnlen;
    uint8_t *usname;
    uint16_t usnamelen;
    uint8_t *psw;
    uint16_t pswlen;
    uint8_t linktype;
    uint8_t bandtype;
    uint8_t *host;
    uint16_t hostlen;
    uint8_t *net_exdata;
    uint16_t net_exdatalen;
    uint8_t debugmode;
    uint8_t extendinfomode;
    uint8_t outputmode;
    uint8_t debug_level;
    uint16_t buffersize;
    uint8_t *sys_exdata;
    uint16_t sys_exdata_len;
    uint16_t objects_count;
} mipl_config_t;

#define MIPL_HEADER_LENGTH 3
#define MIPL_CONFIG_ITEM_LENGTH 3
#define MIPL_CONFIG_INIT_INFO_LENGTH 4
#define MIPL_CONFIG_NO_EXT_DATA 0x00
#define MIPL_CONFIG_HAVE_EXT_DATA 0x01
#define MIPL_HEADER_VERSION_POSITION 5
#define MIPL_HEADER_CONFIG_COUNT_MASK 0x1F
#define MIPL_CONFIG_ID_POSITION 1
#define MIPL_CONFIG_SIZE_EXTEND_FLAG_MASK 0x01
#define MIPL_DEBUG_MODE_POSITION 7
#define MIPL_EXTEND_INFO_MODE_POSITION 6
#define MIPL_EXTEND_INFO_MODE_MASK 0X01
#define MIPL_OUTPUT_MODE_POSITION 4
#define MIPL_OUTPUT_MODE_MASK 0X03
#define MIPL_DEBUG_LEVEL_MASK 0X0F
#define MIPL_LIFETIME_MIN_VALUE 10

typedef enum enum_lwm2m_cfgid
{
    MIPL_CFGID_INIT = 0x01,
    MIPL_CFGID_NET = 0x02,
    MIPL_CFGID_SYS = 0x03,
    MIPL_CFGID_OBJECTS = 0x04,
} lwm2m_cfgid_t;

extern lwm2m_config_t *lwm2m_configs[];

uint8_t *lwm2m_eventName(uint8_t nEvId);

int lwm2m_is_pending_regcmd(uint8_t ref);

void lwm2mPostEvent(uint8_t ref, uint32_t nEventId, uint32_t nResult, uint32_t nParam1, uint32_t nParam2);

int8_t lwm2m_new_config_xml(void *file_config, uint16_t size);

int8_t lwm2m_new_config(const uint8_t *cmdline);

lwm2m_ret_t lwm2m_free_config(uint8_t ref);

lwm2m_ret_t lwm2mcfg_set_handler(uint8_t ref, void *taskHandle, osiCallback_t callback, void *userparam);

lwm2m_ret_t lwm2mcfg_set_handler_ext(uint8_t ref, void *taskHandle, osiCallback_t callback, void *userparam, uint8_t nDLCI);

lwm2m_config_t *lwm2m_get_config(uint8_t configIndex);

uint8_t lwm2m_get_ipcSocket(uint8_t configIndex);

uint8_t lwm2m_get_ipcDataSocket(uint8_t configIndex);

void lwm2m_parse_buffer(char *buffer, void *value, uint8_t count, void *delim);

lwm2m_ret_t lwm2m_excute_cmd(uint8_t *data, uint32_t data_len, uint8_t ref);

lwm2m_ret_t lwm2m_excute_data_cmd(uint8_t *data, uint32_t data_len, uint8_t ref);

lwm2m_ret_t lwm2m_is_registed(uint8_t ref);

obj_arry_t *lwm2m_get_init_objs(uint8_t ref);

lwm2m_ret_t lwm2m_add_obj(uint16_t objId, uint32_t instCount, uint8_t *bitmap, uint16_t resCount, uint8_t ref);

lwm2m_ret_t lwm2m_del_obj(uint16_t objId, uint8_t ref);

lwm2m_ret_t lwm2m_read_rsp(uint16_t objId, uint16_t instId, uint16_t resId, lwm2m_tpye_t type,
                           const uint8_t *value, uint32_t value_length, bool isEnd, uint8_t result, uint8_t ref);

lwm2m_ret_t lwm2m_write_rsp(bool result, uint8_t ref);

lwm2m_ret_t lwm2m_exec_rsp(bool result, uint8_t ref);

lwm2m_ret_t lwm2m_discover_rsp(uint8_t result, const uint8_t *value, uint8_t ref);

lwm2m_ret_t lwm2m_notify(uint16_t objId, uint16_t instId, uint16_t resId, const uint8_t *value,
                         uint32_t value_length, lwm2m_tpye_t type, uint8_t ref);

lwm2m_ret_t lwm2m_update(uint32_t lifetime, bool withObject, uint8_t ref);

lwm2m_ret_t lwm2m_set_parameter(uint16_t objId, uint16_t instId, uint16_t resId,
                                uint8_t *parameter, uint8_t ref);

lwm2m_ret_t lwm2m_register(int ref, uint32_t lifetime, uint32_t timeout);

lwm2m_ret_t lwm2m_unregister(int ref);

bool lwm2m_is_dynamic_ipso(int ref);

lwm2m_ret_t lwm2m_set_dynamic_ipso(int ref);

ipso_obj_t *lwm2m_get_ipso_obj(uint16_t objId, int16_t resId, ipso_res_t **ipso_res, uint8_t ref);

uint8_t lwm2m_isObjSupport(uint16_t objId, uint8_t ref);

bool lwm2m_isResReady(uint16_t objId, uint8_t ref);

void lwm2m_registNewObj(uint16_t objId, uint16_t resCount, uint8_t ref);

int lwm2m_registNewRes(uint16_t objId, uint8_t *resString, uint8_t ref);

lwm2m_ret_t lwm2m_set_send_flag(int ref, int flag);

lwm2m_ret_t lwm2m_get_observer(uint8_t ref);

lwm2m_ret_t lwm2m_set_observer(uint8_t ref, bool observer);

lwm2m_ret_t lwm2m_free_task(uint8_t ref);

lwm2m_ret_t lwm2m_start_fota_download(const uint8_t *uri, uint8_t ref);
lwm2m_ret_t lwm2m_notify_fota_state(uint32_t state, uint32_t resulte, uint8_t ref);
#endif
