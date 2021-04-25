/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_GNSS_H
#define QL_GNSS_H

#include "ql_api_common.h"
#include "ql_uart.h"
#include "time.h"
#include <time.h>
//#include "quec_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Typedef
 ===========================================================================*/
typedef void (*ql_gnss_callback)(uint32 ind_type, ql_uart_port_number_e port, uint32 size);
typedef char nmea_cardinal_t;
typedef int32 loc_client_handle_type;
/** Number of item types; unsigned integer. */
typedef unsigned int dsat_num_item_type;
typedef unsigned char boolean;
typedef uint64 loc_position_valid_mask_type;
extern unsigned char  quec_GPS_NMEATYPE_table[];
extern unsigned char  quec_GLONASS_NMEATYPE_table[];
extern unsigned char  quec_GALILEO_NMEATYPE_table[];
extern unsigned char  quec_BEIDOU_NMEATYPE_table[];
extern unsigned char  quec_GNSS_NMEATYPE_table[];
/*===========================================================================
 * Typedef
 ===========================================================================*/
typedef void (*ql_gnss_callback)(uint32 ind_type, ql_uart_port_number_e port, uint32 size);
typedef char nmea_cardinal_t;
typedef int32 loc_client_handle_type;
/** Number of item types; unsigned integer. */
typedef unsigned int dsat_num_item_type;
typedef unsigned char boolean;
typedef uint64 loc_position_valid_mask_type;

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#define ARRY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#define ARRAY_LENGTH(a) (sizeof a / sizeof (a[0]))
#define QL_GNSS_DATA_SIZE    1024
#define QL_GNSS_ERRCODE_BASE (QL_COMPONENT_BSP_GNSS<<16)
#define QL_GSV_MAX_SATS  (40)
#define GNSS_UART QL_UART_PORT_3
#define QUEC_GPS_RECBUF_LEN_MAX  1024
#define QUEC_GPS_CMD_LEN_MAX  32
#define NMEA_END_CHAR_1 '\r'
#define NMEA_END_CHAR_2 '\n'
#define jmin(a, b) (((a) > (b)) ? (b) : (a))
#define jmax(a, b) (((a) > (b)) ? (a) : (b))
/* nmea gsv packet define */
#define NMEA_GPGSV_TOTAL_MSGS               0
#define NMEA_GPGSV_MSG_NR                   1
#define NMEA_GPGSV_SATS                     2
#define NMEA_GPGSV_SAT_INFO_NR1             3
#define NMEA_GPGSV_SAT_INFO_ELEV1           4
#define NMEA_GPGSV_SAT_INFO_AZIMUTH1        5
#define NMEA_GPGSV_SAT_INFO_SNR1            6
#define NMEA_GPGSV_SAT_INFO_NR2             7
#define NMEA_GPGSV_SAT_INFO_ELEV2           8
#define NMEA_GPGSV_SAT_INFO_AZIMUTH2        9
#define NMEA_GPGSV_SAT_INFO_SNR2            10
#define NMEA_GPGSV_SAT_INFO_NR3             11
#define NMEA_GPGSV_SAT_INFO_ELEV3           12
#define NMEA_GPGSV_SAT_INFO_AZIMUTH3        13
#define NMEA_GPGSV_SAT_INFO_SNR3            14
#define NMEA_GPGSV_SAT_INFO_NR4             15
#define NMEA_GPGSV_SAT_INFO_ELEV4           16
#define NMEA_GPGSV_SAT_INFO_AZIMUTH4        17
#define NMEA_GPGSV_SAT_INFO_SNR4            18
#define NMEA_GPGSV_SAT_INFO_SIGID           19

#define KNOTS_CONVERSION_FACTOR         (1.852)
            
#define QL_GPS_ERRCODE_BASE (QL_COMPONENT_BSP_GNSS<<16)
    
#define NMEA_MAX_LENGTH  (82)
    
#define NMEA_CARDINAL_DIR_NORTH		(nmea_cardinal_t) 'N'
#define NMEA_CARDINAL_DIR_EAST		(nmea_cardinal_t) 'E'
#define NMEA_CARDINAL_DIR_SOUTH		(nmea_cardinal_t) 'S'
#define NMEA_CARDINAL_DIR_WEST		(nmea_cardinal_t) 'W'
#define NMEA_CARDINAL_DIR_UNKNOWN	(nmea_cardinal_t) '\0'
    
