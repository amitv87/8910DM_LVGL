#ifndef QL_SSL_H
#define QL_SSL_H

#if 1//defined(CONFIG_QUEC_PROJECT_FEATURE_SSL)

#define QL_MAX_CA_CERT_CNT        10


#define QL_SSL_TLS_RSA_WITH_NULL_MD5                    0x0001   /**< Weak! */
#define QL_SSL_TLS_RSA_WITH_NULL_SHA                    0x0002   /**< Weak! */

#define QL_SSL_TLS_RSA_WITH_RC4_128_MD5                 0x0004
#define QL_SSL_TLS_RSA_WITH_RC4_128_SHA                 0x0005
#define QL_SSL_TLS_RSA_WITH_DES_CBC_SHA                 0x0009   /**< Weak! Not in TLS 1.2 */

#define QL_SSL_TLS_RSA_WITH_3DES_EDE_CBC_SHA            0x000A

#define QL_SSL_TLS_DHE_RSA_WITH_DES_CBC_SHA             0x0015   /**< Weak! Not in TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA        0x0016

#define QL_SSL_TLS_PSK_WITH_NULL_SHA                    0x002C   /**< Weak! */
#define QL_SSL_TLS_DHE_PSK_WITH_NULL_SHA                0x002D   /**< Weak! */
#define QL_SSL_TLS_RSA_PSK_WITH_NULL_SHA                0x002E   /**< Weak! */
#define QL_SSL_TLS_RSA_WITH_AES_128_CBC_SHA             0x002F

#define QL_SSL_TLS_DHE_RSA_WITH_AES_128_CBC_SHA         0x0033
#define QL_SSL_TLS_RSA_WITH_AES_256_CBC_SHA             0x0035
#define QL_SSL_TLS_DHE_RSA_WITH_AES_256_CBC_SHA         0x0039

#define QL_SSL_TLS_RSA_WITH_NULL_SHA256                 0x003B   /**< Weak! */
#define QL_SSL_TLS_RSA_WITH_AES_128_CBC_SHA256          0x003C   /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_WITH_AES_256_CBC_SHA256          0x003D   /**< TLS 1.2 */

#define QL_SSL_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA        0x0041
#define QL_SSL_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA    0x0045

#define QL_SSL_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256      0x0067   /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256      0x006B   /**< TLS 1.2 */

#define QL_SSL_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA        0x0084
#define QL_SSL_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA    0x0088

#define QL_SSL_TLS_PSK_WITH_RC4_128_SHA                 0x008A
#define QL_SSL_TLS_PSK_WITH_3DES_EDE_CBC_SHA            0x008B
#define QL_SSL_TLS_PSK_WITH_AES_128_CBC_SHA             0x008C
#define QL_SSL_TLS_PSK_WITH_AES_256_CBC_SHA             0x008D

#define QL_SSL_TLS_DHE_PSK_WITH_RC4_128_SHA             0x008E
#define QL_SSL_TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA        0x008F
#define QL_SSL_TLS_DHE_PSK_WITH_AES_128_CBC_SHA         0x0090
#define QL_SSL_TLS_DHE_PSK_WITH_AES_256_CBC_SHA         0x0091

#define QL_SSL_TLS_RSA_PSK_WITH_RC4_128_SHA             0x0092
#define QL_SSL_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA        0x0093
#define QL_SSL_TLS_RSA_PSK_WITH_AES_128_CBC_SHA         0x0094
#define QL_SSL_TLS_RSA_PSK_WITH_AES_256_CBC_SHA         0x0095

#define QL_SSL_TLS_RSA_WITH_AES_128_GCM_SHA256          0x009C   /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_WITH_AES_256_GCM_SHA384          0x009D   /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256      0x009E   /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384      0x009F   /**< TLS 1.2 */

#define QL_SSL_TLS_PSK_WITH_AES_128_GCM_SHA256          0x00A8   /**< TLS 1.2 */
#define QL_SSL_TLS_PSK_WITH_AES_256_GCM_SHA384          0x00A9   /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256      0x00AA   /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_PSK_WITH_AES_256_GCM_SHA384      0x00AB   /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_PSK_WITH_AES_128_GCM_SHA256      0x00AC   /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_PSK_WITH_AES_256_GCM_SHA384      0x00AD   /**< TLS 1.2 */

