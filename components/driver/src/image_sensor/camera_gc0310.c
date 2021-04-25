/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

//#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "osi_log.h"
#include "osi_api.h"
#include "hwregs.h"
#include <stdlib.h>
#include "hal_chip.h"
#include "drv_i2c.h"
#include "image_sensor.h"
#include "drv_camera.h"
#include "drv_names.h"
#include "osi_clock.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
#define VGA (614400)
#define QVGA (153600)

extern bool quec_camGc0310CheckId(void);

static const cameraReg_t RG_InitQvgaMIPI[] =
{
    {0xfe, 0xf0},
{0xfe, 0xf0},
{0xfe, 0x00},
{0xfc, 0x0e},
{0xfc, 0x0e},
{0xf2, 0x80},
{0xf3, 0x00}, // output_disable
{0xf7, 0x1b},
{0xf8, 0x04},
{0xf9, 0x8e},
{0xfa, 0x11},
/////////////////////////////////////////////////
///////////////////   MIPI   ////////////////////
/////////////////////////////////////////////////
{0xfe, 0x03},
{0x40, 0x08},
{0x42, 0x00},
{0x43, 0x00},
{0x01, 0x03},
{0x10, 0x84},

{0x01, 0x03},
{0x02, 0x00},
{0x03, 0x94},
{0x04, 0x01},
{0x05, 0x00},
{0x06, 0x80},
{0x11, 0x1e},
{0x12, 0x80},
{0x13, 0x02},

{0x15, 0x10},
{0x21, 0x10},
{0x22, 0x01},
{0x23, 0x10},
{0x24, 0x02},
{0x25, 0x10},
{0x26, 0x03},
{0x29, 0x02},
{0x2a, 0x0a},
{0x2b, 0x04},
{0xfe, 0x00},

/////////////////////////////////////////////////
/////////////////	CISCTL reg	 /////////////////
/////////////////////////////////////////////////
{0x00, 0x2f},
{0x01, 0x0f}, //06
{0x02, 0x04},
{0x03, 0x03},
{0x04, 0x50},
{0x09, 0x00},
{0x0a, 0x00},
{0x0b, 0x00},
{0x0c, 0x04},
{0x0d, 0x01},
{0x0e, 0xe8},
{0x0f, 0x02},
{0x10, 0x88},
{0x16, 0x00},
{0x17, 0x14},
        {0x18, 0x1a},
{0x19, 0x14},
{0x1b, 0x48},
{0x1c, 0x1c},
{0x1e, 0x6b},
{0x1f, 0x28},
{0x20, 0x8b}, //0x89 travis20140801
{0x21, 0x49},
{0x22, 0xb0},
{0x23, 0x04},
{0x24, 0x16},
{0x34, 0x20},

/////////////////////////////////////////////////
////////////////////	BLK   ////////////////////
/////////////////////////////////////////////////
{0x26, 0x23},
{0x28, 0xff},
{0x29, 0x00},
{0x32, 0x00},
{0x33, 0x10},
{0x37, 0x20},
{0x38, 0x10},
{0x47, 0x80},
{0x4e, 0x66},
{0xa8, 0x02},
{0xa9, 0x80},

/////////////////////////////////////////////////
//////////////////  ISP reg   ///////////////////
/////////////////////////////////////////////////
{0x40, 0xff},
{0x41, 0x21},
{0x42, 0xcf},
{0x44, 0x02},
{0x45, 0xa8},
{0x46, 0x02}, //sync
{0x4a, 0x11},
{0x4b, 0x01},
{0x4c, 0x20},
{0x4d, 0x05},
{0x4f, 0x01},
{0x50, 0x01},
{0x51, 0x00},  //update
{0x52, 0x78},
{0x53, 0x00},
{0x54, 0xa0},

{0x55, 0x00},   
{0x56, 0xf0},    //240
{0x57, 0x01},   
{0x58, 0x40},     //320

/////////////////////////////////////////////////
///////////////////   GAIN   ////////////////////
/////////////////////////////////////////////////
{0x70, 0x70},
{0x5a, 0x84},
{0x5b, 0xc9},
{0x5c, 0xed},
{0x77, 0x74},
{0x78, 0x40},
{0x79, 0x5f},

/////////////////////////////////////////////////
///////////////////   DNDD  /////////////////////
/////////////////////////////////////////////////
{0x82, 0x14},
{0x83, 0x0b},
{0x89, 0xf0},

/////////////////////////////////////////////////
//////////////////   EEINTP  ////////////////////
/////////////////////////////////////////////////
{0x8f, 0xaa},
{0x90, 0x8c},
{0x91, 0x90},
{0x92, 0x03},
{0x93, 0x03},
{0x94, 0x05},
{0x95, 0x65},
{0x96, 0xf0},

/////////////////////////////////////////////////
/////////////////////	ASDE  ////////////////////
/////////////////////////////////////////////////
{0xfe, 0x00},

{0x9a, 0x20},
{0x9b, 0x80},
{0x9c, 0x40},
{0x9d, 0x80},

{0xa1, 0x30},
{0xa2, 0x32},
{0xa4, 0x30},
{0xa5, 0x30},
{0xaa, 0x10},
{0xac, 0x22},

/////////////////////////////////////////////////
///////////////////   GAMMA   ///////////////////
/////////////////////////////////////////////////
{0xfe, 0x00}, //default
{0xbf, 0x08},
{0xc0, 0x16},
{0xc1, 0x28},
{0xc2, 0x41},
{0xc3, 0x5a},
{0xc4, 0x6c},
{0xc5, 0x7a},
{0xc6, 0x96},
{0xc7, 0xac},
{0xc8, 0xbc},
{0xc9, 0xc9},
{0xca, 0xd3},
{0xcb, 0xdd},
{0xcc, 0xe5},
{0xcd, 0xf1},
{0xce, 0xfa},
{0xcf, 0xff},

/////////////////////////////////////////////////
///////////////////   YCP	//////////////////////
/////////////////////////////////////////////////
{0xd0, 0x40},
{0xd1, 0x34},
{0xd2, 0x34},
{0xd3, 0x40},
{0xd6, 0xf2},
{0xd7, 0x1b},
{0xd8, 0x18},
{0xdd, 0x03},

/////////////////////////////////////////////////
////////////////////	AEC   ////////////////////
/////////////////////////////////////////////////
{0xfe, 0x01},
{0x05, 0x30},
{0x06, 0x75},
{0x07, 0x40},
{0x08, 0xb0},
{0x0a, 0xc5},
{0x0b, 0x11},
{0x0c, 0x00},
{0x12, 0x52},
{0x13, 0x38},
{0x18, 0x95},
{0x19, 0x96},
{0x1f, 0x20},
{0x20, 0xc0},
{0x3e, 0x40},
{0x3f, 0x57},
{0x40, 0x7d},
{0x03, 0x60},
{0x44, 0x02},

/////////////////////////////////////////////////
////////////////////	AWB   ////////////////////
/////////////////////////////////////////////////
{0xfe, 0x01},
{0x1c, 0x91},
{0x21, 0x15},
{0x50, 0x80},
{0x56, 0x04},
{0x59, 0x08},
{0x5b, 0x02},
{0x61, 0x8d},
{0x62, 0xa7},
{0x63, 0xd0},
{0x65, 0x06},
{0x66, 0x06},
{0x67, 0x84},
{0x69, 0x08},
{0x6a, 0x25},
{0x6b, 0x01},
{0x6c, 0x00},
{0x6d, 0x02},
{0x6e, 0xf0},
{0x6f, 0x80},
{0x76, 0x80},
{0x78, 0xaf},
{0x79, 0x75},
{0x7a, 0x40},
{0x7b, 0x50},
{0x7c, 0x0c},

{0x90, 0xc9}, //stable AWB
{0x91, 0xbe},
{0x92, 0xe2},
{0x93, 0xc9},
{0x95, 0x1b},
{0x96, 0xe2},
{0x97, 0x49},
{0x98, 0x1b},
{0x9a, 0x49},
{0x9b, 0x1b},
{0x9c, 0xc3},
{0x9d, 0x49},
{0x9f, 0xc7},
{0xa0, 0xc8},
{0xa1, 0x00},
{0xa2, 0x00},
{0x86, 0x00},
{0x87, 0x00},
{0x88, 0x00},
{0x89, 0x00},
{0xa4, 0xb9},
{0xa5, 0xa0},
{0xa6, 0xba},
{0xa7, 0x92},
{0xa9, 0xba},
{0xaa, 0x80},
{0xab, 0x9d},
{0xac, 0x7f},
{0xae, 0xbb},
{0xaf, 0x9d},
{0xb0, 0xc8},
{0xb1, 0x97},
{0xb3, 0xb7},
{0xb4, 0x7f},
{0xb5, 0x00},
{0xb6, 0x00},
{0x8b, 0x00},
{0x8c, 0x00},
{0x8d, 0x00},
{0x8e, 0x00},
{0x94, 0x55},
{0x99, 0xa6},
{0x9e, 0xaa},
{0xa3, 0x0a},
{0x8a, 0x00},
{0xa8, 0x55},
{0xad, 0x55},
{0xb2, 0x55},
{0xb7, 0x05},
{0x8f, 0x00},
{0xb8, 0xcb},
{0xb9, 0x9b},

/////////////////////////////////////////////////
////////////////////  CC ////////////////////////
/////////////////////////////////////////////////
{0xfe, 0x01},

{0xd0, 0x38}, //skin red
{0xd1, 0x00},
{0xd2, 0x02},
{0xd3, 0x04},
{0xd4, 0x38},
{0xd5, 0x12},

{0xd6, 0x30},
{0xd7, 0x00},
{0xd8, 0x0a},
{0xd9, 0x16},
{0xda, 0x39},
{0xdb, 0xf8},

/////////////////////////////////////////////////
////////////////////	LSC   ////////////////////
/////////////////////////////////////////////////
{0xfe, 0x01},
{0xc1, 0x3c},
{0xc2, 0x50},
{0xc3, 0x00},
{0xc4, 0x40},
{0xc5, 0x30},
{0xc6, 0x30},
{0xc7, 0x10},
{0xc8, 0x00},
{0xc9, 0x00},
{0xdc, 0x20},
{0xdd, 0x10},
{0xdf, 0x00},
{0xde, 0x00},

/////////////////////////////////////////////////
///////////////////  Histogram  /////////////////
/////////////////////////////////////////////////
{0x01, 0x10},
{0x0b, 0x31},
{0x0e, 0x50},
{0x0f, 0x0f},
{0x10, 0x6e},
{0x12, 0xa0},
{0x15, 0x60},
{0x16, 0x60},
{0x17, 0xe0},

/////////////////////////////////////////////////
//////////////   Measure Window   ///////////////
/////////////////////////////////////////////////
{0xcc, 0x0c},
{0xcd, 0x10},
{0xce, 0xa0},
{0xcf, 0xe6},

/////////////////////////////////////////////////
/////////////////	 dark sun	//////////////////
/////////////////////////////////////////////////
{0x45, 0xf7},
{0x46, 0xff},
{0x47, 0x15},
{0x48, 0x03},
{0x4f, 0x60},

/////////////////////////////////////////////////
///////////////////  banding  ///////////////////
/////////////////////////////////////////////////
{0xfe, 0x00},
{0x05, 0x00},
{0x06, 0xd0}, //HB
{0x07, 0x00},
{0x08, 0x42}, //VB
{0xfe, 0x01},
{0x25, 0x00}, //step
{0x26, 0xa6},
{0x27, 0x01}, //20fps
{0x28, 0xf2},
{0x29, 0x01}, //12.5fps
{0x2a, 0xf2},
{0x2b, 0x01}, //7.14fps
{0x2c, 0xf2},
{0x2d, 0x01}, //5.55fps
{0x2e, 0xf2},
{0x3c, 0x00},
{0xfe, 0x00},

{0xfe, 0x03},
{0x10, 0x94},
{0xfe, 0x00},
};