#define NMEA_PREFIX_LENGTH          (5)
#define NMEA_TIME_FORMAT	"%H%M%S"
#define NMEA_TIME_FORMAT_LEN	6
    
#define NMEA_DATE_FORMAT	"%d%m%y"
#define NMEA_DATE_FORMAT_LEN	6

#define KNOTS_CONVERSION_FACTOR         (1.852)
            
#define QL_GPS_ERRCODE_BASE (QL_COMPONENT_BSP_GNSS<<16)
    
#define QL_GSV_MAX_SATS  (40)  

#define NMEA_GPGGA_UTC  0

/* Value indexes */
#define NMEA_GPRMC_TIME			            0
#define NMEA_GPRMC_STATUS                   1
#define NMEA_GPRMC_LATITUDE		            2
#define NMEA_GPRMC_LATITUDE_CARDINAL	    3
#define NMEA_GPRMC_LONGITUDE		        4
#define NMEA_GPRMC_LONGITUDE_CARDINAL	    5
#define NMEA_GPRMC_SPEED		            6
#define NMEA_GPRMC_COURSE                   7
#define NMEA_GPRMC_DATE			            8
/*nmea gga packet define*/
//2,3,4,5,6,7
#define NMEA_GPGGA_LATITUDE                     1
#define NMEA_GPGGA_LATITUDE_CARDINAL            2
#define NMEA_GPGGA_LONGITUDE                    3
#define NMEA_GPGGA_LONGITUDE_CARDINAL           4
#define NMEA_GPGGA_QUALITY                      5
#define NMEA_GPGGA_SATELLITES_TRACKED           6
#define NMEA_GPGGA_HDOP                         7
#define NMEA_GPGGA_ALTITUDE                     8
/*nmea gsa packet define*/
#define NMEA_GPGSA_MODE                         0
#define NMEA_GPGSA_NAVMODE                      1
#define NMEA_GPGSA_PDOP                         14
#define NMEA_GPGSA_HDOP                         15
#define NMEA_GPGSA_VDOP                         16
#define NMEA_GPGSA_systemID                     17


#define PDAPI_NMEA_GGA 0x0001
#define PDAPI_NMEA_RMC 0x0002
#define PDAPI_NMEA_GSV 0x0004
#define PDAPI_NMEA_GSA 0x0008
#define PDAPI_NMEA_VTG 0x0010
    
/*nmea txt packet define*/
#define NMEA_GNTXT_FWVER                        3  
/*===========================================================================
* Enum
===========================================================================*/
 typedef enum
 {
     QUEC_GNSS_UNKNOW_TYPE,
     QUEC_GNSS_RESET_TYPE,
     QUEC_GNSS_BAUD_SET, 
     QUEC_GNSS_INFO,
     QUEC_GNSS_NMEA_TYPE,
     QUEC_GNSS_CFGSYS,
     QUEC_GNSS_SYS_SAVE,
     QUEC_CNSS_CUSTOM_CMD,
}quec_gnss_cmd;
    
typedef enum
{
   GPS_UNKNOW,
   GSP_OK_CMD=0x01,
   GPS_FAIL_0_CMD=0x02,
   GPS_FAIL_1_CMD=0x04,
   GPS_INFO_CMD=0x08,
}quec_gnss_cmd_resp;
typedef enum {
    NMEA_UNKNOWN,
    NMEA_RMC,
    NMEA_GGA,
    NMEA_GSA,
    NMEA_GSV,
    NMEA_VTG,
    NMEA_TXT,
} nmea_type;

typedef enum {
    SAT_UNKNOWN,
    SAT_GPS,
    SAT_GLONASS,
    SAT_GALILEO,
    SAT_BDS,
    SAT_MULSYS
} satellite_type;

