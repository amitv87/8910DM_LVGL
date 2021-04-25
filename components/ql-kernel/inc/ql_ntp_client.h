#ifndef QL_NTP_CLIENT_H
#define QL_NTP_CLIENT_H


#define ntp_print(msg, ...)     custom_log("NTP", msg, ##__VA_ARGS__)

typedef int  ntp_client_id;

typedef struct{
	int    pdp_cid;
	int    sim_id;
	int    retry_cnt;
	int    retry_interval_tm;
}ql_ntp_sync_option;

typedef enum{
	QL_NTP_SUCCESS                    = 0,
	QL_NTP_ERROR_UNKNOWN              = -1,
	QL_NTP_ERROR_WODBLOCK             = -2,
	QL_NTP_ERROR_INVALID_PARAM        = -3,
	QL_NTP_ERROR_OUT_OF_MEM           = -4,
	QL_NTP_ERROR_TIMEOUT              = -5,
	QL_NTP_ERROR_DNS_FAIL             = -6,
	QL_NTP_ERROR_SOCKET_ALLOC_FAIL    = -7,
	QL_NTP_ERROR_SOCKET_SEND_FAIL     = -8,
	QL_NTP_ERROR_SOCKET_RECV_FAIL     = -9,
	QL_NTP_ERROR_INVALID_REPLY        = -10
}ql_ntp_error_code_e;

typedef void(*ql_ntp_sync_result_cb)(ntp_client_id        cli_id, int result, struct tm *sync_time, void *arg);

/*****************************************************************
* Function: ql_ntp_sync
*
* Description: ntp时间同步函数,启用ntp服务功能
* 
* Parameters:
*	host          [in]    ntp服务器IP地址或域名地址,以及端口号.
*   user_option   [in]    记录pdp相关信息,以及是否同步ntp时间的配置信息
*   cb            [in]    ntp事件回调函数,设置事件类型为ntp,以及对回调结果的判断
*   arg           [in]    自定义传参
*   error_code    [out]   返回ntp服务过程中错误代码
*
* Return:
* 	记录ntp服务信息的上下文id号
*
*****************************************************************/
ntp_client_id ql_ntp_sync(const char* host, ql_ntp_sync_option *user_option,ql_ntp_sync_result_cb cb, void *arg, int *error_code);

#endif
