/****************************************************************************************
** File Name:		calibration.h															*	
** Author:		Fancier fan															*
** DATE:			12/13/2002															*
** CopyRight		2001 SpreadTrum Communications CORP. All Rights Reserved.			*
** Description:		This is the head file for main function of Physical Layer			*
					It reads messages from Layer1 queue, and then calls a right function*
					to process them. It may maintain a Layer1 state machine.			*
*****************************************************************************************


*****************************************************************************************
**					Edit History														*	
** -------------------------------------------------------------------------------------*
**																						*
** DATE:			NAME				DESCIPTION										*
** 12/13/2002	Fancier fan			Create.											*
*****************************************************************************************/

#ifndef _CALIBRATION_H
#define _CALIBRATION_H


//#include "sci_types.h"
//#include "sci_api.h"
//#include "tasks_id.h"
//#include "os_api.h"

/**---------------------------------------------------------------------------*
 **                         Data Structures                                   *
 **---------------------------------------------------------------------------*/
typedef uint16	CALI_ARFCN_T;

//#define	MAX_POWERMONQ_SIZE				30	//20
#define MAX_BLOCK_LENGTH				23
// msg define
/*
typedef enum
{
	DIAG_CALIBRATION_REQ =	0xff00,
	DIAG_CALIBRATION_CNF
} TOOL_MPH_SIGNAL_LIST_E;
*/
#define MAX_MPH_RXLEV_ARFCN_NUM		 	8
#define MAX_CALI_BA_SIZE				64
#define MAX_CALI_ARFCN_SIZE				548
#define SINGLE_EDGE_RAMP_POINTS_NUM		16
#define MAX_RAMP_POINTS_NUM				SINGLE_EDGE_RAMP_POINTS_NUM*2
#define MAX_TX_LEVEL_NUM				16
#define MAX_BAND_NUM					10
#define MAX_NUM_RAMP_POINTS     		20          //points of ramp up and down totally

#define FASTCALI_MAX_SLOTNUM       (4)

typedef enum	
{
	CALI_EGSM900,
	CALI_DCS1800,
	CALI_EGSM_DCS_DUALBAND,
	CALI_PCS1900,
	CALI_GSM850,
  	CALI_EGSM_PCS_DUALBAND, 
	CALI_GSM850_DCS_DUALBAND,
	CALI_GSM850_PCS_DUALBAND,
	CALI_GSM850_EGSM_DUALBAND,
	CALI_GSM850_EGSM_PCS_TRIBAND,
	CALI_GSM850_EGSM_DCS_TRIBAND
} CALI_MS_BAND_E;