typedef enum {
    PORT_NONE,
    PORT_USBNMEA,
    PORT_UARTDEBUG,
    PORT_MAX,
} nmea_outport_row;
typedef enum {
    QGPS_PROTO_NMEA_SRC_GGA,        /* GPGGA */
    QGPS_PROTO_NMEA_SRC_GLL,        /* GPGGA */
    QGPS_PROTO_NMEA_SRC_GSA,        /* GPGSA GNGSA PQGSA */
    QGPS_PROTO_NMEA_SRC_GSV,        /* GPGSV GLGSV GAGSV PQGSV */
    QGPS_PROTO_NMEA_SRC_RMC,        /* GPRMC */
    QGPS_PROTO_NMEA_SRC_VTG,        /* GPVTG */
    QGPS_PROTO_NMEA_SRC_ZDA,        /* GPVTG */
    QGPS_PROTO_NMEA_SRC_GST,        /* GPVTG */
QGPS_PROTO_NMEA_SRC_MAX,
} qgps_proto_nmea_src_e_type;

typedef enum {
    QGPS_START_COLD,
    QGPS_START_HOT,
    QGPS_START_WARM,
    QGPS_START_XTRA
} qgps_start_e_type;

typedef enum
{
    QL_GNSS_SUCCESS  = QL_SUCCESS,

    QL_GNSS_EXECUTE_ERR                  = 1|QL_GNSS_ERRCODE_BASE,
    QL_GNSS_INVALID_PARAM_ERR,
    QL_GNSS_NOT_SUPPORT_ERR,
    QL_GNSS_UART_SET_ERR,
    QL_GNSS_CB_NULL_ERR,
    QL_GNSS_ALREADY_OPEN,
    QL_GNSS_NVM_WRITE_ERR,
    QL_GNSS_NEMA_PARSE_ERR
}ql_errcode_gnss;

/**
 * GNSS switch select
 */
typedef enum
{
    GNSS_DISABLE,
    GNSS_ENABLE,
    GNSS_RESET
}ql_GnssSW;
typedef enum
{
     AUTO_GNSS_DISABLE,
     AUTO_GNSS_ENABLE
}ql_AutoGnssSW;
typedef enum
{
    AGPS_GNSS_DISABLE,
    AGPS_GNSS_ENABLE
}ql_AGPSGnssSW;
//typedef enum
//{
//     gpsupdate_flagCLR,
//     gpsupdate_flagSET
//}ql_GnssUpdateFlag; 
typedef enum
{
   GNSS_CLOSE,
   GNSS_FIRMWARE_UPDATE,
   GNSS_POSITIONING,
   GNSS_FIX
}ql_GnssState; 

/*===========================================================================
 * data struct
===========================================================================*/
typedef struct ql_gnss_data_struct {
    /*鏄惁鏈夋晥瀹氫�?*/
    unsigned char valid;
    /*缁忓�?*/
    double longitude;
    /*缁忓害E(涓滅�?)鎴朩(瑗跨�?)*/
    unsigned char longitude_cardinal;
    /*绾�?*/
    double latitude;
    /*绾害N(鍖楃�?)鎴朣(鍗楃�?)*/
    unsigned char latitude_cardinal;
    /*hdop绮惧�?*/
    float  hdop;
    /*phop绮惧�?*/
    float pdop;
    /*鑸�? 0-360*/
    double heading;
    /*閫熷�? km/h*/
    float gps_speed;
    /*淇″彿寮哄�?,max=5, data from avg_snr*/
    unsigned char gps_signal;
    /*鏈€澶т俊鍙峰�?? db*/
    unsigned int max_cnr;
    unsigned int max_cnr2;
    /*鏈€灏忎俊鍙峰€? db*/
    unsigned int min_cnr;
    /*骞冲潎淇″彿�?? db*/
    unsigned int avg_cnr;
    /*淇″彿鍊兼暟缁?*/
    unsigned int cnrs[QL_GSV_MAX_SATS];
    unsigned int cnrs_index;
    /*瀹氫綅鍗槦鏁伴�?*/
    unsigned int satellites_num;
    /*娴锋嫈楂樺害*/
    float altitude;
    /*FWVER*/
    char fwver[32];

    struct tm time;
    
    unsigned int UTC;
 
    unsigned char quality;
    unsigned char navmode;
}ql_gnss_data_t;

typedef struct
{
//   TelAtParserID       atp_index;
//    uint32              atHandle;
    quec_gnss_cmd       cmd_id;
    char*               cmd_data;
    char                cmd_len;
    char                cmd_checksum;
    quec_gnss_cmd_resp  resp_type;  
}QGPS_CMD_TYPE;


