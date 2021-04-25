#ifndef QL_CTSREG_H
#define QL_CTSREG_H

#define ctsreg_print(msg, ...)     custom_log("CTSREG", msg, ##__VA_ARGS__)

typedef  long ql_ctsreg_hndl;

typedef enum{
    QL_CTSREG_RESPONSE_ERR   = -19,
    QL_CTSREG_INVALID_PRM    = -10,
    QL_CTSREG_OUT_OF_MEM     = -9,
    QL_CTSREG_WOULD_BLOCK    = -2,
    QL_CTSREG_REG_PDP_S      = 0,
    QL_CTSREG_REG_PDP_OK,
    QL_CTSREG_REG_PDP_NOK,
    QL_CTSREG_DNS_PARSE_S,
    QL_CTSREG_DNS_PARSE_OK,
    QL_CTSREG_DNS_PARSE_NOK,
    QL_CTSREG_SEND_DATA_S,
    QL_CTSREG_SEND_DATA_OK,
    QL_CTSREG_SEND_DATA_NOK,
    QL_CTSREG_REG_OK,
    QL_CTSREG_REG_NOK,
    QL_CTSREG_REG_A,
}ql_ctsreg_state_code;

typedef struct{
	char         model_id[20];
	char         sw_version[128];
	char         iccid[64];
	char         imei[64];
	char         imsi[64];
	char         cellid[64];
	char         date[128];
}ql_ctsreg_param_t;

typedef struct{
	int                 contextid;
	int                 sim_id;
	char                server_addr[256];
	uint16_t            server_port;
	char                registered_iccid[64];
	int                 encrypt;
	int                 retry_cnt;
	int                 resp_timeout;
	int                 interval_timeout;
}ql_ctsreg_option_t;

typedef void (*ctsreg_state_cb)(ql_ctsreg_hndl  cli_hndl, int state_code, int finished, void *arg);

ql_ctsreg_hndl  ql_ctsreg_perform(ql_ctsreg_option_t *option, ql_ctsreg_param_t  * reg_param, ctsreg_state_cb      state_cb, void *arg, int *err_num);

#endif