//enum of parameter or operation types in mcft
typedef enum                                 
 {                                           
	CALI_CALIBRATION_ALL = 0X0,              
	CALI_ADC,                       //01     
	CALI_AFC_CENTER,                //02     
	CALI_AFC_HZ_6BITS,              //03     
	CALI_AFC_SLOPE,                 //04     
	CALI_AFC_SLOPE_LEN,             //05     
	CALI_AFC_CDAC,                  //06     
	CALI_AFC_CAFC,                  //07     
	CALI_RAMP_TABLE,                //08     
	CALI_RAMP_TABLE_CONST_UP,       //09     
	CALI_RAMP_TABLE_CONST_DN,       //0A     
	CALI_RAMP_UP_PARAM_NUM,         //0B     
    CALI_RAMP_DELTA_TIMING,         //0C
	CALI_RAMP_DUARATOIN,             //0D
	CALI_AFC_RANGE,                 //0E     
	CALI_AFC_SLOPE_RECI,            //0F     
	CALI_TX_COMP_ARFCN_INDEX,       //10     
	CALI_RX_COMP_ARFCN_INDEX,       //11     
	CALI_RX_COMP_VALUE,             //12     
	CALI_TX_ARFCN,                  //13     
	CALI_TX_PCL,                    //14     
	CALI_TX_PWR_STEP_FACTOR,        //15     
	CALI_RX_ARFCN,                  //16     
	CALI_AGC_CRTL_WORD,             //17     
	CALI_MAX_RF_GAIN_INDEX,         //18     
	CALI_RSSI,                      //19
	CALI_RXLEV,                     //1A    
	CALI_ACTIVE_DEACTIVE,	        //1B    
	CALI_TX_ON_OFF, 		        //1C    
	CALI_RX_ON_OFF,	                //1D    
	CALI_IMEI,                      //1E    
	CALI_CHIP_ID,                   //1F	         
	CALI_SAVE_TO_NV,                 //20 
	CALI_RF_SOLUTION,				//21       
	CALI_MODE_READY,			//22    refered by ref ucom cr108228               
	CALI_IMEI1,       //23
	//for MCS5-9
	CALI_8PSK_TX_PWR_STEP_FACTOR = 0x24,  //0x24
	CALI_8PSK_TX_PWR_TX_GAIN,             //0x25
	CALI_8PSK_RAMP_DELTA_TIMING,          //0x26
	CALI_8PSK_RAMP_DUARATOIN,             //0x27  
	CALI_NOSINGAL_RXTXLOOP = 0x28,  //0x28 (value defined by tools team)
	CALI_FASTCALI_MODE=0x2D,               // FAST DEVICE TURN MODE

	CALI_OSCI_TEMP_COM_IN_DAC = 0x30,      
       CALI_TEMPERATURE_GSM_TX_COMPENSATE,  // 0x31
       CALI_TEMPERATURE_EDGE_TX_COMPENSATE,    // 0x32
       CALI_TEMPERATURE_GSM_TXPOWER_COMPENSATE, // 0x33
       CALI_TEMPERATURE_EDGE_TXPOWER_COMPENSATE, // 0x34
       CALI_TX_COMPENSATE_BOUNDARY_ARFCN_INDEX, // 0x35
       CALI_TEMPERATURE_RX_COMPENSATE_ADC,   // 0x36
       CALI_TEMPERATURE_RX_LEVEL_COMPENSATE,  // 0x37
       CALI_TEMPERATURE_TX_COMPENSATE_ADC,    // 0x38
       CALI_TEMPERATURE_TX_RAMPWR_STEP_FACTOR, // 0x39
       CALI_RF_EDGE_DELTA_TIMING,   // 0x3a
       CALI_RF_EDGE_POWER_ON_DURATION,// 0x3b
       CALI_TEMP_SENSOR = 0x48,
       CALI_END


} CALIBRATION_PARAM_E;       

// No-Singal CFT_RX_TX Test Feature. @arthur 02/2009 MS00134792
// Command enum of rxtx-loop state(sub action reffer to sub state )
typedef enum
{
    CMD_RXTXLOOP_NULL ,//= 0X00,  used  as a state, real command should not be this value
    CMD_RXTXLOOP_INITIATE,
    CMD_RXTXLOOP_STARTLOOP,
    CMD_RXTXLOOP_STOPLOOP,
    CMD_RXTXLOOP_CHANGE_PCL, // change power level(must used in tch loopbacking state)
    CMD_RXTXLOOP_CHANGE_TCHARFCN,
    CMD_RXTXLOOP_GET_RXLEV,
    CMD_RXTXLOOP_UPDATE_CALINV,
    CMD_RXTXLOOP_MAX
} L1_RXTXLOOP_COMMAND_E;

// result enum of rxtx-loop state(sub action reffer to sub state ), to notify diag command process result.
typedef enum
{
    RET_RXTXLOOP_OK,//
    RET_RXTXLOOP_WAKEDSP_FAIL,
    RET_RXTXLOOP_MEASUREMENT_FAIL,
    RET_RXTXLOOP_SCHSEARCH_FAIL,
    RET_RXTXLOOP_SCHREAD_FAIL,
    RET_RXTXLOOP_CAMP_FAIL,
    RET_RXTXLOOP_STARTLOOP_FAIL,
    RET_RXTXLOOP_CHANGEPCL_FAIL,
    RET_RXTXLOOP_INIT_FAIL,
    RET_RXTXLOOP_CHANGEARFCN_FAIL,
    RET_RXTXLOOP_EXCEPTION,    
    RET_RXTXLOOP_GETRXLEV_FAIL,
    RET_RXTXLOOP_MAX
} L1_RXTXLOOP_RESULT_E;             
                                    
//struct for cm to send operation request to diag
typedef struct
{
	BOOLEAN is_nv;
	CALI_MS_BAND_E band;
	CALIBRATION_PARAM_E  type;
	uint16  index;
	uint16 length;
} TOOL_DIAG_CALI_PARAM_T;

