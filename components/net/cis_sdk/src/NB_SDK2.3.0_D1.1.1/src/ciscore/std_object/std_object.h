#ifndef STD_OBJECT_H_
#define STD_OBJECT_H_

#include "../cis_api.h"
#include "../cis_internals.h"
extern int g_reboot;


/*
 * Standard Object IDs
 */
#define CIS_SECURITY_OBJECT_ID          (0)
#if CIS_ENABLE_UPDATE || CIS_ENABLE_MONITER
#define CIS_CONNECTIVITY_OBJECT_ID      (4)
#endif
#if CIS_ENABLE_UPDATE
#define CIS_DEVICE_OBJECT_ID            (3)
#define CIS_FIRMWARE_OBJECT_ID          (5)
#endif

#if CIS_ENABLE_MONITER
#define CIS_MONITOR_OBJECT_ID           (10290)
#endif

#if  CIS_ENABLE_CMIOT_OTA
#define CIS_POWERUPLOG_OBJECT_ID        (3351)
#define CIS_CMDHDEFECVALUES_OBJECT_ID   (2051)
#endif

#if CIS_ENABLE_DM
#define CIS_DM_OBJECT_ID                (666)
#endif

typedef enum et_std_objectid
{
	std_object_security = CIS_SECURITY_OBJECT_ID,
#if CIS_ENABLE_UPDATE
	std_object_device = CIS_DEVICE_OBJECT_ID,
	std_object_conn = CIS_CONNECTIVITY_OBJECT_ID,
	std_object_firmware = CIS_FIRMWARE_OBJECT_ID,
#endif
#if CIS_ENABLE_MONITER
	std_object_monitor = CIS_MONITOR_OBJECT_ID,
#if !CIS_ENABLE_UPDATE
	std_object_conn = CIS_CONNECTIVITY_OBJECT_ID,
#endif
#endif
#if	CIS_ENABLE_CMIOT_OTA
	std_object_poweruplog = CIS_POWERUPLOG_OBJECT_ID,
	std_object_cmdhdefecvalues = CIS_CMDHDEFECVALUES_OBJECT_ID,
#endif
#if CIS_ENABLE_DM
    std_object_dm = CIS_DM_OBJECT_ID,
#endif
}cis_std_objectid;

/*
 * Resource IDs for the LWM2M Security Object
 */
#define RES_SECURITY_URI_ID                 0
#define RES_SECURITY_BOOTSTRAP_ID           1
#define RES_SECURITY_SECURITY_ID            2
#define RES_SECURITY_IDENTIFY               3
#define RES_SECURITY_PUBLIC_KEY             4
#define RES_SECURITY_SECRET                 5
#define RES_SECURITY_SHORT_SERVER_ID        10
#define RES_SECURITY_HOLD_OFF_ID            11


// Resource Id's:
#define RES_M_PACKAGE                   0
#define RES_M_PACKAGE_URI               1
#define RES_M_UPDATE                    2
#define RES_M_STATE                     3
#define RES_M_UPDATE_RESULT             5
#define RES_M_SUPPORT_PORTOCOL          8
#define RES_M_DELIVER_METHOD            9
#define RES_M_TRANSGERRED_BYTES         26500
#define RES_M_SWITCH_TO_DOWNLOAD        26501
#define RES_M_FOTA_TRIGGER              26600
#if CIS_ENABLE_UPDATE_MCU
#define RES_M_SWITCH_MCU_UPDATE         26550
#endif


/*
* resources:
* 3 firmwareVersion                   

* 9 batteryLevel                      read
* 10 freeMomery                       read
*/

// Resource Id's for Device:

#define RES_O_FIRMWARE_VERSION      3
#define RES_O_BATTERY_LEVEL         9
#define RES_O_MEMORY_FREE           10
#define RES_O_BATTERY_VOLTAGE       26260
#if CIS_ENABLE_UPDATE_MCU
#define RES_O_SOFTWARE_VERSION      19
#define RES_O_MEMORY_FREE_MCU		26261
#endif

/*
* Resource IDs for the LWM2M Connectivity Moniter Object
*/
#define RES_CONN_RADIO_SIGNAL_STRENGTH_ID			2
#define RES_CONN_CELL_ID							8
//#define RES_CONN_RSRP                               6035
#define RES_CONN_SINR                               6038