typedef struct {
    unsigned int ih_magic;
    unsigned int ih_checksum;
    unsigned int ih_time;
    unsigned int ih_size;
    unsigned int ih_reserved;
    unsigned int pkg_flash_addr;
    unsigned int pkg_run_addr;
    unsigned int ih_hcrc;
} quec_image_header_t;

struct nmea_s{
satellite_type sat_type;
nmea_type type;
void *data;
};

/* GPS position struct */
struct nmea_position{
double minutes;
int degrees;
nmea_cardinal_t cardinal;
};

/* nmea rmc packet define */
struct nmea_gprmc_s{
/*1: valid, 0: invalid*/
int valid;
struct nmea_position longitude;
struct nmea_position latitude;
double course;
double speed;
struct tm time;
} ;

struct nmea_sat_info {
/* PRN */
int nr;
/*elevation of satellite*/
int elevation;
/*azimuth of satellite*/
int azimuth;
/*Signal Noise Ratio*/
int snr;
};

struct nmea_gpgsv_s {
int cur_sats;
int sig_id;
/*total msg count*/
int total_msgs;
/*current msg index*/
int msg_nr;
/*total satellite count*/
int total_sats;
/*satellites info*/
struct nmea_sat_info sats[4];
};

struct nmea_gpgga_s{
struct nmea_position longitude;
struct nmea_position latitude;
unsigned char quality;
unsigned int satellites_tracked;
double hdop;
double altitude;
unsigned int  time;
};

struct nmea_gpgsa_s {
unsigned char mode;
unsigned char navmode;
double pdop;
double hdop;
double vdop;
unsigned int systemid;
};

typedef struct {
  boolean                       nmeasrc_enable;
  qgps_proto_nmea_src_e_type    gnss_nmea_sentence_type;
/* GGA */
struct
{
    uint8 gp_gga_nmea_len;      /* GPGGA */
    char *gp_gga_sent;
}gga_sentences[5];  /* gpgga glgga gagga pqgga gngga */
/* GSA */
struct
{
    uint8 gp_gsa_nmea_len;
    char *gp_gsa_sent;
}gsa_sentences[5]; /* gpgsa glgsa gagsa pqgsa gngsa */
/* GSV */
struct 
{  
    uint8  gp_gsv_nmea_len[8];
    char   *gp_gsv_sent[8]; 
    uint8  cur_gsa_line;
}gsv_sentences[5];  /* gpgsv glgsv gagsv pqgsv gngsv*/
/* RMC */
struct
{
    uint8 gp_rmc_nmea_len;      /* GPRMC */
    char *gp_rmc_sent;
}rmc_sentences[5];  /* gprmc glrmc garmc pqrmc gnrmc*/
/* VTG */
struct
{
    uint8 gp_vtg_nmea_len;      /* GPVTG */
    char *gp_vtg_sent;
}vtg_sentences[5];  /* gpvtg glvtg gavtg pqvtg gnvtg*/
} nmeasrc_sentences;

typedef struct {
  boolean                       nmeasrc_enable;
  qgps_proto_nmea_src_e_type    gnss_nmea_sentence_type;
  uint32                        gnss_nmea_sys_type;
  uint32                        gps_gp_nmea_type;
  uint32                        gps_gl_nmea_type;
  uint32                        gps_ga_nmea_type;
  uint32                        gps_beidou_nmea_type; 
  uint32                        gnss_nmea_type; 
} nmeasrc_sentences_cfg;

typedef struct
{
uint16        year;
unsigned char month;
unsigned char day_of_week;
unsigned char day;
unsigned char hour;
unsigned char minute;
unsigned char second;
uint16        millisecond;
} loc_calendar_time_s_type;


typedef struct
{
dsat_num_item_type gnssmode;   // 
dsat_num_item_type fixmaxtime; //鏈€澶ц秴鏃堕噸鍚?
dsat_num_item_type fixmaxdist; // 瀹氫綅绮惧害
dsat_num_item_type b_fixcount_flag;
dsat_num_item_type fixcount;   // count of fs
dsat_num_item_type fixrate;    //fs  per/s
}quec_gps_config;

typedef struct
{
boolean  outport_en;
nmea_outport_row type;
unsigned char out_channel;
}quec_gps_outport_config;