#define TOOL_DIAG_CALI_HEAD_SIZE (5*sizeof(uint16))//used for calculate the offset 

// struct for Diag to send read request to layer1
typedef struct
{
	SIGNAL_VARS
	CALI_MS_BAND_E band;
	CALIBRATION_PARAM_E  type;
	uint16  index;
	uint16 length;
}DIAG_L1_RD_CALI_PARAM_REQ_T;


// struct for Diag to send  request of NoSingal rxtx loopback to layer1
typedef struct //No-Singal CFT_RX_TX Test Feature. @arthur 02/2009 MS00134792
{
    CALI_ARFCN_T  arfcn;      
    uint16  traffic_tn;
    uint8  tsc;
    uint8  power_level;
    LOOP_LIST_E mode;
    L1_RXTXLOOP_COMMAND_E  command;    
    CALI_MS_BAND_E band;  
    uint16 dummy0;
    uint16 dummy1;
}CALI_L1_RXTXLOOP_PARAM_T;

#define OFFSET_BY_BYTE_CALI_L1_RXTXLOOP_PARAM  (sizeof(uint16))

// struct for Diag to send  request of NoSingal rxtx loopback to layer1
typedef struct //No-Singal CFT_RX_TX Test Feature. @arthur 02/2009 MS00134792
{
    SIGNAL_VARS
    CALI_L1_RXTXLOOP_PARAM_T param;
}DIAG_L1_NOSIGAL_RXTXLOOP_REQ_T;


// struct for l1 to send  confirm to diag(pc tool)
typedef struct //No-Singal CFT_RX_TX Test Feature. @arthur 02/2009 MS00134792
{
    L1_RXTXLOOP_RESULT_E ret;
    uint16  rx_lev;      
    uint16  rx_qual;
    uint16  dummy0;      
}L1_CALI_RXTXLOOP_PARAM_T;

#define NUM_CALI_L1_RXTXLOOP_PARAM  (4)

// confirm singal of DIAG_L1_NOSIGAL_RXTXLOOP_PARAM_REQ_T
typedef struct //No-Singal CFT_RX_TX Test Feature. @arthur 02/2009 MS00134792
{
    SIGNAL_VARS
    L1_CALI_RXTXLOOP_PARAM_T param;    
}L1_DIAG_NOSIGAL_RXTXLOOP_CNF_T;

//fast ms calibration structure  define @arthur begin
typedef enum
{
    FASTCALI_SET_TX_PARAM = 0,
    FASTCALI_SET_RX_PARAM,
    FASTCALI_SET_AFC_PARAM,
    FASTCALI_START_TX,
    FASTCALI_START_RX,
    FASTCALI_START_AFC,
    FASTCALI_INVALID_CMD
}FASTCALI_CMD_E;

typedef enum
{
    RX_SLOT_TYPE_FCB,  // FCCH
    RX_SLOT_TYPE_FSB,  // SCH
    RX_SLOT_TYPE_PRBS, //
    RX_SLOT_TYPE_DUMMY //DUMMY
}FASTCALI_RX_SLOT_TYPE_E;

typedef enum
{
    FASTCALI_RET_SUC = 0,
    FASTCALI_RET_FAIL,
    FASTCALI_RET_PARAM_ERR,
    FASTCALI_RET_NORXFRM_ERR,
    FASTCALI_RET_SCH_FAIL
}FASTCALI_RET_E;	

typedef enum
{
    FASTCALI_SUB_NULL= 0,
    FASTCALI_SUB_TX,
    FASTCALI_SUB_SEARCH_READY,
    FASTCALI_SUB_SEARCHING,
    FASTCALI_SUB_SEARCH_END,
    FASTCALI_SUB_READ_READY,
    FASTCALI_SUB_READING,
    FASTCALI_SUB_READ_END,
    FASTCALI_SUB_RX,
    FASTCALI_SUB_AFC,
    FASTCALI_SUB_MAX
}FASTCALI_SUBSTATE_E;	


