#ifndef _CG_HTTP_IF_H_
#define _CG_HTTP_IF_H_

#include "osi_api.h"
//#include "csw_debug.h"

#define SOMAXCONN 0xff

#define SEV_DEBUG_L6 256
#define SEV_DEBUG_L6_S "str1" //modify as "httpmu"...etc
#define SEV_DEBUG_L7 512
#define SEV_DEBUG_L7_S "str2"
#define SEV_DEBUG_L8 1024
#define SEV_DEBUG_L8_S "str3"
#define SEV_DEBUG_L9 2048
#define SEV_DEBUG_L9_S "str4"

//#define mupnp_log_info(format,...)  http_log_print(SEV_INFO, __FILE__, __LINE__, __PRETTY_FUNCTION__, format)

//#define open(a,b) FS_Open((a), (b), 0)
//#define read(a,b,c) FS_Read((a), (b), (c))
//#define close(a) FS_Close((a))

#endif