typedef struct
{
    bool agpsflag;
    bool agps_assist_data_valid;
    int64_t local_last_time_min;
    uint8 profile_inx;
    char agps_url[100];
    char agpsvendorID[30];
    char agpsmodelID[30];
    char agpspassWord[30];
}quec_agps;

typedef struct
{
    ql_GnssState gnss_state;
    bool gnss_firmware_update_success;
    bool gnss_first_open_success;
}quec_gnss_flag;

typedef struct
{
    quec_gps_config confg;
    loc_calendar_time_s_type time;
    unsigned char start_type;
    nmeasrc_sentences_cfg nmea_handle_cfg;
    quec_gps_outport_config outport;
    bool gpsautoflag;
    quec_agps agps;
    char device_information[100];
}quec_gps_handle;

extern nmeasrc_sentences nmea_handle;
extern quec_gps_handle quec_gps_config_param;

struct nmea_gntxt_s {
char fwver[32];
};

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
/**
 * @brief GNSS function switch
 * @param gnss_sw : GNSS function enable/disable
 * @return
 *       QL_GNSS_INVALID_PARAM_ERR
 *       QL_GNSS_NO_SUPPORT_ERR
 *       QL_GNSS_ALREADY_OPEN
 *       QL_GNSS_SUCCESS
 */
ql_errcode_gnss ql_gnss_switch(ql_GnssSW gnss_sw);

/**
 * @brief GNSS auto start gnss cfg
 * @param gnss_sw : GNSS auto start enable/disable
 * @return
 *       QL_GNSS_INVALID_PARAM_ERR
 *       QL_GNSS_SUCCESS
 */ 
ql_errcode_gnss ql_auto_gnss_cfg(ql_AutoGnssSW autoflag);

/**
 * @brief user's GNSS callback register
 * @param gnss_cb : set callback by user
 * @return
 *       QL_GNSS_CB_NULL_ERR
 *       QL_GNSS_SUCCESS
 */
ql_errcode_gnss ql_gnss_callback_register(ql_gnss_callback gnss_cb);

/**
 * @brief GNSS nmea infomation get
 * @param pbuff : point for nmea info
 * @param len   : length for nmea info
 * @return
 *       QL_GNSS_INVALID_PARAM_ERR
 *       QL_GNSS_SUCCESS
 */
ql_errcode_gnss ql_gnss_nmea_get(ql_uart_port_number_e uart_port,unsigned char *pbuff, uint32 len);

/**
 * @brief GNSS update Firmware SUCCESS/NOT SUCCESS now
 * @param NULL
 * @return 
 *        ql_GnssState
 */
ql_GnssState ql_gnss_update_info_get(void);

/**
 * @brief GNSS agps cfg
 * @param gnssagpsflag:GNSS agps enable/disable
 * @return 
 *        QL_GNSS_INVALID_PARAM_ERR
          QL_GNSS_SUCCESS
 */
ql_errcode_gnss ql_gnss_agps_cfg(ql_AGPSGnssSW gnssagpsflag);

/**
 * @brief GNSS agps param cfg
 * @param profile: agps httpdowmload profile
 *        URL:     agps httpdowmload URL
 *        vendorID:agps httpdowmload vendorID
 *        modelID: agps httpdowmload modelID
 *        password:agps httpdowmload password
 * @return 
 *        QL_GNSS_INVALID_PARAM_ERR
          QL_GNSS_SUCCESS
 */
ql_errcode_gnss ql_gnss_agps_param_cfg(uint8 profile,const char *URL,const char *vendorID,const char *modelID,const char *password);

/**
 * @brief GNSS sys_type cfg
 * @param sys_type:
          0  GPS only锛坰upport�??    
          1  GPS+GLONASS+BeiDou+Galileo
          2  GPS+GLONASS+BeiDou
          3  GPS+GLONASS+Galileo
          4  GPS+GLONASS
          5  GPS+BeiDou锛坰upport�??
          6  GPS+Galileo
          7  BeiDou only锛坰upport�??
 * @return 
 *        QL_GNSS_INVALID_PARAM_ERR
 *        QL_GNSS_SUCCESS
 */
ql_errcode_gnss ql_gnss_sys_type_cfg(uint32 sys_type);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_GNSS_H */