typedef struct
{
	CALI_MS_BAND_E band; // band of arfcn
	uint16   arfcn;        
	uint8     slot_mask;  // (BIT MAP) which slot of 0-7 is setted with power
	uint8     slot_num;    // how many slot is setted with power
	uint16   slot_power_factor[FASTCALI_MAX_SLOTNUM]; // factor value for each slot of one frame
	uint16   slot_pcl[FASTCALI_MAX_SLOTNUM];  // PCL value for each slot of one frame
	uint16   slot_power_ind[FASTCALI_MAX_SLOTNUM]; // index of RF_ramppwr_step_fac[] to to set with slot_power_factor[] value
}FASTCALI_TX_PARAM_T;

typedef struct
{
	CALI_MS_BAND_E band;  // band
	uint16   arfcn;         // arfcn
	uint8     slot_mask;  // (BIT MAP) which slot of 0-7 is setted with power
	uint8     slot_num;    // how many slot is setted with power
	uint16   slot_gain_val[FASTCALI_MAX_SLOTNUM];   // slot gain value for each slot in one frame
	uint16   slot_gain_index[FASTCALI_MAX_SLOTNUM]; // slot gain index for each slot in one frame
	FASTCALI_RX_SLOT_TYPE_E slot_type[FASTCALI_MAX_SLOTNUM]; // slot type for each slot in one frame
}FASTCALI_RX_PARAM_T;

typedef struct
{
	CALI_MS_BAND_E band;
	uint16   arfcn;
	uint8     slot_mask;  // (BIT MAP) which slot of 0-7 is setted with power
	uint8     slot_num;    // how many slot is setted with power
	uint16   slot_cdac[FASTCALI_MAX_SLOTNUM];
	uint16   slot_cafc[FASTCALI_MAX_SLOTNUM];
}FASTCALI_AFC_PARAM;

typedef struct
{
	uint16 cmd; // command of signal
	uint16 p1;       //reserved
       uint16 p2;       //reserved
       uint16 p3;      // reserved
       uint16 p4;      //reseved
	uint16 count; // size of param[]
	void * param_ptr;  // pointer  to a array of  FASTCALI_RX_PARAM_T or FASTCALI_TX_PARAM_T
}FASTCALI_DIAG2L1_PARAM_T;

// 
typedef struct
{
	FASTCALI_CMD_E cmd; // command of signal	
       uint16 p1;       //reserved
       uint16 p2;       //reserved
       uint16 p3;      // reserved
       uint16 p4;      //reseved
       uint16 count; // size of param[]
	FASTCALI_TX_PARAM_T * frames_set_ptr;  // pointer  to a array of FASTCALI_TX_PARAM_T
}FASTCALI_TX_FRAMES_SET_T;

typedef struct
{
	FASTCALI_CMD_E cmd; // command of signal
	uint16 p1;       //fn_start : start fn of test frames.
       uint16 p2;       //sync_timeout : 60ms as unit
       uint16 p3;      // reserved
       uint16 p4;      //reseved
	uint16 count; // size of param[]
	FASTCALI_RX_PARAM_T * frames_set_ptr;  // pointer  to a array of FASTCALI_RX_PARAM_T
}FASTCALI_RX_FRAMES_SET_T;

typedef struct
{
	FASTCALI_CMD_E cmd; // command of signal
	uint16 p1;       //reserved
       uint16 p2;       //reserved
       uint16 p3;      // reserved
       uint16 p4;      //reseved
	uint16 count; // size of param[]
	FASTCALI_AFC_PARAM * frames_set_ptr;  // pointer  to a array of FASTCALI_AFC_PARAM
}FASTCALI_AFC_FRAMES_SET_T;

typedef struct
{
    SIGNAL_VARS
    FASTCALI_DIAG2L1_PARAM_T param;    
}FASTCALI_DIAG2L1_SIG_T;

typedef struct
{
    FASTCALI_RET_E err; // result of RX/TX process
    uint16 count;  // size of array rssi_va_ptr[]
    int32  * rssi_va_ptr; // rssi value each slot
}FASTCALI_L12DIAG_PARAM_T;

typedef struct
{
    SIGNAL_VARS
    FASTCALI_L12DIAG_PARAM_T param;   
}FASTCALI_L12DIAG_SIG_T;

typedef struct{
	uint16 	arfcn;
	uint8  	slot_mask; 	// which slot 's rxleve and rssi is valid.
	uint8         slot_num;
	uint16	rxlev[8];				
	int16         rssi[8];
}FASTCALI_FRAME_RXLEV_T;