#endif

static osiClockConstrainRegistry_t gcCamCLK = {.tag = HAL_NAME_CAM};

static const cameraReg_t RG_InitVgaMIPI[] =
    {
        {0xfe, 0xf0},
        {0xfe, 0xf0},
        {0xfe, 0x00},
        {0xfc, 0x0e},
        {0xfc, 0x0e},
        {0xf2, 0x80},
        {0xf3, 0x00}, // output_disable
        {0xf7, 0x1b},
        {0xf8, 0x04},
        {0xf9, 0x8e},
        {0xfa, 0x11},
        /////////////////////////////////////////////////
        ///////////////////   MIPI   ////////////////////
        /////////////////////////////////////////////////
        {0xfe, 0x03},
        {0x40, 0x08},
        {0x42, 0x00},
        {0x43, 0x00},
        {0x01, 0x03},
        {0x10, 0x84},

        {0x01, 0x03},
        {0x02, 0x00},
        {0x03, 0x94},
        {0x04, 0x01},
        {0x05, 0x00},
        {0x06, 0x80},
        {0x11, 0x1e},
        {0x12, 0x00},
        {0x13, 0x05},
        {0x15, 0x10},
        {0x21, 0x10},
        {0x22, 0x01},
        {0x23, 0x10},
        {0x24, 0x02},
        {0x25, 0x10},
        {0x26, 0x03},
        {0x29, 0x02},
        {0x2a, 0x0a},
        {0x2b, 0x04},
        {0xfe, 0x00},

        /////////////////////////////////////////////////
        /////////////////	CISCTL reg	 /////////////////
        /////////////////////////////////////////////////
        {0x00, 0x2f},
        {0x01, 0x0f}, //06
        {0x02, 0x04},
        {0x03, 0x03},
        {0x04, 0x50},
        {0x09, 0x00},
        {0x0a, 0x00},
        {0x0b, 0x00},
        {0x0c, 0x04},
        {0x0d, 0x01},
        {0x0e, 0xe8},
        {0x0f, 0x02},
        {0x10, 0x88},
        {0x16, 0x00},
        {0x17, 0x14},
        {0x18, 0x1a},
        {0x19, 0x14},
        {0x1b, 0x48},
        {0x1c, 0x1c},
        {0x1e, 0x6b},
        {0x1f, 0x28},
        {0x20, 0x8b}, //0x89 travis20140801
        {0x21, 0x49},
        {0x22, 0xb0},
        {0x23, 0x04},
        {0x24, 0x16},
        {0x34, 0x20},

        /////////////////////////////////////////////////
        ////////////////////	BLK   ////////////////////
        /////////////////////////////////////////////////
        {0x26, 0x23},
        {0x28, 0xff},
        {0x29, 0x00},
        {0x32, 0x00},
        {0x33, 0x10},
        {0x37, 0x20},
        {0x38, 0x10},
        {0x47, 0x80},
        {0x4e, 0x66},
        {0xa8, 0x02},
        {0xa9, 0x80},

        /////////////////////////////////////////////////
        //////////////////  ISP reg   ///////////////////
        /////////////////////////////////////////////////
        {0x40, 0xff},
        {0x41, 0x21},
        {0x42, 0xcf},
        {0x44, 0x02},
        {0x45, 0xa8},
        {0x46, 0x02}, //sync
        {0x4a, 0x11},
        {0x4b, 0x01},
        {0x4c, 0x20},
        {0x4d, 0x05},
        {0x4f, 0x01},
        {0x50, 0x01},
        {0x55, 0x01},
        {0x56, 0xe0},
        {0x57, 0x02},
        {0x58, 0x80},

        /////////////////////////////////////////////////
        ///////////////////   GAIN   ////////////////////
        /////////////////////////////////////////////////
        {0x70, 0x70},
        {0x5a, 0x84},
        {0x5b, 0xc9},
        {0x5c, 0xed},
        {0x77, 0x74},
        {0x78, 0x40},
        {0x79, 0x5f},

        /////////////////////////////////////////////////
        ///////////////////   DNDD  /////////////////////
        /////////////////////////////////////////////////
        {0x82, 0x14},
        {0x83, 0x0b},
        {0x89, 0xf0},

        /////////////////////////////////////////////////
        //////////////////   EEINTP  ////////////////////
        /////////////////////////////////////////////////
        {0x8f, 0xaa},
        {0x90, 0x8c},
        {0x91, 0x90},
        {0x92, 0x03},
        {0x93, 0x03},
        {0x94, 0x05},
        {0x95, 0x65},
        {0x96, 0xf0},

        /////////////////////////////////////////////////
        /////////////////////	ASDE  ////////////////////
        /////////////////////////////////////////////////
        {0xfe, 0x00},

        {0x9a, 0x20},
        {0x9b, 0x80},
        {0x9c, 0x40},
        {0x9d, 0x80},

        {0xa1, 0x30},
        {0xa2, 0x32},
        {0xa4, 0x30},
        {0xa5, 0x30},
        {0xaa, 0x10},
        {0xac, 0x22},

        /////////////////////////////////////////////////
        ///////////////////   GAMMA   ///////////////////
        /////////////////////////////////////////////////
        {0xfe, 0x00}, //default
        {0xbf, 0x08},
        {0xc0, 0x16},
        {0xc1, 0x28},
        {0xc2, 0x41},
        {0xc3, 0x5a},
        {0xc4, 0x6c},
        {0xc5, 0x7a},
        {0xc6, 0x96},
        {0xc7, 0xac},
        {0xc8, 0xbc},
        {0xc9, 0xc9},
        {0xca, 0xd3},
        {0xcb, 0xdd},
        {0xcc, 0xe5},
        {0xcd, 0xf1},
        {0xce, 0xfa},
        {0xcf, 0xff},

        /////////////////////////////////////////////////
        ///////////////////   YCP	//////////////////////
        /////////////////////////////////////////////////
        {0xd0, 0x40},
        {0xd1, 0x34},
        {0xd2, 0x34},
        {0xd3, 0x40},
        {0xd6, 0xf2},
        {0xd7, 0x1b},
        {0xd8, 0x18},
        {0xdd, 0x03},

        /////////////////////////////////////////////////
        ////////////////////	AEC   ////////////////////
        /////////////////////////////////////////////////
        {0xfe, 0x01},
        {0x05, 0x30},
        {0x06, 0x75},
        {0x07, 0x40},
        {0x08, 0xb0},
        {0x0a, 0xc5},
        {0x0b, 0x11},
        {0x0c, 0x00},
        {0x12, 0x52},
        {0x13, 0x38},
        {0x18, 0x95},
        {0x19, 0x96},
        {0x1f, 0x20},
        {0x20, 0xc0},
        {0x3e, 0x40},
        {0x3f, 0x57},
        {0x40, 0x7d},
        {0x03, 0x60},
        {0x44, 0x02},

        /////////////////////////////////////////////////
        ////////////////////	AWB   ////////////////////
        /////////////////////////////////////////////////
        {0xfe, 0x01},
        {0x1c, 0x91},
        {0x21, 0x15},
        {0x50, 0x80},
        {0x56, 0x04},
        {0x59, 0x08},
        {0x5b, 0x02},
        {0x61, 0x8d},
        {0x62, 0xa7},
        {0x63, 0xd0},
        {0x65, 0x06},
        {0x66, 0x06},
        {0x67, 0x84},
        {0x69, 0x08},
        {0x6a, 0x25},
        {0x6b, 0x01},
        {0x6c, 0x00},
        {0x6d, 0x02},
        {0x6e, 0xf0},
        {0x6f, 0x80},
        {0x76, 0x80},
        {0x78, 0xaf},
        {0x79, 0x75},
        {0x7a, 0x40},
        {0x7b, 0x50},
        {0x7c, 0x0c},

        {0x90, 0xc9}, //stable AWB
        {0x91, 0xbe},
        {0x92, 0xe2},
        {0x93, 0xc9},
        {0x95, 0x1b},
        {0x96, 0xe2},
        {0x97, 0x49},
        {0x98, 0x1b},
        {0x9a, 0x49},
        {0x9b, 0x1b},
        {0x9c, 0xc3},
        {0x9d, 0x49},
        {0x9f, 0xc7},
        {0xa0, 0xc8},
        {0xa1, 0x00},
        {0xa2, 0x00},
        {0x86, 0x00},
        {0x87, 0x00},
        {0x88, 0x00},
        {0x89, 0x00},
        {0xa4, 0xb9},
        {0xa5, 0xa0},
        {0xa6, 0xba},
        {0xa7, 0x92},
        {0xa9, 0xba},
        {0xaa, 0x80},
        {0xab, 0x9d},
        {0xac, 0x7f},
        {0xae, 0xbb},
        {0xaf, 0x9d},
        {0xb0, 0xc8},
        {0xb1, 0x97},
        {0xb3, 0xb7},
        {0xb4, 0x7f},
        {0xb5, 0x00},
        {0xb6, 0x00},
        {0x8b, 0x00},
        {0x8c, 0x00},
        {0x8d, 0x00},
        {0x8e, 0x00},
        {0x94, 0x55},
        {0x99, 0xa6},
        {0x9e, 0xaa},
        {0xa3, 0x0a},
        {0x8a, 0x00},
        {0xa8, 0x55},
        {0xad, 0x55},
        {0xb2, 0x55},
        {0xb7, 0x05},
        {0x8f, 0x00},
        {0xb8, 0xcb},
        {0xb9, 0x9b},

        /////////////////////////////////////////////////
        ////////////////////  CC ////////////////////////
        /////////////////////////////////////////////////
        {0xfe, 0x01},

        {0xd0, 0x38}, //skin red
        {0xd1, 0x00},
        {0xd2, 0x02},
        {0xd3, 0x04},
        {0xd4, 0x38},
        {0xd5, 0x12},

        {0xd6, 0x30},
        {0xd7, 0x00},
        {0xd8, 0x0a},
        {0xd9, 0x16},
        {0xda, 0x39},
        {0xdb, 0xf8},

        /////////////////////////////////////////////////
        ////////////////////	LSC   ////////////////////
        /////////////////////////////////////////////////
        {0xfe, 0x01},
        {0xc1, 0x3c},
        {0xc2, 0x50},
        {0xc3, 0x00},
        {0xc4, 0x40},
        {0xc5, 0x30},
        {0xc6, 0x30},
        {0xc7, 0x10},
        {0xc8, 0x00},
        {0xc9, 0x00},
        {0xdc, 0x20},
        {0xdd, 0x10},
        {0xdf, 0x00},
        {0xde, 0x00},

        /////////////////////////////////////////////////
        ///////////////////  Histogram  /////////////////
        /////////////////////////////////////////////////
        {0x01, 0x10},
        {0x0b, 0x31},
        {0x0e, 0x50},
        {0x0f, 0x0f},
        {0x10, 0x6e},
        {0x12, 0xa0},
        {0x15, 0x60},
        {0x16, 0x60},
        {0x17, 0xe0},

        /////////////////////////////////////////////////
        //////////////   Measure Window   ///////////////
        /////////////////////////////////////////////////
        {0xcc, 0x0c},
        {0xcd, 0x10},
        {0xce, 0xa0},
        {0xcf, 0xe6},

        /////////////////////////////////////////////////
        /////////////////	 dark sun	//////////////////
        /////////////////////////////////////////////////
        {0x45, 0xf7},
        {0x46, 0xff},
        {0x47, 0x15},
        {0x48, 0x03},
        {0x4f, 0x60},

        /////////////////////////////////////////////////
        ///////////////////  banding  ///////////////////
        /////////////////////////////////////////////////
        {0xfe, 0x00},
        {0x05, 0x00},
        {0x06, 0xd0}, //HB
        {0x07, 0x00},
        {0x08, 0x42}, //VB
        {0xfe, 0x01},
        {0x25, 0x00}, //step
        {0x26, 0xa6},
        {0x27, 0x01}, //20fps
        {0x28, 0xf2},
        {0x29, 0x01}, //12.5fps
        {0x2a, 0xf2},
        {0x2b, 0x01}, //7.14fps
        {0x2c, 0xf2},
        {0x2d, 0x01}, //5.55fps
        {0x2e, 0xf2},
        {0x3c, 0x00},
        {0xfe, 0x00},

        {0xfe, 0x03},
        {0x10, 0x94},
        {0xfe, 0x00},
};

