
#-------------------------------------------------------------------------------
# Configure variable
#-------------------------------------------------------------------------------
TOP_DIR:=${TOP_DIR}
MAKEFILE_PATH :=$(subst $(strip \),/,$(abspath $(lastword $(MAKEFILE_LIST))))

#-------------------------------------------------------------------------------
# Configure source code files
#-------------------------------------------------------------------------------
SRC_FILES+= \
	core\utils\core_auth.c \
	core\utils\core_diag.c \
	core\utils\core_global.c \
	core\utils\core_http.c \
	core\utils\core_sha256.c \
	core\utils\core_string.c \
	core\utils\core_log.c \
	core\aiot_http_api.c \
	core\aiot_mqtt_api.c \
	core\aiot_state_api.c \
	core\sysdep\core_sysdep.c \
	external\ali_ca_cert.c \
	portfiles\adapt_ql_port.c \
	components\bootstrap\aiot_bootstrap_api.c \
	components\data-model\aiot_dm_api.c \
	components\devinfo\aiot_devinfo_api.c \
	components\logpost\aiot_logpost_api.c \
	components\ntp\aiot_ntp_api.c \
	components\ota\aiot_ota_api.c \
	components\ota\ota_md5.c \
	components\shadow\aiot_shadow_api.c \
	components\diag\aiot_diag_api.c \
	components\das\src\core\proto\lsoc.pb.c \
	components\das\src\core\proto\pb_common.c \
	components\das\src\core\proto\pb_decode.c \
	components\das\src\core\proto\pb_encode.c \
	components\das\src\core\das_core.c \
	components\das\src\core\das_attest.c \
	components\das\src\core\sha1.c \
	components\das\src\core\utils.c \
	components\das\src\service\service.c \
	components\das\src\service\service_lwip_nfi.c \
	components\das\src\service\service_rom.c \
	components\das\src\service\service_sys.c \
	components\das\aiot_das_api.c \
	components\dynreg\aiot_dynreg_api.c \
	components\dynreg-mqtt\aiot_dynregmq_api.c \
	components\subdev\aiot_subdev_api.c \
	

#-------------------------------------------------------------------------------
# the include directories
#-------------------------------------------------------------------------------
INC_DIRS+= \
  -I${TOP_DIR} \
  -I${TOP_DIR}\common\include \
  -I${TOP_DIR}\third_party\LinkSDK\core \
  -I${TOP_DIR}\third_party\LinkSDK\core\utils \
  -I${TOP_DIR}\third_party\LinkSDK\core\sysdep \
  -I${TOP_DIR}\third_party\LinkSDK\components\bootstrap \
  -I${TOP_DIR}\third_party\LinkSDK\components\data-model \
  -I${TOP_DIR}\third_party\LinkSDK\components\devinfo \
  -I${TOP_DIR}\third_party\LinkSDK\components\logpost \
  -I${TOP_DIR}\third_party\LinkSDK\components\ntp \
  -I${TOP_DIR}\third_party\LinkSDK\components\ota \
  -I${TOP_DIR}\third_party\LinkSDK\components\shadow \
  -I${TOP_DIR}\third_party\LinkSDK\components\diag \
  -I${TOP_DIR}\third_party\LinkSDK\components\das \
  -I${TOP_DIR}\third_party\LinkSDK\components\das\inc \
  -I${TOP_DIR}\third_party\LinkSDK\components\das\inc\das \
  -I${TOP_DIR}\third_party\LinkSDK\components\das\src\board\freertos \
  -I${TOP_DIR}\third_party\LinkSDK\components\dynreg \
  -I${TOP_DIR}\third_party\LinkSDK\components\dynreg-mqtt \
  -I${TOP_DIR}\third_party\LinkSDK\components\subdev \
  -I${TOP_DIR}\third_party\mbedtls\include \
  -I${TOP_DIR}\network\sockets\inc \
  -I${TOP_DIR}\network\data_call\inc \
  -I${TOP_DIR}\fs\inc \
  
  

#-------------------------------------------------------------------------------
# Configure compile flag for C
#-------------------------------------------------------------------------------
U_FLAGS+= \

#-------------------------------------------------------------------------------
# Configure compile flag for C++
#-------------------------------------------------------------------------------
U_CPPFLAGS+=