typedef struct{
      	uint16 	arfcn;
	uint16 rxlev;
	int16  rssi;
}FASTCALI_SLOT_RXLEV_T;


// struct for layer1 to send read confirm to diag
typedef struct
{
	SIGNAL_VARS
	CALI_MS_BAND_E band;
	CALIBRATION_PARAM_E  type;
	BOOLEAN operation_status;
	uint16 *read_data_addr;
	uint16 length;
} L1_DIAG_RD_CALI_PARAM_CNF_T;

// struct for Diag to send write request to layer1
typedef struct
{
	SIGNAL_VARS
	CALI_MS_BAND_E band;
	CALIBRATION_PARAM_E  type;
	uint16  index;
	uint16 * parameter;
	uint16 length;
} DIAG_L1_WR_CALI_PARAM_REQ_T;

// struct for layer1 to send write confirm to diag
typedef struct
{
	SIGNAL_VARS
	CALI_MS_BAND_E band;
	CALIBRATION_PARAM_E  type;
	BOOLEAN operation_status;
	uint16 length ;
} L1_DIAG_WR_CALI_PARAM_CNF_T;

#if defined(PLATFORM_UIX8910)
// struct for CP to send write confirm to diag
typedef struct
{
	SIGNAL_VARS
    uint8  subType;
	uint16 length ;
    uint8 *data;
} DIAG_CALI_CMD_CNF_T;
#endif

typedef struct
{
	SIGNAL_VARS
}CALI_TX_REQ_T;

typedef struct
{
	SIGNAL_VARS
}CALI_EDGE_TX_REQ_T;


typedef struct
{
	SIGNAL_VARS
	
	
}CALI_RX_REQ_T;

typedef struct
{
	SIGNAL_VARS
}CALI_RACH_REQ_T;


typedef struct
{
	SIGNAL_VARS
	
}CALI_RSSI_REQ_T;

typedef CALI_ARFCN_T  CALI_ARFCN_LIST_ARR_T[MAX_CALI_BA_SIZE];

typedef struct CALI_ARFCN_LIST_T_s {
    uint16  				num;
    CALI_ARFCN_LIST_ARR_T   arfcn_list_arr;
} CALI_ARFCN_LIST_T;

typedef uint8	CALI_BLOCK_ARR_T[MAX_BLOCK_LENGTH];
typedef uint8 	CALI_RXLEV_ARR_T[MAX_CALI_ARFCN_SIZE];

typedef enum _TOOL_MPH_MSG_ID_E
{
	TOOL_MPH_ACTIVE_REQ     = 0xa101,
	TOOL_MPH_DEACTIVE_REQ ,
	TOOL_MPH_CLASSMARK_REQ,
	TOOL_MPH_RXPOWER_SWEEP_REQ   ,
	TOOL_MPH_RXPOWER_SWEEP_CNF   ,     	     
	TOOL_MPH_BCCH_DECODE_REQ ,
	TOOL_MPH_BCCH_INFO_IND      ,            
	TOOL_MPH_BCCH_STOP_REQ 	  ,
	TOOL_MPH_BSIC_DECODE_REQ  ,     
	TOOL_MPH_BSIC_DECODE_CNF  ,    
	TOOL_MPH_BSIC_DECODE_FAIL  ,  
	TOOL_MPH_DSP_MEMORY_WRITE_REQ,
	TOOL_MPH_DSP_MEMORY_READ_REQ,
	TOOL_MPH_DSP_VERSION_REQ,
	TOOL_MPH_DSP_MEMORY_READ_CNF,
	TOOL_MPH_DSP_VERSION_CNF,
	TOOL_MPH_DSP_TXRX_REQ,
	TOOL_MPH_STOP_DSP_TXRX_REQ,
	TOOL_MPH_DSP_RXLEV_CNF,
	TOOL_MPH_DAC_REQ,
	TOOL_MPH_STOP_DAC_REQ,
	TOOL_MPH_AFC_REQ,
	TOOL_MPH_AGC_OFFSET_REQ,
	TOOL_MPH_POWER_RAMP_REQ,
	TOOL_MPH_POWER_STEP_FACTOR_REQ,
	TOOL_MPH_PA_PARA_REQ,
	TOOL_MPH_BATTERY_REQ,
	TOOL_MPH_BATTERY_CNF,
	TOOL_MPH_RF_CONTROL_REQ,
	TOOL_MPH_DSP_RX_REQ,
	TOOL_MPH_STOP_DSP_RX_REQ,
	//@Shijun.cui add for factory product test
	TOOL_MPH_DSP_RXLEV_QUERY,
	TOOL_MPH_DSP_TXLEV_QUERY,
	TOOL_MPH_DAC_QUERY,
	TOOL_MPH_CLASSMARK_QUERY
} TOOL_MPH_MSG_ID_E;