#define QL_SSL_TLS_PSK_WITH_AES_128_CBC_SHA256          0x00AE
#define QL_SSL_TLS_PSK_WITH_AES_256_CBC_SHA384          0x00AF
#define QL_SSL_TLS_PSK_WITH_NULL_SHA256                 0x00B0   /**< Weak! */
#define QL_SSL_TLS_PSK_WITH_NULL_SHA384                 0x00B1   /**< Weak! */

#define QL_SSL_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256      0x00B2
#define QL_SSL_TLS_DHE_PSK_WITH_AES_256_CBC_SHA384      0x00B3
#define QL_SSL_TLS_DHE_PSK_WITH_NULL_SHA256             0x00B4   /**< Weak! */
#define QL_SSL_TLS_DHE_PSK_WITH_NULL_SHA384             0x00B5   /**< Weak! */

#define QL_SSL_TLS_RSA_PSK_WITH_AES_128_CBC_SHA256      0x00B6
#define QL_SSL_TLS_RSA_PSK_WITH_AES_256_CBC_SHA384      0x00B7
#define QL_SSL_TLS_RSA_PSK_WITH_NULL_SHA256             0x00B8   /**< Weak! */
#define QL_SSL_TLS_RSA_PSK_WITH_NULL_SHA384             0x00B9   /**< Weak! */

#define QL_SSL_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256     0x00BA   /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 0x00BE   /**< TLS 1.2 */

#define QL_SSL_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256     0x00C0   /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 0x00C4   /**< TLS 1.2 */

#define QL_SSL_TLS_ECDH_ECDSA_WITH_NULL_SHA             0xC001 /**< Weak! */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_RC4_128_SHA          0xC002 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA     0xC003 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA      0xC004 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA      0xC005 /**< Not in SSL3! */

#define QL_SSL_TLS_ECDHE_ECDSA_WITH_NULL_SHA            0xC006 /**< Weak! */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_RC4_128_SHA         0xC007 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA    0xC008 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA     0xC009 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA     0xC00A /**< Not in SSL3! */

#define QL_SSL_TLS_ECDH_RSA_WITH_NULL_SHA               0xC00B /**< Weak! */
#define QL_SSL_TLS_ECDH_RSA_WITH_RC4_128_SHA            0xC00C /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA       0xC00D /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA        0xC00E /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA        0xC00F /**< Not in SSL3! */

#define QL_SSL_TLS_ECDHE_RSA_WITH_NULL_SHA              0xC010 /**< Weak! */
#define QL_SSL_TLS_ECDHE_RSA_WITH_RC4_128_SHA           0xC011 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA      0xC012 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA       0xC013 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA       0xC014 /**< Not in SSL3! */

#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256  0xC023 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384  0xC024 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256   0xC025 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384   0xC026 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256    0xC027 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384    0xC028 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256     0xC029 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384     0xC02A /**< TLS 1.2 */

#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256  0xC02B /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384  0xC02C /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256   0xC02D /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384   0xC02E /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256    0xC02F /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384    0xC030 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256     0xC031 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384     0xC032 /**< TLS 1.2 */

#define QL_SSL_TLS_ECDHE_PSK_WITH_RC4_128_SHA           0xC033 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_PSK_WITH_3DES_EDE_CBC_SHA      0xC034 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA       0xC035 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA       0xC036 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256    0xC037 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384    0xC038 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_PSK_WITH_NULL_SHA              0xC039 /**< Weak! No SSL3! */
#define QL_SSL_TLS_ECDHE_PSK_WITH_NULL_SHA256           0xC03A /**< Weak! No SSL3! */
#define QL_SSL_TLS_ECDHE_PSK_WITH_NULL_SHA384           0xC03B /**< Weak! No SSL3! */

#define QL_SSL_TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256 0xC072 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384 0xC073 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_CAMELLIA_128_CBC_SHA256  0xC074 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_CAMELLIA_256_CBC_SHA384  0xC075 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256   0xC076 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384   0xC077 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_RSA_WITH_CAMELLIA_128_CBC_SHA256    0xC078 /**< Not in SSL3! */
#define QL_SSL_TLS_ECDH_RSA_WITH_CAMELLIA_256_CBC_SHA384    0xC079 /**< Not in SSL3! */

