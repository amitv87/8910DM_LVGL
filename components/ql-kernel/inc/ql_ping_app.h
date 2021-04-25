#ifndef QL_PING_APP_H
#define QL_PING_APP_H

#if 1//def CONFIG_QUEC_PROJECT_FEATURE_TCPIP

#define ping_print(msg, ...)     custom_log("PING", msg, ##__VA_ARGS__)

typedef enum{
	QL_PING_OK                      = 0,
	QL_PING_ERR_INVALID_PARAM       = (QL_COMPONENT_LWIP_SOCKET << 16) | 552	/* Invalid parameters   */,
	QL_PING_ERR_SOCKET_NEW_FAILURE  = (QL_COMPONENT_LWIP_SOCKET << 16) | 554	/* Create socket failed */,
	QL_PING_ERR_SOCKET_BIND_FAILURE = (QL_COMPONENT_LWIP_SOCKET << 16) | 556	/* Socket bind failed   */,
	QL_PING_ERR_DNS_FAILURE         = (QL_COMPONENT_LWIP_SOCKET << 16) | 565	/* DNS parse failed     */,
	QL_PING_ERR_TIMEOUT             = (QL_COMPONENT_LWIP_SOCKET << 16) | 569	/* Operation timeout    */,
}ql_ping_result_code;

typedef struct
{
  uint32_t  num_data_bytes;          /* Data byte size of the ping packet    */
  uint32_t  ping_response_time_out;  /* Wait time for ping response, ms      */
  uint32_t  num_pings;               /* Number of times to ping              */
  uint32_t  ttl;                     /* Time To Live for the ping packet     */
} ql_ping_config_type;

typedef enum{
	QL_PING_STATS    = 1,
	QL_PING_SUMMARY  = 2,
}ql_ping_event_type;

typedef struct{
  uint32_t     ping_rtt;
  uint32_t     ping_size;
  uint32_t     ping_ttl;
  char         resolved_ip_addr[256];
}ql_ping_stats_type;

typedef struct{
   uint32_t  min_rtt;        /* Minimum RTT so far, in millisecs           */
   uint32_t  max_rtt;        /* Maximum RTT so far, in millisecs           */
   uint32_t  avg_rtt;        /* Average RTT so far, in millisecs           */
   uint32_t  num_pkts_sent;  /* Number of pings sent so far          */
   uint32_t  num_pkts_recvd; /* Number of responses recieved so far  */
   uint32_t  num_pkts_lost;  /* Number of responses not received     */
}ql_ping_summary_type;

typedef int ping_session_id;

typedef void(*ql_ping_event_cb)(ping_session_id ping_sess_id, uint16_t event_id, int evt_code, void *evt_param);

/*****************************************************************
* Function: ql_ping_start
*
* Description: 启用ping功能,根据配置参数设置发送的ping包
* 
* Parameters:
*	cid           [in]    pdp cid信息.
*   sim_id        [in]    sim id信息
*   host          [in]    ping操作的对端地址
*   ping_options  [in]    ping请求发送数据设置,请求字节数,超时时间,最大请求次数
*   cb_fcn        [in]    ping操作的事件回调函数,设置事件类型为ping start,以及对ping回调结果的判断处理
*
* Return:
* 	ping事件会话id
*
*****************************************************************/
ping_session_id ql_ping_start(int cid, int sim_id, const char *host, ql_ping_config_type *ping_options , ql_ping_event_cb cb_fcn);

/*****************************************************************
* Function: ql_ping_stop
*
* Description: 返回固定结果值1
* 
* Parameters:
*	ping_sess_id  [in]    ping事件会话id.
*
* Return:
* 	1          返回固定结果1
*
*****************************************************************/
int ql_ping_stop(ping_session_id      ping_sess_id);

#endif
#endif