typedef enum _DSP_TX_TYPE_E
{
	DSP_TX_TYPE_ALL_ZERO,
	DSP_TX_TYPE_ALL_SET,
	DSP_TX_TYPE_RANDOM,
	DSP_TX_TYPE_RACH,
	//@meiwen.cao for edge tx
	DSP_TX_TYPE_EDGE,
	DSP_TX_TYPE_CW
} DSP_TX_TYPE_E;

typedef enum _TOOL_FREQ_LEVEL_E
{
  TOOL_FREQ_LEVEL_LOWER,
  TOOL_FREQ_LEVEL_LOW,
  TOOL_FREQ_LEVEL_MIDDLE,
  TOOL_FREQ_LEVEL_HIGH,
  TOOL_FREQ_LEVEL_HIGHER
}TOOL_FREQ_LEVEL_E;
/*
typedef struct
{
	CALI_ARFCN_T		arfcn;
	uint16				rxlev;
	uint16				valid;	// 1 good, 2 saturate hi, 3 below noise 
} PWR_MON_RESULT_T;
*/

typedef struct 
{
	SIGNAL_VARS
		
	uint32					is_valid;
	uint16					rxlev;
	uint16					rssi;
} yPDef_TOOL_MPH_DSP_RXLEV_CNF;

typedef struct
{
	SIGNAL_VARS
	
	CALI_MS_BAND_E	band;    /* enum type: E_GSM900,DCS1800,Dualband,PCS1900 */
} yPDef_TOOL_MPH_CLASSMARK_REQ;

typedef struct 
{
	SIGNAL_VARS
		
	CALI_ARFCN_T			arfcn;	// arfcn 
	uint16 					dac_val;
	uint16					txpwr_level;
	uint16                  afc_val;

} yPDef_TOOL_MPH_DAC_REQ;

typedef struct 
{
	SIGNAL_VARS
} yPDef_TOOL_MPH_STOP_DAC_REQ;

typedef struct
{
	SIGNAL_VARS
} yPDef_TOOL_MPH_BATTERY_REQ;

typedef struct
{
	SIGNAL_VARS
		
	CALI_MS_BAND_E			band;
	CALI_ARFCN_T			arfcn;	// arfcn 
	uint16					gain_ind; // gain index for rx
	uint16					gain_val; // gain value for rx
	uint16					txpwr_level; // tx power level for tx
	DSP_TX_TYPE_E			data_type;      // data type for tx
	uint16					sample_couter;  // how much sample , to calculate 
} yPDef_TOOL_MPH_DSP_TXRX_REQ;



typedef struct
{
	SIGNAL_VARS
		
	CALI_MS_BAND_E			band;
	CALI_ARFCN_T			arfcn;	// arfcn 
	uint16					gain_ind; // gain index for rx
	uint16					gain_val; // gain value for rx
	uint16					txpwr_level; // tx power level for tx
	DSP_TX_TYPE_E			data_type;      // data type for tx
	uint16					sample_couter;  // how much sample , to calculate 
	uint16                  		coding_scheme;
	uint16                  		training_sequence;
	uint16                  		puncture_type;   
}  TX_RX_PARAM_T;


typedef struct
{
	uint16					gain_ind; // gain index for rx
	uint16					gain_val; // gain value for rx
	DSP_TX_TYPE_E			data_type;      // data type for tx
	uint16					sample_couter;  // how much sample , to calculate 
	BOOLEAN 				on_off;
	//@meiwen.cao for edge tx
	uint16                  		coding_scheme;
	uint16                  		training_sequence;
	uint16					puncture_type; 
}  TX_RX_REQ_T;