#define QL_SSL_TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256         0xC07A /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384         0xC07B /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256     0xC07C /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384     0xC07D /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256 0xC086 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384 0xC087 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_CAMELLIA_128_GCM_SHA256  0xC088 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_ECDSA_WITH_CAMELLIA_256_GCM_SHA384  0xC089 /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256   0xC08A /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384   0xC08B /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_RSA_WITH_CAMELLIA_128_GCM_SHA256    0xC08C /**< TLS 1.2 */
#define QL_SSL_TLS_ECDH_RSA_WITH_CAMELLIA_256_GCM_SHA384    0xC08D /**< TLS 1.2 */

#define QL_SSL_TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256       0xC08E /**< TLS 1.2 */
#define QL_SSL_TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384       0xC08F /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_PSK_WITH_CAMELLIA_128_GCM_SHA256   0xC090 /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_PSK_WITH_CAMELLIA_256_GCM_SHA384   0xC091 /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_PSK_WITH_CAMELLIA_128_GCM_SHA256   0xC092 /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_PSK_WITH_CAMELLIA_256_GCM_SHA384   0xC093 /**< TLS 1.2 */

#define QL_SSL_TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256       0xC094
#define QL_SSL_TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384       0xC095
#define QL_SSL_TLS_DHE_PSK_WITH_CAMELLIA_128_CBC_SHA256   0xC096
#define QL_SSL_TLS_DHE_PSK_WITH_CAMELLIA_256_CBC_SHA384   0xC097
#define QL_SSL_TLS_RSA_PSK_WITH_CAMELLIA_128_CBC_SHA256   0xC098
#define QL_SSL_TLS_RSA_PSK_WITH_CAMELLIA_256_CBC_SHA384   0xC099
#define QL_SSL_TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256 0xC09A /**< Not in SSL3! */
#define QL_SSL_TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384 0xC09B /**< Not in SSL3! */

#define QL_SSL_TLS_RSA_WITH_AES_128_CCM                0xC09C  /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_WITH_AES_256_CCM                0xC09D  /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_AES_128_CCM            0xC09E  /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_AES_256_CCM            0xC09F  /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_WITH_AES_128_CCM_8              0xC0A0  /**< TLS 1.2 */
#define QL_SSL_TLS_RSA_WITH_AES_256_CCM_8              0xC0A1  /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_AES_128_CCM_8          0xC0A2  /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_RSA_WITH_AES_256_CCM_8          0xC0A3  /**< TLS 1.2 */
#define QL_SSL_TLS_PSK_WITH_AES_128_CCM                0xC0A4  /**< TLS 1.2 */
#define QL_SSL_TLS_PSK_WITH_AES_256_CCM                0xC0A5  /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_PSK_WITH_AES_128_CCM            0xC0A6  /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_PSK_WITH_AES_256_CCM            0xC0A7  /**< TLS 1.2 */
#define QL_SSL_TLS_PSK_WITH_AES_128_CCM_8              0xC0A8  /**< TLS 1.2 */
#define QL_SSL_TLS_PSK_WITH_AES_256_CCM_8              0xC0A9  /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_PSK_WITH_AES_128_CCM_8          0xC0AA  /**< TLS 1.2 */
#define QL_SSL_TLS_DHE_PSK_WITH_AES_256_CCM_8          0xC0AB  /**< TLS 1.2 */
/* The last two are named with PSK_DHE in the RFC, which looks like a typo */

#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_128_CCM        0xC0AC  /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_256_CCM        0xC0AD  /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8      0xC0AE  /**< TLS 1.2 */
#define QL_SSL_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8      0xC0AF  /**< TLS 1.2 */

#define QL_SSL_TLS_ECJPAKE_WITH_AES_128_CCM_8          0xC0FF  /**< experimental */

typedef enum
{
 	QL_SSL_VERSION_0 = 0,         /**< SSL protocol ver. 3.0. */
  	QL_SSL_VERSION_1,             /**< TLS protocol ver. 1.0 (SSL 3.1). */
  	QL_SSL_VERSION_2,             /**< TLS protocol ver. 1.1 (SSL 3.2). */
  	QL_SSL_VERSION_3,
  	//QL_SSL_VERSION_4,
  	QL_SSL_VERSION_ALL
} ql_ssl_version_type_e;