/*
* Resource IDs for the Extend Moniter Object
*/
#define RES_MONITER_NETWORK_BEARER                  0
#define RES_MONITER_LINK_UTILIZATION                6
#define RES_MONITER_CSQ                             30
#define RES_MONITER_ECL                             31    
#define RES_MONITER_SNR                             32
#define RES_MONITER_PCI                             6034
#define RES_MONITER_ECGI                            20626
#define RES_MONITER_EARFCN                          6032
#define RES_MONITER_PCI_FIRST                       20628
#define RES_MONITER_PCI_SECOND                      20629
#define RES_MONITER_RSRP_FIRST                      20630
#define RES_MONITER_RSRP_SECOND                     20631

//some additional resource IDs
#define RES_MONITER_PLMN							6030
#define RES_MONITER_LATITUDE						5513
#define RES_MONITER_LONGITUDE						5514
#define RES_MONITER_ALTITUDE						5515

#if  CIS_ENABLE_CMIOT_OTA
/*
* Resource IDs for the CMIOT OTA
*/

#define RES_M_DEVICENAME					0
#define RES_M_TOOLVERSION					1
#define RES_M_IMEI							2
#define RES_M_IMSI							3
#define RES_M_MSISDN						4



#define RES_M_ORDER							0
#define RES_M_DEFECVALUE					1
#define RES_M_REQUESTORIGIN					2
#define RES_M_REQUESTCONTEXT				3
#define RES_M_REQUESTCONTEXTNOTIFICATION	4
#define RES_M_REQUESTCHARACTERISTICS		5

#endif //CIS_ENABLE_CMIOT_OTA




typedef cis_coapret_t (*std_object_read_callback)(st_context_t * contextP,cis_iid_t instanceId,int * numDataP, st_data_t ** dataArrayP,st_object_t * objectP);
typedef cis_coapret_t (*std_object_exec_callback)(st_context_t * contextP,cis_iid_t instanceId,uint16_t resourceId,uint8_t * buffer,int length,st_object_t * objectP);
typedef cis_coapret_t (*std_object_write_callback)(st_context_t * contextP,cis_iid_t instanceId,int numData,st_data_t * dataArray,st_object_t * objectP);
typedef cis_coapret_t (*std_object_discover_callback)(st_context_t * contextP,cis_iid_t instanceId,int * numDataP, st_data_t ** dataArrayP,st_object_t * objectP);


struct st_std_object_callback_mapping
{
    cis_oid_t           stdObjectId;
    std_object_read_callback onRead;
    std_object_write_callback onWrite;
    std_object_exec_callback onExec;
	std_object_discover_callback onDiscover;
};


/*
 * object.c
 */

bool          std_object_isStdObject(cis_oid_t oid);

cis_coapret_t std_object_read_handler(st_context_t * contextP,cis_iid_t instanceId,int * numDataP, st_data_t ** dataArrayP,st_object_t * objectP);
cis_coapret_t std_object_exec_handler(st_context_t * contextP,cis_iid_t instanceId,uint16_t resourceId,uint8_t * buffer,int length,st_object_t * objectP);
cis_coapret_t std_object_write_handler(st_context_t * contextP,cis_iid_t instanceId,int numData,st_data_t * dataArray,st_object_t * objectP);
cis_coapret_t std_object_discover_handler(st_context_t * contextP,cis_iid_t instanceId,int * numDataP, st_data_t ** dataArrayP,st_object_t * objectP);

cis_coapret_t   std_object_writeInstance(st_context_t * contextP,st_uri_t * uriP, st_data_t * dataP);
cis_list_t*     std_object_get_securitys(st_context_t * contextP);
cis_list_t*     std_object_put_securitys(st_context_t * contextP,cis_list_t* targetP);
void            std_object_remove_securitys(st_context_t * contextP,cis_list_t* targetP);

#if CIS_ENABLE_UPDATE || CIS_ENABLE_MONITER
cis_list_t* std_object_get_conn(st_context_t * contextP, cis_iid_t instanceId);
cis_list_t* std_object_put_conn(st_context_t * contextP, cis_list_t* targetP);
void std_object_remove_conn(st_context_t * contextP, cis_list_t* targetP);
#endif

#if CIS_ENABLE_DM
cis_list_t* std_object_get_dm(st_context_t * contextP,cis_iid_t instanceId);
cis_list_t* std_object_put_dm(st_context_t * contextP,cis_list_t* targetP);
void std_object_remove_dm(st_context_t * contextP,cis_list_t* targetP);
#endif