typedef struct
{
	SIGNAL_VARS
} yPDef_TOOL_MPH_STOP_DSP_TXRX_REQ;




typedef struct
{
	SIGNAL_VARS
} yPDef_TOOL_MPH_ACTIVE_REQ;

typedef struct
{
	SIGNAL_VARS
} yPDef_TOOL_MPH_DEACTIVE_REQ;

typedef struct
{
	SIGNAL_VARS
		
	
	CALI_MS_BAND_E		band;
	CALI_ARFCN_LIST_T	arfcn_list;	// ba list 
	uint32				ba_valid;
} yPDef_TOOL_MPH_RXPOWER_SWEEP_REQ;

typedef struct
{
	SIGNAL_VARS
		
//	TOOL_MPH_MSG_HEADER
	
	CALI_RXLEV_ARR_T		rxlev_arr;
} yPDef_TOOL_MPH_RXPOWER_SWEEP_CNF;

typedef struct
{
	SIGNAL_VARS

	CALI_ARFCN_T			arfcn;	// arfcn 
} yPDef_TOOL_MPH_BCCH_DECODE_REQ;

typedef struct
{
	SIGNAL_VARS

	CALI_ARFCN_T			arfcn;	// arfcn 
} yPDef_TOOL_MPH_BCCH_STOP_REQ;

typedef struct
{
	SIGNAL_VARS

	CALI_ARFCN_T			arfcn;	// arfcn 
} yPDef_TOOL_MPH_BSIC_DECODE_REQ;

typedef struct
{
	SIGNAL_VARS

	CALI_ARFCN_T			arfcn;	// arfcn 
	uint32   				is_good_block;
	CALI_BLOCK_ARR_T		block;
} yPDef_TOOL_MPH_BCCH_INFO_IND;

typedef struct
{
	SIGNAL_VARS

	CALI_ARFCN_T			arfcn;	// arfcn 
	uint8					bsic;
} yPDef_TOOL_MPH_BSIC_DECODE_CNF;

typedef struct
{
	SIGNAL_VARS
} yPDef_TOOL_MPH_BSIC_DECODE_FAIL;

typedef struct 
{
	int16 stage0[MAX_NUM_RAMP_POINTS];
	int16 stage1[MAX_NUM_RAMP_POINTS];
	int16 stage2[MAX_NUM_RAMP_POINTS];
	int16 stage3[MAX_NUM_RAMP_POINTS];
	int16 stage4[MAX_NUM_RAMP_POINTS];
	int16 stage5[MAX_NUM_RAMP_POINTS];
	int16 stage6[MAX_NUM_RAMP_POINTS];
	int16 stage7[MAX_NUM_RAMP_POINTS];
	int16 stage8[MAX_NUM_RAMP_POINTS];
	int16 stage9[MAX_NUM_RAMP_POINTS];
	int16 stage10[MAX_NUM_RAMP_POINTS];
	int16 stage11[MAX_NUM_RAMP_POINTS];
	int16 stage12[MAX_NUM_RAMP_POINTS];
	int16 stage13[MAX_NUM_RAMP_POINTS];
	int16 stage14[MAX_NUM_RAMP_POINTS];
	int16 stage15[MAX_NUM_RAMP_POINTS];

} TOOL_RF_ramp_table_T;

typedef struct 
{
	SIGNAL_VARS

	CALI_MS_BAND_E	band;
	uint16			txpwr_type; // 0 means stage 0(high),1 means stage 1(low)
	TOOL_RF_ramp_table_T  RF_ramp_table;
	
} yPDef_TOOL_MPH_POWER_RAMP_REQ;


typedef struct
{
	SIGNAL_VARS

	CALI_MS_BAND_E	band;
	uint16			txpwr_level;
	uint16 			factor;	// pwr step factor
	TOOL_FREQ_LEVEL_E    freq_level;
} yPDef_TOOL_MPH_POWER_STEP_FACTOR_REQ;

typedef struct
{
	SIGNAL_VARS

	CALI_MS_BAND_E	band;
	uint16			txpwr_level;
	uint16 			delta_time;	
	uint16			duration;
} yPDef_TOOL_MPH_PA_PARA_REQ;

typedef struct
{
	SIGNAL_VARS

	uint16			adc_val;
} yPDef_TOOL_MPH_BATTERY_CNF;