typedef enum
{
   QL_SSL_VERIFY_NULL          = 0x0000,
   QL_SSL_VERIFY_SERVER        = 0x0001,
   QL_SSL_VERIFY_CLIENT_SERVER = 0x0002,
} ql_ssl_authmode_e;


typedef enum{
	QL_SSL_TLS_PROTOCOL  = 0,
	QL_SSL_DTLS_PROTOCOL = 1,
}ql_ssl_transport_type_e;

typedef int  ql_ssl_context;

typedef void(*ql_ssl_handshark_timeout_cb)(ql_ssl_context *ssl_ctx, void *arg);

typedef struct{
	int      	   	 ssl_version;
	int         	 transport; //0: TLS  1: DTLS
	int        		*ciphersuites;
	int              auth_mode;
	int              sni_enable;
	char            *ca_cert_path[QL_MAX_CA_CERT_CNT];
	char            *own_cert_path;
	char            *own_key_path;
	char            *own_key_pwd;
	unsigned char   *psk_key;
	int              psk_key_len;
	unsigned char   *psk_identity;
	int              psk_identity_len;
	int              ssl_negotiate_timeout;
	ql_ssl_handshark_timeout_cb   negotiate_timeout_cb;
	void            *negotiate_timeout_cb_arg;
}ql_ssl_config;



typedef enum{
	QL_SSL_CONF_VERSION        = 1,
	QL_SSL_CONF_TRANSPORT      = 2,
	QL_SSL_CONF_CIPHERSUITE    = 3,
	QL_SSL_CONF_AUTHMODE       = 4,
	QL_SSL_CONF_CACERT         = 5,
	QL_SSL_CONF_OWNCERT        = 6,
	QL_SSL_CONF_PSK            = 7,
	QL_SSL_CONF_SNI            = 8,
	QL_SSL_CONF_HS_TIMEOUT     = 9,
	QL_SSL_CONF_IGNORE_LOCALTM = 10,
	QL_SSL_CONF_HS_TIMEOUT_FUNC= 11,
}ql_ssl_config_type_e;


typedef enum{
	QL_SSL_SUCCESS                = 0,
	QL_SSL_ERROR_UNKOWN           = -1,
	QL_SSL_ERROR_WOUNDBLOCK       = -2,
	QL_SSL_ERROR_INVALID_PARAM    = -3,
	QL_SSL_ERROR_OUT_OF_MEM       = -4,
	QL_SSL_ERROR_NOT_SUPPORT      = -5,
	QL_SSL_ERROR_HS_FAILURE       = -6,
	QL_SSL_ERROR_DECRYPT_FAILURE  = -7,
	QL_SSL_ERROR_ENCRYPT_FAILURE  = -8,
	QL_SSL_ERROR_HS_INPROGRESS    = -9,
	QL_SSL_ERROR_BAD_REQUEST      = -10,
	QL_SSL_ERROR_WANT_READ        = -11,
	QL_SSL_ERROR_WANT_WRITE       = -12,
	QL_SSL_ERROR_SOCKET_RESET     = -13,
}ql_ssl_error_code_e;