#if CIS_ENABLE_UPDATE
cis_list_t* std_object_get_device(st_context_t * contextP,cis_iid_t instanceId);
cis_list_t* std_object_put_device(st_context_t * contextP,cis_list_t* targetP);
void std_object_remove_device(st_context_t * contextP,cis_list_t* targetP);
cis_list_t* std_object_get_firmware(st_context_t * contextP,cis_iid_t instanceId);
cis_list_t* std_object_put_firmware(st_context_t * contextP,cis_list_t* targetP);
void std_object_remove_firmware(st_context_t * contextP,cis_list_t* targetP);
#endif //CIS_ENABLE_UPDATE

#if CIS_ENABLE_CMIOT_OTA
cis_list_t* std_object_get_poweruplog(st_context_t * contextP,cis_iid_t instanceId);
cis_list_t* std_object_put_poweruplog(st_context_t * contextP,cis_list_t* targetP);
void std_object_remove_poweruplog(st_context_t * contextP,cis_list_t* targetP);

cis_list_t* std_object_get_cmdhdefecvalues(st_context_t * contextP,cis_iid_t instanceId);
cis_list_t* std_object_put_cmdhdefecvalues(st_context_t * contextP,cis_list_t* targetP);
void std_object_remove_cmdhdefecvalues(st_context_t * contextP,cis_list_t* targetP);
#endif

#if CIS_ENABLE_MONITER
cis_list_t* std_object_get_moniter(st_context_t * contextP, cis_iid_t instanceId);
cis_list_t* std_object_put_moniter(st_context_t * contextP, cis_list_t* targetP);
void std_object_remove_moniter(st_context_t * contextP, cis_list_t* targetP);
#endif //CIS_ENABLE_MONITER


/*
 * object_security.c
 */
bool            std_security_create(st_context_t * contextP, int instanceId, const char* serverHost, bool isBootstrap, st_object_t * securityObj);
void            std_security_clean(st_context_t * contextP);
char *          std_security_get_host(st_context_t * contextP,cis_iid_t instanceId);


/*
 * object_firmware.c
 */
#if CIS_ENABLE_UPDATE

#define PRV_FIRMWARE_URI  "COAP://NULL"
#define PRV_FIRMWARE_URI_LEN 32

typedef struct _firmware_data_
{
	struct _firmware_data_ * next;        // matches st_list_t::next
	cis_listid_t                 instanceId;  // matches st_list_t::id

	bool supported;
	cis_fw_context_t * fw_info;
	uint8_t delivery;
	char download_uri[PRV_FIRMWARE_URI_LEN];

} firmware_data_t;

bool            std_firmware_create(st_context_t * contextP,int instanceId,st_object_t * firmwareObj);
void            std_firmware_clean(st_context_t * contextP);


/*
 * object_device.c
 */
bool            std_device_create(st_context_t * contextP,int instanceId,st_object_t * deviceObj);
void            std_device_clean(st_context_t * contextP);

#endif

#if CIS_ENABLE_MONITER || CIS_ENABLE_UPDATE
/*
* object_conn_moniter.c
*/
bool            std_conn_moniter_create(st_context_t * contextP, int instanceId, st_object_t * connObj);
void            std_conn_moniter_clean(st_context_t * contextP);
#endif

#if	CIS_ENABLE_CMIOT_OTA
/*
 *	cis_object_powerup_log.c
 */
typedef struct _poweruplog_data_
{
	struct _poweruplog_data_ * next;		 // matches st_list_t::next
	cis_listid_t				 instanceId;  // matches st_list_t::id
	bool supported;
	char *				deviceName;    // matches st_list_t::id
	char *				toolVersion;
	char *				IMEI;
	char *				IMSI;
	char *				MSISDN;
	} poweruplog_data_t;


bool			std_poweruplog_create(st_context_t * contextP,int instanceId,st_object_t * poweruplogObj);
void			std_poweruplog_clean(st_context_t * contextP);

bool            std_cmdhdefecvalues_create(st_context_t * contextP,int instanceId,st_object_t * cmdhdefecvaluesObj);
void            std_cmdhdefecvalues_clean(st_context_t * contextP);

uint8_t std_poweruplog_ota_notify(st_context_t * contextP,cis_oid_t * objectid,uint16_t mid);


#endif //CIS_ENABLE_CMIOT_OTA

#if CIS_ENABLE_MONITER
/*
* object_extend_moniter.c
*/
bool            std_moniter_create(st_context_t * contextP, int instanceId, st_object_t * moniterObj);
void            std_moniter_clean(st_context_t * contextP);
#endif //CIS_ENABLE_MONITER

#if CIS_ENABLE_DM
bool            std_dm_create(st_context_t * contextP,int instanceId,st_object_t * dmObj);
void            std_dm_clean(st_context_t * contextP);
#endif

#endif /* STD_OBJECT_H_ */