typedef enum _RF_CONTROL_TYPE_E
{
    RF_CONTROL_NONE,
    RF_CONTROL_VALUE,
    RF_CONTROL_LENGTH,
    RF_CONTROL_GPIO,            //range: 0-11
    RF_CONTROL_TX
}RF_CONTROL_TYPE_E;

typedef struct
{
	SIGNAL_VARS

    RF_CONTROL_TYPE_E rf_control_op;
    
    uint32       rf_control_value;       //control word value
	uint16       rf_control_length;      //control word length range: bits 0-5
    uint16       rf_gpio_no;             //control gpio NO range   0-11
    uint16       rf_gpio_op;             //control gpio operation  0: down 1:up
	BOOLEAN      rf_is_tx;               //control RF uart         FALSE:disable TRUE:enable	
} yPDef_TOOL_MPH_RF_CONTROL_REQ;


void SCI_StartCalibration(void);

void Audio_Loop_Enable(BOOLEAN bEnableLoop, BOOLEAN bAudioAux);

/*----------------------------------------------------------------
	FUNCATION NAME:	CAL_GetDacValue
	Description:	get current DAC value
	Notes:	
-----------------------------------------------------------------*/
PUBLIC  yPDef_TOOL_MPH_DAC_REQ CAL_GetDacValue(
        void
        );

/*----------------------------------------------------------------
	FUNCATION NAME:	CAL_GetDspRxLev
	Description:	get DSP rx level
	Notes:	
-----------------------------------------------------------------*/
PUBLIC  uint16 CAL_GetDspRxLev(
        void
        );

/*----------------------------------------------------------------
	FUNCATION NAME:	CAL_GetDspRssi
	Description:	get rssi
	Notes:	
-----------------------------------------------------------------*/
PUBLIC  uint16 CAL_GetDspRssi(
        void
        );

/*****************************************************************************/
//  Description:    used for getting the specific address of the calibration 
//					parameters in nv
//	Global resource dependence:
//  Author:         y.j.ke
//	Note:
/*****************************************************************************/    

 uint16 *CAL_GetCalibrationParamAddrInNv( CALIBRATION_PARAM_E  param_type,uint16 index, CALI_MS_BAND_E param_band);

/*****************************************************************************/
//  Description:   For getting the size of the parameter
//	Global resource dependence:
//  Author:         y.j.ke
//	Note:
/*****************************************************************************/    

uint16 CAL_GetCalibrationParamSize(CALIBRATION_PARAM_E  param_type);

/*----------------------------------------------------------------
//  Description:  get whether now we are in CFT rxtx no-signal looping mode 
//  Global resource dependence:
//  Author: 
//  Note: 
----------------------------------------------------------------*/
BOOLEAN Cali_GetIsInRXTXLOOPMode( void );

/*----------------------------------------------------------------
    FUNCATION NAME: CALI_HandlePowerSweep
    Description:       
    Notes:  
----------------------------------------------------------------*/
extern void CALI_HandlePowerSweep(uint16 arfcn,uint16 band);

/*----------------------------------------------------------------
    FUNCATION NAME: CALI_HandleActiveDeactive
    Description:     
    Notes:  
-----------------------------------------------------------------*/
extern void CALI_HandleActiveDeactive(BOOLEAN is_active);

/*****************************************************************************/
//  Description:   handle DSP_RW_RF_PARAM_CNF IN calibration STATE
//  Global resource dependence:
//                      P0	P1	            P2	                                   P3
//  Read temp sensor    4   X               0 ok,1 fail,2 waiting dsp response     temp value
//  Author: 
//  Note:
/*****************************************************************************/   
void Cali_HandleRwRfParamCnf(uint16 parameter0,
                             uint16 parameter1,
                             uint16 parameter2,
                             uint16 parameter3);

/*****************************************************************************/
//  Description:    used for getting the specific address of the calibration 
//					parameters in nv
//	Global resource dependence:
//  Author:         y.j.ke
//	Note:
/*****************************************************************************/    
extern uint16 *GetCalibrationParamAddrInNv(CALIBRATION_PARAM_E  param_type,uint16 index, CALI_MS_BAND_E param_band);

#endif /* End of _CALIBRATION_H*/