sensorInfo_t gc0310Info =
    {
        "gc0310",         //		const char *name; // name of sensor
        DRV_I2C_BPS_100K, //		drvI2cBps_t baud;
        0x42 >> 1,        //		uint8_t salve_i2c_addr_w;	 // salve i2c write address
        0x43 >> 1,        //		uint8_t salve_i2c_addr_r;	 // salve i2c read address
        0,                //		uint8_t reg_addr_value_bits; // bit0: 0: i2c register value is 8 bit, 1: i2c register value is 16 bit
        {0xa3, 0x10},     //		uint8_t sensorid[2];
        640,              //		uint16_t spi_pixels_per_line;	// max width of source image
        480,              //		uint16_t spi_pixels_per_column; // max height of source image
        1,                //		uint16_t rstActiveH;	// 1: high level valid; 0: low level valid
        100,              //		uint16_t rstPulseWidth; // Unit: ms. Less than 200ms
        1,                //		uint16_t pdnActiveH;	// 1: high level valid; 0: low level valid
        0,                //		uint16_t dstWinColStart;
        640,              //		uint16_t dstWinColEnd;
        0,                //		uint16_t dstWinRowStart;
        480,              //		uint16_t dstWinRowEnd;
        1,                //		uint16_t spi_ctrl_clk_div;
        DRV_NAME_I2C1,    //		uint32_t i2c_name;
        0,                //		uint32_t nPixcels;
        2,                //		uint8_t captureDrops;
        0,
        0,
        NULL, //		uint8_t *buffer;
        {NULL, NULL},
        false,
        true,
        true,
        false, //		bool isCamIfcStart;
        false, //		bool scaleEnable;
        false, //		bool cropEnable;
        false, //		bool dropFrame;
        false, //		bool spi_href_inv;
        false, //		bool spi_little_endian_en;
        false,
        false,
        true,
        SENSOR_VDD_2800MV,     //		cameraAVDD_t avdd_val; // voltage of avdd
        SENSOR_VDD_1800MV,     //		cameraAVDD_t iovdd_val;
        CAMA_CLK_OUT_FREQ_24M, //		cameraClk_t sensorClk;
        ROW_RATIO_1_1,         //		camRowRatio_t rowRatio;
        COL_RATIO_1_1,         //		camColRatio_t colRatio;
        CAM_FORMAT_YUV,        //		cameraImageFormat_t image_format; // define in SENSOR_IMAGE_FORMAT_E enum,
        SPI_MODE_NO,           //		camSpiMode_t camSpiMode;
        SPI_OUT_Y1_U0_Y0_V0,   //		camSpiYuv_t camYuvMode;
        camCaptureIdle,        //		camCapture_t camCapStatus;
        camCsi_In,             //
        NULL,                  //		drvIfcChannel_t *camp_ipc;
        NULL,                  //		drvI2cMaster_t *i2c_p;
        NULL,                  //		CampCamptureCB captureCB;
        NULL,
};

