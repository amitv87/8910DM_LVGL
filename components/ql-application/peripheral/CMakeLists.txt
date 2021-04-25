# Copyright (C) 2020 QUECTEL Technologies Limited and/or its affiliates("QUECTEL").
# All rights reserved.
#

set(target ql_app_peripheral)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_app_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_QUEC)
target_include_directories(${target} PUBLIC inc)
#target_link_libraries(${target} PRIVATE  ql_api_common)

target_sources(${target} PRIVATE
	gpio_demo.c
	gpio_int_demo.c
	adc_demo.c
	led_cfg_demo.c
	uart_demo.c
)
target_sources_if(QL_APP_FEATURE_KEYPAD THEN ${target} PRIVATE keypad_demo.c)

relative_glob(srcs include/*.h src/*.c inc/*.h)
beautify_c_code(${target} ${srcs})