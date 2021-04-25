#ifndef QL_MMS_CLIENT_H
#define QL_MMS_CLIENT_H

#define mms_print(msg, ...)     custom_log("MMS_CLI", msg, ##__VA_ARGS__)

#define QL_MMS_BCC_ADDR_MAX_CNT             6
#define QL_MMS_CC_ADDR_MAX_CNT              6
#define QL_MMS_TO_ADDR_MAX_CNT              6
#define QL_MMS_ATTACHMENT_MAX_CNT           13

typedef void*  ql_mms_client; 

typedef enum{
	QL_MMS_OK                        = 0,
	QL_MMS_INVLIAD_PARAM             = -1,
	QL_MMS_WOULDBLOCK                = -2,
	QL_MMS_OUT_OF_MEM                = -3,
	QL_MMS_HTTP_REQ_FAIL             = -4,
	QL_MMS_OPT_BUSY                  = -5,
	QL_MMS_HTTP_RESP_FAIL            = -6,
	QL_MMS_SEND_FAIL                 = -7,
	QL_MMS_HTTP_TIMEOUT              = -8,
	QL_MMS_DNS_FAIL                  = -9,
}mms_result_code_e;

typedef enum{
	MMS_CHARSET_ASCII_VALUE      = 0x03,
	MMS_CHARSET_UTF8_VALUE       = 0x6A,
	MMS_CHARSET_UCS2_VALUE       = 0x03E8,
	MMS_CHARSET_BIG5_VALUE       = 0x07EA,
	MMS_CHARSET_GBK_VALUE        = 0x71,
}mms_charset_type;


typedef enum 
{
    MMS_PARAM_VALID_1HOUR=0,
    MMS_PARAM_VALID_12HOURS,
    MMS_PARAM_VALID_24HOURS,
    MMS_PARAM_VALID_2DAYS,
    MMS_PARAM_VALID_1WEEK,
    MMS_PARAM_VALID_MAXIMUM,
    MMS_PARAM_VALID_DEFAULT
}mms_valid_type;

typedef enum 
{
    MMS_PARAM_PRIORITY_LOWEST=0,
    MMS_PARAM_PRIORITY_MORMAL,
    MMS_PARAM_PRIORITY_HIGHEST,
    MMS_PARAM_PRIORITY_DEFAULT
}mms_priority_type;

typedef enum 
{
    MMS_PARAM_SENDERADDR_HIDE=0,
    MMS_PARAM_SENDERADDR_SHOW,
    MMS_PARAM_SENDERADDR_DEFAULT
}mms_visible_type;

typedef enum
{
    MMS_PARAM_CLASS_PERSONAL=0,
    MMS_PARAM_CLASS_ADVERTISEMENT,
    MMS_PARAM_CLASS_INFORMATIONAL,
    MMS_PARAM_CLASS_AUTO,
    MMS_PARAM_CLASS_DEFAULT
}mms_class_type;

struct mms_send_param_t{
	int            validity_period;
	int            priority;
	int            delivery_report;
	int            read_report;
	int            visible;
	int            class;
};

struct mms_addr_t{
	struct mms_addr_t  *next;
	int                 addr_len;
	char               *address;
};

struct mms_attachment_t{
	struct mms_attachment_t  *next;
	char                     *file_path;
};

struct mms_subject_t{
	char                   *subject;
	int                     length;
};

struct mms_option_t{
	int             	          context_id;
	int              	          sim_id;
	char                         *mmsc_url;
	char           	             *proxy_addr;
	uint16_t                      proxy_port;
	int                           charset;	
	uint8_t                       supportfield;
	int                           wait_timeout;
	struct mms_send_param_t       send_param;
};

struct mms_info_t{
	struct mms_subject_t        *subject;
	struct mms_addr_t           *to_addr;
	struct mms_addr_t           *cc_addr;
	struct mms_addr_t           *bcc_addr;
	struct mms_attachment_t     *attachments;
};

typedef void(*mms_send_result_cb)(ql_mms_client        client_hndl, int mms_result, int http_code, char *mms_err_msg, void *arg);

int  ql_mms_client_new(ql_mms_client *client_hndl, mms_send_result_cb       result_cb, void *arg);

int  ql_mms_client_send_msg(ql_mms_client *client_hndl, struct mms_option_t *send_option, struct mms_info_t  *mms_info);

int  ql_mms_client_release(ql_mms_client *client_hndl);

#endif
