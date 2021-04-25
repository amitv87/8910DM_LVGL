# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

#configure_file(include/lv_gui_config.h.in ${out_inc_dir}/lv_gui_config.h)

set(target ql_ali_linkSDK)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_app_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_ALIYUN)
target_include_directories(${target} PUBLIC inc core core/utils core/sysdep components/bootstrap  components/data-model components/devinfo components/logpost  components/ntp components/ota  components/shadow components/diag components/das components/das/inc components/das/inc/das components/das/src/board/freertos components/dynreg  components/dynreg-mqtt components/subdev include )
#target_link_libraries(${target} PRIVATE kernel driver hal ql_api_common)
target_sources(${target} PRIVATE
   	core/utils/core_auth.c 
	core/utils/core_diag.c 
	core/utils/core_global.c 
	core/utils/core_http.c 
	core/utils/core_sha256.c
	core/utils/core_string.c 
	core/utils/core_log.c 
	core/aiot_http_api.c 
	core/aiot_mqtt_api.c 
	core/aiot_state_api.c 
	core/sysdep/core_sysdep.c 
	external/ali_ca_cert.c 
	components/bootstrap/aiot_bootstrap_api.c 
	components/data-model/aiot_dm_api.c 
	components/devinfo/aiot_devinfo_api.c 
	components/logpost/aiot_logpost_api.c 
	components/ntp/aiot_ntp_api.c 
	components/ota/aiot_ota_api.c 
	components/ota/ota_md5.c 
	components/shadow/aiot_shadow_api.c 
	components/diag/aiot_diag_api.c 
	components/das/src/core/proto/lsoc.pb.c 
	components/das/src/core/proto/pb_common.c 
	components/das/src/core/proto/pb_decode.c 
	components/das/src/core/proto/pb_encode.c 
	components/das/src/core/das_core.c 
	components/das/src/core/das_attest.c 
	components/das/src/core/sha1.c 
	components/das/src/core/utils.c 
	components/das/src/service/service.c 
	components/das/src/service/service_lwip_nfi.c 
	components/das/src/service/service_rom.c 
	components/das/src/service/service_sys.c 
	components/das/aiot_das_api.c 
	components/dynreg/aiot_dynreg_api.c 
	components/dynreg-mqtt/aiot_dynregmq_api.c 
	components/subdev/aiot_subdev_api.c 
	portfiles/adapt_ql_port.c 
	demos/mqtt_basic_demo.c
)
relative_glob(srcs include/*.h src/*.c inc/*.h)
beautify_c_code(${target} ${srcs})