/*****************************************************************
* Function: ql_ssl_conf_init
*
* Description: 初始化ssl上下文配置项的结构信息
* 
* Parameters:
*	conf        [in]    ssl上下文配置项结构体指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_conf_init(ql_ssl_config *conf);

/*****************************************************************
* Function: ql_ssl_conf_set
*
* Description: 设置ssl上下文配置项中的配置参数
* 
* Parameters:
*	conf        [in]    ssl上下文配置项结构体指针.
*   type        [in]    ssl上下文配置项的参数处理类型
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_conf_set(ql_ssl_config *conf, int type, ...);

/*****************************************************************
* Function: ql_ssl_conf_get
*
* Description: 获取已设置的ssl上下文配置项的配置参数
* 
* Parameters:
*	conf        [in]    ssl上下文配置项结构体指针.
*   type        [in]    ssl上下文中配置参数处理类型
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_conf_get(ql_ssl_config *conf, int type, ...);

/*****************************************************************
* Function: ql_ssl_conf_free
*
* Description: 释放ssl上下文配置项结构体的内存块
* 
* Parameters:
*	conf        [in]    ssl上下文配置项结构体指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_conf_free(ql_ssl_config *conf);

/*****************************************************************
* Function: ql_ssl_new
*
* Description: 初始化ssl上下文结构
* 
* Parameters:
*	ssl        [in]    ssl上下文结构体指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_new(ql_ssl_context *ssl);

/*****************************************************************
* Function: ql_ssl_setup
*
* Description: 通过conf配置ssl上下文,ssl上下文信息确定ssl连接的属性
* 
* Parameters:
*	ssl        [in]    ssl上下文结构体指针.
*   conf       [in]    ssl上下文配置项结构体指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_setup(ql_ssl_context *ssl, ql_ssl_config *conf);

/*****************************************************************
* Function: ql_ssl_set_socket_fd
*
* Description: 关联当前ssl上下文与指定的套接字描述符,以及设置mbed bio的读写函数
* 
* Parameters:
*	ssl         [in]    ssl上下文结构体指针.
*   sock_fd     [in]    socket套接字描述符.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_set_socket_fd(ql_ssl_context *ssl, int sock_fd);

/*****************************************************************
* Function: ql_ssl_set_hostname
*
* Description: 设置远端主机名用以检查接收到的服务器证书
* 
* Parameters:
*	ssl         [in]    ssl上下文结构体指针.
*   hostname    [in]    远端IP地址或域名地址
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_set_hostname(ql_ssl_context *ssl, const char *hostname);

/*****************************************************************
* Function: ql_ssl_handshark
*
* Description: ssl握手通信,通过ssl上下文确定ssl建立连接属性
* 
* Parameters:
*	ssl        [in]    ssl上下文结构体指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_handshark(ql_ssl_context *ssl);

/*****************************************************************
* Function: ql_ssl_close_notify
*
* Description: 通知ssl连接对端连接正在关闭
* 
* Parameters:
*	ssl        [in]    ssl上下文结构体指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_close_notify(ql_ssl_context *ssl);

/*****************************************************************
* Function: ql_ssl_get_bytes_avail
*
* Description: 返回可读取的数据字节数
* 
* Parameters:
*	ssl         [in]    ssl上下文结构体指针.
*
* Return:
* 	返回读缓冲区中有多少字节可用
*
*****************************************************************/
int  ql_ssl_get_bytes_avail(ql_ssl_context *ssl);

/*****************************************************************
* Function: ql_ssl_read
*
* Description: 从缓冲区中读取len字节的数据
* 
* Parameters:
*	ssl         [in]    ssl上下文结构体指针.
*   buf         [in]    读buf缓冲区指针.
*   len         [in]    读取的字节数.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_read(ql_ssl_context *ssl, unsigned char *buf, size_t len);

/*****************************************************************
* Function: ql_ssl_write
*
* Description: 向缓冲区中写入len字节的数据
* 
* Parameters:
*	ssl         [in]    ssl上下文结构体指针.
*   buf         [in]    保存数据buf缓冲区指针.
*   len         [in]    读取的字节数.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_write(ql_ssl_context *ssl, const unsigned char *buf, size_t len);

/*****************************************************************
* Function: ql_ssl_free
*
* Description: 释放ssl上下文,以及ssl上下文所占用的内存块
* 
* Parameters:
*	ssl         [in]    ssl上下文结构体指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_free(ql_ssl_context *ssl);

/*****************************************************************
* Function: ql_ssl_handshark_finished
*
* Description: ssl握手通信结束,更改ssl上下文状态为HANDSHAKE_OVER
* 
* Parameters:
*	ssl         [in]    ssl上下文结构体指针.
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_handshark_finished(ql_ssl_context *ssl);

/*****************************************************************
* Function: ql_ssl_ciphersuit_is_valid
*
* Description: 判断所指定的SSL密码组是否支持
* 
* Parameters:
*	cs_id       [in]    SSL加密套件的数值id
*
* Return:
* 	0           成功
*	other       错误码
*
*****************************************************************/
int  ql_ssl_ciphersuit_is_valid(int cs_id);

#endif

#endif