#ifndef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
static
#endif
void prvCamGc0310PowerOn(void)
{
    sensorInfo_t *p = &gc0310Info;
    halPmuSetPowerLevel(HAL_POWER_CAMD, p->dvdd_val);
    halPmuSwitchPower(HAL_POWER_CAMD, true, false);
    osiDelayUS(1000);
    halPmuSetPowerLevel(HAL_POWER_CAMA, p->avdd_val);
    halPmuSwitchPower(HAL_POWER_CAMA, true, false);
    osiDelayUS(1000);
}

static void prvCamGc0310PowerOff(void)
{
    halPmuSwitchPower(HAL_POWER_CAMA, false, false);
    osiDelayUS(1000);
    halPmuSwitchPower(HAL_POWER_CAMD, false, false);
    osiDelayUS(1000);
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
static
#endif
bool prvCamGc0310I2cOpen(uint32_t name, drvI2cBps_t bps)
{
    sensorInfo_t *p = &gc0310Info;
    if (name == 0 || bps != DRV_I2C_BPS_100K || p->i2c_p != NULL)
    {
        return false;
    }
    p->i2c_p = drvI2cMasterAcquire(name, bps);
    if (p->i2c_p == NULL)
    {
        OSI_LOGE(0x10007d55, "cam i2c open fail");
        return false;
    }
    return true;
}

static void prvCamGc0310I2cClose()
{
    sensorInfo_t *p = &gc0310Info;
    if (p->i2c_p != NULL)
        drvI2cMasterRelease(p->i2c_p);
    p->i2c_p = NULL;
}

static void prvCamWriteOneReg(uint8_t addr, uint8_t data)
{
    sensorInfo_t *p = &gc0310Info;
    drvI2cSlave_t idAddress = {p->salve_i2c_addr_w, addr, 0, false};
    if (p->i2c_p != NULL)
    {
        drvI2cWrite(p->i2c_p, &idAddress, &data, 1);
    }
    else
    {
        OSI_LOGE(0x10007d56, "i2c is not open");
    }
}

static void prvCamReadReg(uint8_t addr, uint8_t *data, uint32_t len)
{
    sensorInfo_t *p = &gc0310Info;
    drvI2cSlave_t idAddress = {p->salve_i2c_addr_w, addr, 0, false};
    if (p->i2c_p != NULL)
    {
        drvI2cRead(p->i2c_p, &idAddress, data, len);
    }
    else
    {
        OSI_LOGE(0x10007d56, "i2c is not open");
    }
}

static bool prvCamWriteRegList(const cameraReg_t *regList, uint16_t len)
{
    sensorInfo_t *p = &gc0310Info;
    uint16_t regCount;
    drvI2cSlave_t wirte_data = {p->salve_i2c_addr_w, 0, 0, false};
    for (regCount = 0; regCount < len; regCount++)
    {
        OSI_LOGI(0x10007d57, "cam write reg %x,%x", regList[regCount].addr, regList[regCount].data);
        wirte_data.addr_data = regList[regCount].addr;
        if (drvI2cWrite(p->i2c_p, &wirte_data, &regList[regCount].data, 1))
            osiDelayUS(5);
        else
            return false;
    }
    return true;
}

static bool prvCamGc0310Rginit(sensorInfo_t *info)
{
    OSI_LOGI(0x10007d58, "cam prvCamGC0310Rginit %d", info->sensorType);
    switch (info->sensorType)
    {
    case camCsi_In:
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
        if(info->nPixcels == VGA)
        {   
            if (!prvCamWriteRegList(RG_InitVgaMIPI, sizeof(RG_InitVgaMIPI) / sizeof(cameraReg_t)))
            {
                return false;
            }
        }
        else if(info->nPixcels == QVGA)
        {
            if (!prvCamWriteRegList(RG_InitQvgaMIPI, sizeof(RG_InitQvgaMIPI) / sizeof(cameraReg_t)))
                return false;
        }
        else
        {
            return false;
        }
        break;
#else
        if(!prvCamWriteRegList(RG_InitVgaMIPI, sizeof(RG_InitVgaMIPI) / sizeof(cameraReg_t)))
            return false;
        break;
#endif
    default:
        return false;
    }
    return true;
}

static void prvSpiCamIsrCB(void *ctx)
{
    sensorInfo_t *p = &gc0310Info;
    static uint8_t pictureDrop = 0;
    REG_CAMERA_IRQ_CAUSE_T cause;
    cameraIrqCause_t mask = {0, 0, 0, 0};
    cause.v = hwp_camera->irq_cause;
    hwp_camera->irq_clear = cause.v;
    switch (p->camCapStatus)
    {
    case camCaptureState1:
        if (cause.b.vsync_f)
        {
            drvCamClrIrqMask();
            drvCamCmdSetFifoRst();
            if (p->isCamIfcStart == true)
            {
                drvCampStopTransfer(p->nPixcels, p->capturedata);
                p->isCamIfcStart = false;
            }
            drvCampStartTransfer(p->nPixcels, p->capturedata);
            mask.fend = 1;
            drvCamSetIrqMask(mask);
            p->camCapStatus = camCaptureState2;
            p->isCamIfcStart = true;
        }
        break;
    case camCaptureState2:
        if (cause.b.vsync_f)
        {
            if (p->isCamIfcStart == true)
            {
                p->isCamIfcStart = false;
                if (drvCampStopTransfer(p->nPixcels, p->capturedata) == true)
                {
                    if (pictureDrop < p->captureDrops)
                    {
                        mask.fend = 1;
                        drvCamSetIrqMask(mask);
                        p->camCapStatus = camCaptureState1;
                        pictureDrop++;
                    }
                    else
                    {
                        p->camCapStatus = camCaptureIdle;
                        osiSemaphoreRelease(p->cam_sem_capture);
                        if (pictureDrop >= p->captureDrops)
                            pictureDrop = 0;
                        p->isFirst = false;
                    }
                }
                else
                {
                    drvCampStartTransfer(p->nPixcels, p->capturedata);
                    mask.fend = 1;
                    p->isCamIfcStart = true;
                    drvCamSetIrqMask(mask);
                }
            }
        }
        break;
    case campPreviewState1:
        if (cause.b.vsync_f)
        {
            drvCamClrIrqMask();
            drvCamCmdSetFifoRst();
            if (p->isCamIfcStart == true)
            {
                drvCampStopTransfer(p->nPixcels, p->previewdata[p->page_turn]);
                p->isCamIfcStart = false;
            }
            drvCampStartTransfer(p->nPixcels, p->previewdata[p->page_turn]);
            mask.fend = 1;
            drvCamSetIrqMask(mask);
            p->camCapStatus = campPreviewState2;
            p->isCamIfcStart = true;
        }
        break;
    case campPreviewState2:
        if (cause.b.vsync_f)
        {
            drvCamClrIrqMask();
            drvCamCmdSetFifoRst();
            if (p->isCamIfcStart == true)
            {
                p->isCamIfcStart = false;
                if (drvCampStopTransfer(p->nPixcels, p->previewdata[p->page_turn]) == true)
                {
                    OSI_LOGI(0x10007d59, "cam recv data %d", p->page_turn);
                    p->page_turn = 1 - p->page_turn;
                    osiSemaphoreRelease(p->cam_sem_preview);
                    p->isFirst = false;
                    if (--p->preview_page)
                    {
                        drvCamClrIrqMask();
                        drvCamCmdSetFifoRst();
                        drvCampStartTransfer(p->nPixcels, p->previewdata[p->page_turn]);
                        p->isCamIfcStart = true;
                        mask.fend = 1;
                        drvCamSetIrqMask(mask);
                    }
                    else
                    {
                        p->camCapStatus = camCaptureIdle;
                    }
                }
                else
                {
                    p->camCapStatus = campPreviewState1;
                    mask.fend = 1;
                    drvCamSetIrqMask(mask);
                }
            }
        }
        break;
    default:
        break;
    }
}

static void prvCsiCamIsrCB(void *ctx)
{
    static uint8_t pictureDrop = 0;
    static uint8_t prev_ovf = 0;
    static uint8_t cap_ovf = 0;
    sensorInfo_t *p = &gc0310Info;
    REG_CAMERA_IRQ_CAUSE_T cause;
    cameraIrqCause_t mask = {0, 0, 0, 0};
    cause.v = hwp_camera->irq_cause;
    hwp_camera->irq_clear = cause.v;
    OSI_LOGI(0x10007d5a, "cam gc0310 prvCsiCamIsrCB %d,%d,%d,%d", cause.b.vsync_f, cause.b.ovfl, cause.b.dma_done, cause.b.vsync_r);
    OSI_LOGI(0x10007d5b, "p->camCapStatus %d", p->camCapStatus);
    switch (p->camCapStatus)
    {
    case camCaptureState1:
        if (cause.b.vsync_f)
        {
            drvCamClrIrqMask();
            drvCamCmdSetFifoRst();
            drvCampStartTransfer(p->nPixcels, p->previewdata[p->page_turn]);
            p->isCamIfcStart = true;
            mask.overflow = 1;
            mask.dma = 1;
            drvCamSetIrqMask(mask);
            p->camCapStatus = camCaptureState2;
        }
        break;
    case camCaptureState2:
        if (cause.b.ovfl)
        {
            OSI_LOGI(0x10007d5c, "cam ovfl ");
            drvCameraControllerEnable(false);
            drvCamCmdSetFifoRst();
            drvCameraControllerEnable(true);
            cap_ovf = 1;
        }
        if (cause.b.dma_done)
        {
            drvCamClrIrqMask();
            if (cap_ovf == 1)
            {
                cap_ovf = 0;
                if (p->isCamIfcStart)
                    drvCampStopTransfer(p->nPixcels, p->previewdata[p->page_turn]);
                p->camCapStatus = camCaptureState1;
                mask.fend = 1;
                drvCamSetIrqMask(mask);
                return;
            }
            if (p->isCamIfcStart == true)
            {
                p->isCamIfcStart = false;
                if (drvCampStopTransfer(p->nPixcels, p->capturedata))
                {
                    OSI_LOGI(0x10007d5d, "cam p->captureDrops %d ", p->captureDrops);
                    OSI_LOGI(0x10007d5e, "cam pictureDrop %d ", pictureDrop);
                    if (pictureDrop < p->captureDrops)
                    {
                        mask.fend = 1;
                        drvCamSetIrqMask(mask);
                        p->camCapStatus = camCaptureState1;
                        pictureDrop++;
                    }
                    else
                    {
                        p->camCapStatus = camCaptureIdle;
                        osiSemaphoreRelease(p->cam_sem_capture);
                        if (pictureDrop >= p->captureDrops)
                            pictureDrop = 0;
                        p->isFirst = false;
                    }
                }
            }
        }
        break;
    case campPreviewState1:
        if (cause.b.vsync_f)
        {
            drvCamClrIrqMask();
            OSI_LOGI(0x10007d5f, "cam mask ovf %d, firstframe %d", prev_ovf, p->isFirstFrame);
            if (prev_ovf || p->isFirstFrame)
            {
                drvCamCmdSetFifoRst();
                drvCampStartTransfer(p->nPixcels, p->previewdata[p->page_turn]);
                p->isCamIfcStart = true;
                p->isFirstFrame = false;
                prev_ovf = 0;
                hwp_camera->irq_clear |= 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3;
            }
            hwp_camera->irq_clear |= 1 << 0;
            mask.overflow = 1;
            mask.dma = 1;
            drvCamSetIrqMask(mask);
            p->camCapStatus = campPreviewState2;
        }
        break;
    case campPreviewState2:
        if (cause.b.ovfl)
        {
            OSI_LOGI(0x10007d5c, "cam ovfl ");
            prev_ovf = 1;

            drvCamClrIrqMask();
            mask.fend = 1;
            drvCamSetIrqMask(mask);
            //stop
            p->camCapStatus = campPreviewState1;
            return;
        }
        if (cause.b.dma_done)
        {
            drvCamClrIrqMask();
            if (prev_ovf == 1)
            {
                prev_ovf = 0;
                if (p->isCamIfcStart)
                    drvCampStopTransfer(p->nPixcels, p->previewdata[p->page_turn]);
                p->camCapStatus = campPreviewState1;
                mask.fend = 1;
                drvCamSetIrqMask(mask);
                return;
            }
            if (p->isCamIfcStart == true)
            {
                p->isCamIfcStart = false;
                if (drvCampStopTransfer(p->nPixcels, p->previewdata[p->page_turn]))
                {
                    OSI_LOGI(0x10007d60, "cam 0310 preview release data %x", p->previewdata[p->page_turn]);
#ifndef CONFIG_CAMERA_SINGLE_BUFFER
                    p->page_turn = 1 - p->page_turn;
#endif
                    osiSemaphoreRelease(p->cam_sem_preview);
                    p->isFirst = false;
                    OSI_LOGI(0x10007d61, "cam 0310 preview_page data %d", p->preview_page);
                    if (--p->preview_page)
                    {
                        p->camCapStatus = campPreviewState1;
                        mask.fend = 1;
                        drvCampStartTransfer(p->nPixcels, p->previewdata[p->page_turn]);
                        p->isCamIfcStart = true;
                        drvCamSetIrqMask(mask);
                    }
                    else
                    {
                        OSI_LOGI(0, "cam go to idle");
                        p->camCapStatus = camCaptureIdle;
                        drvCameraControllerEnable(false);
                    }
                }
                else
                {
                    drvCamClrIrqMask();
                    p->camCapStatus = campPreviewState1;
                    mask.fend = 1;
                    drvCamSetIrqMask(mask);
                    OSI_LOGI(0x10007d62, "cam dma stop error");
                }
            }
        }
        break;
    default:
        break;
    }
}

bool camGc0310Open(void)
{
    OSI_LOGI(0x10007d63, "camGc0310Open");
    sensorInfo_t *p = &gc0310Info;
    osiRequestSysClkActive(&gcCamCLK);
    drvCamSetPdn(false);
    osiDelayUS(1000);
    prvCamGc0310PowerOn();
    drvCamSetMCLK(p->sensorClk);
    osiDelayUS(1000);
    drvCamSetPdn(true);
    osiDelayUS(1000);
    drvCamSetPdn(false);
    if (!prvCamGc0310I2cOpen(p->i2c_name, p->baud))
    {
        OSI_LOGE(0x10007d64, "cam prvCamGc0310I2cOpen fail");
        prvCamGc0310I2cClose();
        prvCamGc0310PowerOff();
        return false;
    }
    if (!prvCamGc0310Rginit(&gc0310Info))
    {
        OSI_LOGE(0x10007d65, "cam prvCamGc0310Rginit fail");
        prvCamGc0310I2cClose();
        prvCamGc0310PowerOff();
        return false;
    }
    drvCampRegInit(&gc0310Info);
    switch (p->sensorType)
    {
    case camSpi_In:
        drvCamSetIrqHandler(prvSpiCamIsrCB, NULL);
        break;
    case camCsi_In:
        drvCamSetIrqHandler(prvCsiCamIsrCB, NULL);
        break;
    default:
        break;
    }
    p->isCamOpen = true;
    return true;
}

void camGc0310Close(void)
{
    sensorInfo_t *p = &gc0310Info;
    if (p->isCamOpen)
    {
        osiReleaseClk(&gcCamCLK);
        drvCamSetPdn(true);
        osiDelayUS(1000);
        drvCamDisableMCLK();
        osiDelayUS(1000);
        prvCamGc0310PowerOff();
        drvCamSetPdn(false);
        prvCamGc0310I2cClose();
        drvCampRegDeInit();
        drvCamDisableIrq();
        p->isFirst = true;
        p->isCamOpen = false;
    }
    else
    {
        p->isCamOpen = false;
    }
}

void camGc0310GetId(uint8_t *data, uint8_t len)
{
    sensorInfo_t *p = &gc0310Info;
    if (p->isCamOpen)
    {
        if (data == NULL || len < 1)
            return;
        prvCamWriteOneReg(0xf4, 0x1c);
        osiDelayUS(2);
        prvCamReadReg(0xf0, data, len);
    }
}

bool camGc0310CheckId(void)
{
    sensorInfo_t *p = &gc0310Info;
    uint8_t sensorID[2] = {0, 0};
    if (!p->isCamOpen)
    {
        prvCamGc0310PowerOn();
        drvCamSetMCLK(p->sensorClk);
        osiDelayUS(1000);
        drvCamSetPdn(false);
        osiDelayUS(1000);
        drvCamSetPdn(true);
        osiDelayUS(1000);
        drvCamSetPdn(false);
        if (!prvCamGc0310I2cOpen(p->i2c_name, p->baud))
        {
            OSI_LOGE(0x10007d66, "cam prvCamGc0310I2cOpen failed");
            return false;
        }
        if (!p->isCamOpen)
        {
            p->isCamOpen = true;
        }
        camGc0310GetId(sensorID, 2);
        OSI_LOGI(0x10007d67, "cam get id 0x%x,0x%x", sensorID[0], sensorID[1]);
        if ((p->sensorid[0] == sensorID[0]) && (p->sensorid[1] == sensorID[1]))
        {
            OSI_LOGI(0x10007d68, "check id successful");
            camGc0310Close();
            return true;
        }
        else
        {
            camGc0310Close();
            OSI_LOGE(0x10007d69, "check id error");
            return false;
        }
    }
    else
    {
        OSI_LOGE(0x10007d6a, "camera already opened !");
        return false;
    }
}

void camGc0310CaptureImage(uint32_t size)
{
    sensorInfo_t *p = &gc0310Info;
    if (size != 0)
    {
        cameraIrqCause_t mask = {0, 0, 0, 0};
        if (p->isCamIfcStart == true)
        {
            drvCampStopTransfer(p->nPixcels, p->capturedata);
            p->isCamIfcStart = false;
        }
        p->nPixcels = size;
        p->camCapStatus = camCaptureState1;
        mask.fend = 1;
        drvCamSetIrqMask(mask);
        drvCameraControllerEnable(true);
    }
}

void camGc0310PrevStart(uint32_t size)
{
    sensorInfo_t *p = &gc0310Info;
    OSI_LOGI(0x10007d6b, "camGc0310PrevStart p->preview_page=%d ", p->preview_page);
    if (p->preview_page == 0)
    {
        cameraIrqCause_t mask = {0, 0, 0, 0};
        drvCamSetIrqMask(mask);
        if (p->isCamIfcStart == true)
        {
            drvCampStopTransfer(p->nPixcels, p->previewdata[p->page_turn]);
            p->isCamIfcStart = false;
        }
        p->nPixcels = size;
        p->camCapStatus = campPreviewState1;
        p->isStopPrev = false;
#ifdef CONFIG_CAMERA_SINGLE_BUFFER
        p->preview_page = 1;
#else
        p->preview_page = 2;
#endif
        mask.fend = 1;
        p->isFirstFrame = true;
        drvCamSetIrqMask(mask);
        drvCameraControllerEnable(true);
    }
}

void camGc0310PrevNotify(void)
{
    sensorInfo_t *p = &gc0310Info;
    uint32_t critical = osiEnterCritical();
    if (p->isCamOpen && !p->isStopPrev)
    {
        if (p->preview_page == 0)
        {
            cameraIrqCause_t mask = {0, 0, 0, 0};

            p->camCapStatus = campPreviewState1;
            p->preview_page++;
            p->isFirstFrame = true;
            mask.fend = 1;
            drvCamSetIrqMask(mask);
            drvCameraControllerEnable(true);
        }
#ifndef CONFIG_CAMERA_SINGLE_BUFFER
        else
        {
            if (p->preview_page < 2)
                p->preview_page++;
        }
#endif
    }
    osiExitCritical(critical);
}

void camGc0310StopPrev(void)
{
    sensorInfo_t *p = &gc0310Info;
    drvCamClrIrqMask();
    if (p->isCamIfcStart == true)
    {
        drvCampStopTransfer(p->nPixcels, p->previewdata[p->page_turn]);
        p->isCamIfcStart = false;
    }
    drvCameraControllerEnable(false);
    p->camCapStatus = camCaptureIdle;
    p->isStopPrev = true;
    p->preview_page = 0;
}

void camGc0310SetFalsh(uint8_t level)
{
    if (level >= 0 && level < 16)
    {
        if (level == 0)
        {
            halPmuSwitchPower(HAL_POWER_CAMFLASH, false, false);
        }
        else
        {
            halPmuSetCamFlashLevel(level);
            halPmuSwitchPower(HAL_POWER_CAMFLASH, true, false);
        }
    }
}

void camGc0310Brightness(uint8_t level)
{
}

void camGc0310Contrast(uint8_t level)
{
}

void camGc0310ImageEffect(uint8_t effect_type)
{
}
void camGc0310Ev(uint8_t level)
{
}

void camGc0310AWB(uint8_t mode)
{
}

void camGc0310GetSensorInfo(sensorInfo_t **pSensorInfo)
{
    OSI_LOGI(0x10007d6c, "CamGc0310GetSensorInfo %08x", &gc0310Info);
    *pSensorInfo = &gc0310Info;
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
sensorInfo_t *quec_getGc0310Info()
{
    sensorInfo_t *Info = &gc0310Info;
    return Info;
}
#endif

bool camGc0310Reg(SensorOps_t *pSensorOpsCB)
{
    uint8_t ret;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
    ret = quec_camGc0310CheckId();
#else
    ret = camGc0310CheckId();
#endif
    if (ret)
    {
        pSensorOpsCB->cameraOpen = camGc0310Open;
        pSensorOpsCB->cameraClose = camGc0310Close;
        pSensorOpsCB->cameraGetID = camGc0310GetId;
        pSensorOpsCB->cameraCaptureImage = camGc0310CaptureImage;
        pSensorOpsCB->cameraStartPrev = camGc0310PrevStart;
        pSensorOpsCB->cameraPrevNotify = camGc0310PrevNotify;
        pSensorOpsCB->cameraStopPrev = camGc0310StopPrev;
        pSensorOpsCB->cameraSetAWB = camGc0310AWB;
        pSensorOpsCB->cameraSetBrightness = camGc0310Brightness;
        pSensorOpsCB->cameraSetContrast = camGc0310Contrast;
        pSensorOpsCB->cameraSetEv = camGc0310Ev;
        pSensorOpsCB->cameraSetImageEffect = camGc0310ImageEffect;
        pSensorOpsCB->cameraGetSensorInfo = camGc0310GetSensorInfo;
        pSensorOpsCB->cameraFlashSet = camGc0310SetFalsh;
        return true;
    }
    return false;
}
