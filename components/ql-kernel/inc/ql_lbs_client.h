#ifndef QL_LBS_CLIENT_H
#define QL_LBS_CLIENT_H

#define lbs_print(msg, ...)     custom_log("LBS_CLI", msg, ##__VA_ARGS__)

#define LBS_MAX_CELL_NUM     6
#define LBS_MAX_POS_NUM      6
#define LBS_MAX_WIFI_NUM     6

#define LBS_CELL_LOC_METHOD  4
#define LBS_WIFI_LOC_METHOD  6

typedef struct{
   uint8_t     type;// 类型 01表示定位�?02表示上报
   uint8_t     encrypt;//加密方案�?01表示异或
   uint8_t     key_index;// 01的加密方案中该参数表�?"加扰KEY编号",本地随机生成，范围在0-7
   uint8_t     pos_format;//回应数据包类�?01 不需要地址信息, 02 需要地址信息
   uint8_t     loc_method;//01-普通基站轮�?,02-多基站分别轮询（废弃�?, 03-服务端三角定位，04-第三方接�?/模块端三角定位，05-cell+wifi定位�?06-仅wifi定位�?
}lbs_basic_info_t;

typedef struct{
    char      user_name[64];//用户�?
    char      user_pwd[64];//密码
    char      token[128];//定位访问凭证
    char      imei[64];//国际移动设备识别�?
    uint16_t  rand;//随机�?
}lbs_auth_info_t;

typedef struct{
    uint8_t         radio;//发射类型
    uint16_t        mcc;//移动国家号码
    uint16_t        mnc;// 移动网络号码
    int             lac_id;//区域�?
    int             cell_id;//基站�?
    int16_t         signal;//信号强度
    uint16_t        tac;//跟踪区编�?
    uint16_t        bcch;//BCCH信道的绝对无线频道编�?
    uint8_t         bsic;//基站识别�?/基站色码,GSM�?
    uint16_t        uarfcndl;//下行频率,WCDMA �?
    uint16_t        psc;//主扰�?,WCDMA�?
    int16_t         rsrq;//参考信号接收质�?,LTE�?
    uint16_t        pci;//物理小区标识,LTE�?
    uint16_t        earfcn;//绝对频点�?,LTE�?
    uint16_t        reserve;//保留
}lbs_cell_info_t;

typedef struct{
	char            wifi_mac[18];
	int             wifi_rssi;
	char            wifi_ssid[32];
}lbs_wifi_mac_info_t;

typedef struct{
	int                  pdp_cid;
	int                  sim_id;
	int                  req_timeout;
	lbs_basic_info_t    *basic_info;
	lbs_auth_info_t     *auth_info;
	int                  cell_num;
	lbs_cell_info_t     *cell_info;
	int                  wifi_num;
	lbs_wifi_mac_info_t *wifi_info;
}lbs_option_t;


typedef struct{
    float             longitude;//经度
    float             latitude;//纬度
    uint16_t          accuracy;//精度
    uint8_t           flag;//0 - 正常基站     1 �? 无效基站
}lbs_postion_info_t;

typedef enum{
    LBS_RES_OK = 0,
    LBS_RES_LOC_FAIL = 10000,                  /*定位失败*/
    LBS_RES_IMEI_ILLEGAL = 10001,              /*非法imei�?*/
    LBS_RES_TOKEN_NOT_EXIST = 10002,           /*token不存�?*/
    LBS_RES_TOKEN_LOC_EXCEED_MAX = 10003,      /*token的定位次数超过最大�?*/
    LBS_RES_IMEI_LOC_EXCEED_DAY_MAX = 10004,   /*设备的每天定位次数超过最大�?*/
    LBS_RES_IMEI_LOC_VISIT_EXCEED_MAX = 1005,  /*token连接设备数超过最大�?*/
    LBS_RES_TOKEN_EXPIRED = 10006,              /*token过期*/
    LBS_RES_IMEI_NO_AUTHORITY = 10007,          /*该imei号设备不可访问服�?.*/
    LBS_RES_TOKEN_LOC_VISIT_EXCEED_MAX = 10008, /*token每天定位次数超过最大�?.*/
    LBS_RES_TOKEN_LOC_EXCEED_PERIOD_MAX = 10009,/*token周期内定位次数超过最大�?*/
    LBS_RES_DNS_FAIL = 10101,
    LBS_RES_MD5_FAIL = 10102,
    LBS_RES_MEMORY_FAIL = 10103,
    LBS_RES_NET_FAIL = 10104,
    LBS_RES_PARAM_FORMAT_FAIL = 10105,
}lbs_result_code_e;

typedef int lbs_client_hndl;

/*****************************************************************
* Function: ql_lbs_response_callback
*
* Description:
* 	LBS的定位结果，通过该函数告知应用程序�?
* 
* Parameters:
* 	hndl	     [in] 	LBS客户端句柄。由ql_lbs_get_position获取 
*   result       [in]   定位结果，对应lbs_result_code_e 
*	pos_num		 [in] 	LBS定位成功后，得到位置信息的个�?
*   pos_info     [in]   LBS得到的位置信�?
*   date         [in]   服务器返回的时间
*
* Return: void
*
*****************************************************************/
typedef void(*ql_lbs_response_callback)(lbs_client_hndl hndl, int result, int pos_num, lbs_postion_info_t  *pos_info, char *date);

/*****************************************************************
* Function: ql_lbs_get_position
*
* Description:
* 	基于基站信息/wifi信息，查询设备所在的位置信息�?
* 
* Parameters:
* 	host	     [in] 	服务器地址 
*	user_opts    [in] 	LBS请求的参�? 
*   cb           [in]   LBS请求结果的回调函�?
*   err_code     [out]  函数调用的结果码,参考lbs_result_code_e
*
* Return:lbs_client_hndl
*
*****************************************************************/
lbs_client_hndl ql_lbs_get_position(char *host, lbs_option_t *user_opts, ql_lbs_response_callback cb, void *arg, int *err_code);


#endif