#-------------------------------------------------------------------------------
# Configure default macro define
#-------------------------------------------------------------------------------
DFLAGS+= \
  -DCMUX_ENABLE \
  -DLWIP_IPNETBUF_SUPPORT \
  -D_TAVOR_B0_SILICON_ \
  -DFEATURE_SHMEM \
  -D"NO_APLP=0" \
  -DL1_RX_DIV_SUPPORT \
  -DRUN_XIP_MODE \
  -DENABLE_MAC_TX_DATA_LOGGING \
  -DINTEL_UPGRADE_EGPRS_M \
  -DNO_OPENSSL \
  -DPLAT_TEST \
  -DYMODEM_EEH_DUMP \
  -DCAPT_PARAMS_OPTIMIZE \
  -D_DDR_INIT_ \
  -DSILICON_TTC_CORE_SEAGULL \
  -DUPGRADE_EGPRS_M \
  -DTV_FNAME="\"SW_PLATFORM=PMD2NONE PHS_SW_DEMO PHS_SW_DEMO_PM SRCNUCLEUS FULL_SYSTEM NOACRTC PDFLT PLAT_TEST PV2 DIAGOSHMEM NVM WITHL1V\"" \
  -DFLAVOR_DUALCORE \
  -DCODE_IN_PSRAM  \
  -DTAVOR_D2_WB_L1_SUPPORT \
  -DCRANE_MCU_DONGLE \
  -DPHS_L1_SW_UPDATE_R7 \
  -DINTEL_HERMON_SAC \
  -DUPGRADE_PLMS_3G \
  -DPM_DEBUG_MODE_ENABLED \
  -DINTEL_UPGRADE_R99 \
  -DENV_XSCALE \
  -DEXT_AT_MODEM_SUPPORT \
  -DFRBD_CALIB_NVM \
  -DUPGRADE_LTE \
  -DPLAT_USE_THREADX \
  -DFG_PLMS_URR \
  -DNO_BIP \
  -DL1_UPGRADE_R7 \
  -DDIAG_NEWPP \
  -DCA_LONG_IPC_MSG   \
  -DPCAC_INCLUDE \
  -DHL_LWG \
  -DTDL1C_SPY_ENABLE \
  -DMRAT_NAS \
  -DL1_SW_UPDATE_FOR_DIGRF       \
  -DPPP_ENABLE \
  -DCRANE_SD_NOT_SUPPORT \
  -DAPLP_SPY_ENABLE \
  -DMIPS_TEST_RAM \
  -D_DDR_INIT_  \
  -DUPGRADE_PLMS_SEARCH_API \
  -DINTEL_UPGRADE_RF_PARAMS_IN_CF_TDS \
  -DNTP \
  -DINTEL_UPGRADE_GSM_CRL_IF \
  -DUPGRADE_PLMS_L1 \
  -DENABLE_CAT1_LG \
  -DURR_MRAT_ICS_SEARCH \
  -DPHS_SW_DEMO \
  -DINTEL_UPGRADE_UNIFIED_VOICE_TASK \
  -DTV_FDESC="\"SW_DESCRIPTION=\"" \
  -DUSE_TTPCOM_CSR_BLUETOOTH_AUDIO_GAIN_CONTROL \
  -DFLAVOR_DDR12MB_GB1MB5 \
  -DUPGRADE_R4_FS1 \
  -DCRANE_CUST_BUILD \
  -DL1_UPGRADE_R5 \
  -DINTEL_2CHIP_PLAT \
  -DINTEL_UPGRADE_EE_HANDLER_SUPPORT \
  -DLFS_FILE_SYS \
  -DUPGRADE_FG_PLMS \
  -DNO_MBEDTLS \
  -DPHS_SW_DEMO_TTC_PM \
  -DCRANE_CAMERA_SUPPORT \
  -DPM_D2FULL_MODE \
  -DL1_WIFI_LOCATION \
  -DSUPPORT_GPLC2_RSSI_WITH_L1A1 \
  -DINTEL_UPGRADE_DUAL_RAT \
  -DL1V_NEW_RSSI \
  -DENABLE_ACIPC \
  -DL1_SW_UPDATE_FOR_DIGRF \
  -D_DIAG_USE_COMMSTACK_ \
  -DLTE_GSMMULTIBCCH \
  -DLTE_DSDS_GSMMULTIBCCH \
  -DUPGRADE_DSDS \
  -DTAVOR \
  -DSILICON_PV2 \
  -DNO_XML \
  -DRUN_WIRELESS_MODEM \
  -DDEBUG_D2_MOR_REG_RESEREVED_ENABLE \
  -DUPGRADE_PLMS_STAGE_2 \
  -DFRBD_DSDS_L1 \
  -DI_2CHIP_PLAT \
  -DMAP_NSS \
  -DNEZHA3_1826 \
  -DCRANE_Z2 \
  -DUPGRADE_DIGRF3G_SUPPORT \
  -DFULL_SYSTEM \
  -DCRANE_Z1 \
  -DUPGRADE_ENHANCED_QUAD_BAND \
  -DRELIABLE_DATA \
  -D_DATAOMSL_ENABLED_ \
  -DNEZHA3 \
  -DMSL_INCLUDE \
  -DSULOG_DISABLE \
  -DNO_PAHO_MQTT \
  -DL1_UPGRADE_R8 \
  -DUPGRADE_ARBEL_PLATFORM \
  -DINTEL_UPGRADE_GPRS_CIPHER_FLUSH \
  -DL1_DDR_HIGH_FREQ \
  -DENABLE_OOS_HANDLING \
  -DUPGRADE_HERMON_DUAL \
  -DFLAVOR_COM \
  -DUPGRADE_MBCCH \
  -DMP3_DECODE \
  -D_FDI_USE_OSA_ \
  -DREL5_CLEANUP_FEATURE \
  -DW_PS_PLUS_G_PAGING \
  -DL1_FEATURE_CPC_CRX \
  -DHSPA_MPR \
  -DACIPC_ENABLE_NEW_CALLBACK_MECHANISM \
  -DOSA_USED \
  -DFEATURE_WB_AMR_PS \
  -D_TAVOR_HARBELL_ \
  -DSILICON_SEAGULL \
  -DUPGRADE_DSDSLTE \
  -DUPGRDE_TAVOR_COMMUNICATION \
  -DOPTIMIZE_FOR_2G_BCCH \
  -DAT_OVER_UART \
  -DINTEL_UPGRADE_2SAMPLES_PER_SYMBOL \
  -DUPGRADE_PLMS \
  -DPM_EXT_DBG_INT_ARR \
  -DNO_LWM2M \
  -D"GPRS_MULTISLOT_CLASS=12" \
  -DMULTI_BCCH_READY_IND \
  -DMARVELL_UPGRADE_BSIC_REDESIGN \
  -DOSA_QUEUE_NAMES \
  -DICS_MBCCH_2G_RSSI \
  -DDLM_TAVOR \
  -DCHARGE_DM \
  -DL1_SW_UPDATE_FOR_DIGRF     \
  -DGPLC_LTE_RSSI_SCAN \
  -D_TAVOR_DIAG_ \
  -DDISABLE_NVRAM_ACCESS \
  -DOSA_NUCLEUS \
  -D__TARGET_FEATURE_DOUBLEWORD \
  -DFLAVOR_DIET_RAM \
  -DICS_MBCCH \
  -DUPGRADE_ICS \
  -DFRBD_FDT_CALIB \
  -DL1_UPGRADE_R6 \
  -DUPGRADE_L1A_FG_PLMS \
  -DNVM_INCLUDE \
  -DFRBD_AGC_CALIB \
  -DPSM_ENABLE \
  -DWHOLE_UMTS_STACK  \
  -DMIPS_TEST \
  -DMACRO_FOR_LWG \
  -DPHS_SW_DEMO_TTC \
  -DMSL_POOL_MEM \
  -DGPIO_TRACE \
  -DUPGRADE_PLMS_SR \
  -DUSB_CABLE_DETECTION_VIA_PMIC \
  -DUPGRADE_EDGE \
  -DNO_DIALER \
  -D"EGPRS_MULTISLOT_CLASS=12" \
  -DCRANE_LCD_SUPPORT \
  -DL1_DUAL_MODE \
  -DPM_D2NONE_MODE \
  -D_DIAG_DISABLE_USB_ \
  -DLTE_W_PS \

#-------------------------------------------------------------------------------
# Configure link library
#-------------------------------------------------------------------------------
U_LIBS:=

#-------------------------------------------------------------------------------
# include mk file
#-------------------------------------------------------------------------------
include ${TOP_DIR}\config\common\makefile.mk
