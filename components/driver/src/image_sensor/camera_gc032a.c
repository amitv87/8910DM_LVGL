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
extern  bool quec_camGc032aCheckId(void);
#endif

static osiClockConstrainRegistry_t gcCamCLK = {.tag = HAL_NAME_CAM};

const cameraReg_t RG_InitVgaSPI[] =
    {
        /*System*/
        {0xf3, 0x83}, //ff//1f//01 data output
        {0xf5, 0x08},
        {0xf7, 0x01},
        {0xf8, 0x04}, ////pll-div----frank
        {0xf9, 0x4e},
        {0xfa, 0x00},
        {0xfc, 0x02},
        {0xfe, 0x02},
        {0x81, 0x03},

        {0xfe, 0x00},
        {0x77, 0x64},
        {0x78, 0x40},
        {0x79, 0x60},
        /*Analog&Cisctl*/
        {0xfe, 0x00},
        {0x03, 0x01},
        {0x04, 0xcb},
        {0x05, 0x01},
        {0x06, 0xb2},
        {0x07, 0x00},
        {0x08, 0x10},

        {0x0a, 0x00},
        {0x0c, 0x00},
        {0x0d, 0x01},
        {0x0e, 0xe8},
        {0x0f, 0x02},
        {0x10, 0x88},

        {0x17, 0x54},
        {0x19, 0x08},
        {0x1a, 0x0a},
        {0x1f, 0x40},
        {0x20, 0x30},
        {0x2e, 0x80},
        {0x2f, 0x2b},
        {0x30, 0x1a},
        {0xfe, 0x02},
        {0x03, 0x02},
        {0x05, 0xd7},
        {0x06, 0x60},
        {0x08, 0x80},
        {0x12, 0x89},

        /*SPI*/
        {0xfe, 0x03},
        {0x52, 0xba},
        {0x53, 0x24},
        {0x54, 0x20},
        {0x55, 0x00},
        {0x59, 0x1f}, // {0x59,0x10}, 20190627 scaler output error
        {0x5a, 0x40}, //00 //yuv
        {0x5b, 0x80},
        {0x5c, 0x02},
        {0x5d, 0xe0},
        {0x5e, 0x01},
        {0x51, 0x03},
        {0x64, 0x04},
        {0xfe, 0x00},

        /*blk*/
        {0xfe, 0x00},
        {0x18, 0x02},
        {0xfe, 0x02},
        {0x40, 0x22},
        {0x45, 0x00},
        {0x46, 0x00},
        {0x49, 0x20},
        {0x4b, 0x3c},
        {0x50, 0x20},
        {0x42, 0x10},

        /*isp*/
        {0xfe, 0x01},
        {0x0a, 0xc5},
        {0x45, 0x00},
        {0xfe, 0x00},
        {0x40, 0xff},
        {0x41, 0x25},
        {0x42, 0xef},
        {0x43, 0x10},
        {0x44, 0x82},
        {0x46, 0x22},
        {0x49, 0x03},
        {0x52, 0x02},
        {0x54, 0x00},
        {0xfe, 0x02},
        {0x22, 0xf6},

        /*Shading*/
        {0xfe, 0x01},
        {0xc1, 0x38},
        {0xc2, 0x4c},
        {0xc3, 0x00},
        {0xc4, 0x2c},
        {0xc5, 0x24},
        {0xc6, 0x18},
        {0xc7, 0x28},
        {0xc8, 0x11},
        {0xc9, 0x15},
        {0xca, 0x20},
        {0xdc, 0x7a},
        {0xdd, 0xa0},
        {0xde, 0x80},
        {0xdf, 0x88},

        /*AWB*/ /*20170110*/
        {0xfe, 0x01},
        {0x50, 0xc1},
        {0x56, 0x34},
        {0x58, 0x04},
        {0x65, 0x06},
        {0x66, 0x0f},
        {0x67, 0x04},
        {0x69, 0x20},
        {0x6a, 0x40},
        {0x6b, 0x81},
        {0x6d, 0x12},
        {0x6e, 0xc0},
        {0x7b, 0x2a},
        {0x7c, 0x0c},
        {0xfe, 0x01},
        {0x90, 0xe3},
        {0x91, 0xc2},
        {0x92, 0xff},
        {0x93, 0xe3},
        {0x95, 0x1c},
        {0x96, 0xff},
        {0x97, 0x44},
        {0x98, 0x1c},
        {0x9a, 0x44},
        {0x9b, 0x1c},
        {0x9c, 0x64},
        {0x9d, 0x44},
        {0x9f, 0x71},
        {0xa0, 0x64},
        {0xa1, 0x00},
        {0xa2, 0x00},
        {0x86, 0x00},
        {0x87, 0x00},
        {0x88, 0x00},
        {0x89, 0x00},
        {0xa4, 0xc2},
        {0xa5, 0x9b},
        {0xa6, 0xc8},
        {0xa7, 0x92},
        {0xa9, 0xc9},
        {0xaa, 0x96},
        {0xab, 0xa9},
        {0xac, 0x99},
        {0xae, 0xce},
        {0xaf, 0xa9},
        {0xb0, 0xcf},
        {0xb1, 0x9d},
        {0xb3, 0xcf},
        {0xb4, 0xac},
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
        {0xb8, 0xc7},
        {0xb9, 0xa0},

        {0xfe, 0x01},
        {0xd0, 0x40},
        {0xd1, 0x00},
        {0xd2, 0x00},
        {0xd3, 0xfa},
        {0xd4, 0x4a},
        {0xd5, 0x02},

        {0xd6, 0x44},
        {0xd7, 0xfa},
        {0xd8, 0x04},
        {0xd9, 0x08},
        {0xda, 0x5c},
        {0xdb, 0x02},
        {0xfe, 0x00},

        /*Gamma*/
        {0xfe, 0x00},
        {0xba, 0x00},
        {0xbb, 0x06},
        {0xbc, 0x0b},
        {0xbd, 0x10},
        {0xbe, 0x19},
        {0xbf, 0x26},
        {0xc0, 0x33},
        {0xc1, 0x3f},
        {0xc2, 0x54},
        {0xc3, 0x68},
        {0xc4, 0x7c},
        {0xc5, 0x8c},
        {0xc6, 0x9c},
        {0xc7, 0xb4},
        {0xc8, 0xc9},
        {0xc9, 0xd8},
        {0xca, 0xe4},
        {0xcb, 0xee},
        {0xcc, 0xf4},
        {0xcd, 0xf9},
        {0xce, 0xfa},
        {0xcf, 0xff},

        /*Auto Gamma*/
        {0xfe, 0x00},
        {0x5a, 0x08},
        {0x5b, 0x0f},
        {0x5c, 0x15},
        {0x5d, 0x1c},
        {0x5e, 0x28},
        {0x5f, 0x36},
        {0x60, 0x45},
        {0x61, 0x51},
        {0x62, 0x6a},
        {0x63, 0x7d},
        {0x64, 0x8d},
        {0x65, 0x98},
        {0x66, 0xa2},
        {0x67, 0xb5},
        {0x68, 0xc3},
        {0x69, 0xcd},
        {0x6a, 0xd4},
        {0x6b, 0xdc},
        {0x6c, 0xe3},
        {0x6d, 0xf0},
        {0x6e, 0xf9},
        {0x6f, 0xff},

        /*Gain*/
        {0xfe, 0x00},
        {0x70, 0x50},

        /*AEC*/
        {0xfe, 0x00},
        {0x4f, 0x01},
        {0xfe, 0x01},
        {0x0c, 0x01},
        {0x0d, 0x00}, //08 add 20170110
        {0x12, 0xa0},
        {0x13, 0x4a},
        {0x44, 0x04},
        {0x1f, 0x40},
        {0x20, 0x40},
        {0x23, 0x0a},
        {0x26, 0x9a},
        {0x3e, 0x20},
        {0x3f, 0x2d},
        {0x40, 0x40},
        {0x41, 0x5b},
        {0x42, 0x82},
        {0x43, 0xb7},
        {0x04, 0x0a},
        {0x02, 0x79},
        {0x03, 0xc0},

        /*measure window*/
        {0xfe, 0x01},
        {0xcc, 0x08},
        {0xcd, 0x08},
        {0xce, 0xa4},
        {0xcf, 0xec},

        /*DNDD*/
        {0xfe, 0x00},
        {0x81, 0xb8},
        {0x82, 0x04},
        {0x83, 0x10},
        {0x84, 0x01},
        {0x86, 0x50},
        {0x87, 0x18},
        {0x88, 0x10},
        {0x89, 0x70},
        {0x8a, 0x20},
        {0x8b, 0x10},
        {0x8c, 0x08},
        {0x8d, 0x0a},

        /*Intpee*/
        {0xfe, 0x00},
        {0x8f, 0xaa},
        {0x90, 0x1c},
        {0x91, 0x52},
        {0x92, 0x03},
        {0x93, 0x03},
        {0x94, 0x08},
        {0x95, 0x6a},
        {0x97, 0x00},
        {0x98, 0x00},

        /*ASDE*/
        {0xfe, 0x00},
        {0x9a, 0x30},
        {0x9b, 0x50},
        {0xa1, 0x30},
        {0xa2, 0x66},
        {0xa4, 0x28},
        {0xa5, 0x30},
        {0xaa, 0x28},
        {0xac, 0x32},

        /*YCP*/
        {0xfe, 0x00},
        {0xd1, 0x3f},
        {0xd2, 0x3f},
        {0xd3, 0x38},
        {0xd6, 0xf4},
        {0xd7, 0x1d},
        {0xdd, 0x72},
        {0xde, 0x84},

        {0xfe, 0x00},
        {0x05, 0x01},
        {0x06, 0xad},
        {0x07, 0x00},
        {0x08, 0x10},

        {0xfe, 0x01},
        {0x25, 0x00},
        {0x26, 0x4d},

        {0x27, 0x01},
        {0x28, 0xce}, //16.6fps
        {0x29, 0x01},
        {0x2a, 0xce}, //12.5fps
        {0x2b, 0x01},
        {0x2c, 0xce}, //10fps
        {0x2d, 0x01},
        {0x2e, 0xce}, //8.33fps
        {0x2f, 0x01},
        {0x30, 0xce}, //5.88fps
        {0x31, 0x01},
        {0x32, 0xce}, //4.34fps
        {0x33, 0x01},
        {0x34, 0xce}, //3.99fps
        {0x3c, 0x10}, //{0x3c,0x00}
        {0xfe, 0x00},
};

sensorInfo_t gc032aInfo =
    {
        "gc032a",         //		const char *name; // name of sensor
        DRV_I2C_BPS_100K, //		drvI2cBps_t baud;
        0x42 >> 1,        //		uint8_t salve_i2c_addr_w;	 // salve i2c write address
        0x43 >> 1,        //		uint8_t salve_i2c_addr_r;	 // salve i2c read address
        0,                //		uint8_t reg_addr_value_bits; // bit0: 0: i2c register value is 8 bit, 1: i2c register value is 16 bit
        {0x23, 0x2a},     //		uint8_t sensorid[2];
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
        1,                //		uint8_t captureDrops;
        0,
        0,
        NULL, //		uint8_t *buffer;
        {NULL, NULL},
        false,
        true,
        true,
        false, //		bool isCamIfcStart;
        false, //		bool scaleEnable;
        true,  //		bool cropEnable;
        false, //		bool dropFrame;
        false, //		bool spi_href_inv;
        false, //		bool spi_little_endian_en;
        false,
        false,
        true,
        SENSOR_VDD_2800MV,     //		cameraAVDD_t avdd_val; // voltage of avdd
        SENSOR_VDD_1800MV,     //		cameraAVDD_t iovdd_val;
        CAMA_CLK_OUT_FREQ_12M, //		cameraClk_t sensorClk;
        ROW_RATIO_1_1,         //		camRowRatio_t rowRatio;
        COL_RATIO_1_1,         //		camColRatio_t colRatio;
        CAM_FORMAT_YUV,        //		cameraImageFormat_t image_format; // define in SENSOR_IMAGE_FORMAT_E enum,
        SPI_MODE_MASTER2_2,    //		camSpiMode_t camSpiMode;
        SPI_OUT_Y1_U0_Y0_V0,   //		camSpiYuv_t camYuvMode;
        camCaptureIdle,        //		camCapture_t camCapStatus;
        camSpi_In,
        NULL, //		drvIfcChannel_t *camp_ipc;
        NULL, //		drvI2cMaster_t *i2c_p;
        NULL, //		CampCamptureCB captureCB;
        NULL,
};

#ifndef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
static
#endif
void prvCamGc032aPowerOn(void)
{
    sensorInfo_t *p = &gc032aInfo;
    halPmuSetPowerLevel(HAL_POWER_CAMD, p->dvdd_val);
    halPmuSwitchPower(HAL_POWER_CAMD, true, false);
    osiDelayUS(1000);
    halPmuSetPowerLevel(HAL_POWER_CAMA, p->avdd_val);
    halPmuSwitchPower(HAL_POWER_CAMA, true, false);
    osiDelayUS(1000);
}

static void prvCamGc032aPowerOff(void)
{
    halPmuSwitchPower(HAL_POWER_CAMA, false, false);
    osiDelayUS(1000);
    halPmuSwitchPower(HAL_POWER_CAMD, false, false);
    osiDelayUS(1000);
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
static
#endif
bool prvCamGc032aI2cOpen(uint32_t name, drvI2cBps_t bps)
{
    sensorInfo_t *p = &gc032aInfo;
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

static void prvCamGc032aI2cClose()
{
    sensorInfo_t *p = &gc032aInfo;
    if (p->i2c_p != NULL)
        drvI2cMasterRelease(p->i2c_p);
    p->i2c_p = NULL;
}

static void prvCamWriteOneReg(uint8_t addr, uint8_t data)
{
    sensorInfo_t *p = &gc032aInfo;
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
    sensorInfo_t *p = &gc032aInfo;
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
    sensorInfo_t *p = &gc032aInfo;
    uint16_t regCount;
    drvI2cSlave_t wirte_data = {p->salve_i2c_addr_w, 0, 0, false};
    prvCamWriteOneReg(0xf4, 0x1c);
    osiDelayUS(1000);
    for (regCount = 0; regCount < len; regCount++)
    {
        wirte_data.addr_data = regList[regCount].addr;
        if (drvI2cWrite(p->i2c_p, &wirte_data, &regList[regCount].data, 1))
            osiDelayUS(5);
        else
            return false;
    }
    return true;
}

static bool prvCamGc032aRginit(sensorInfo_t *info)
{
    switch (info->sensorType)
    {
    case camSpi_In:
        if (!prvCamWriteRegList(RG_InitVgaSPI, sizeof(RG_InitVgaSPI) / sizeof(cameraReg_t)))
            return false;
        break;
    default:
        return false;
    }
    return true;
}

static void prvCamIsrCB(void *ctx)
{
    sensorInfo_t *p = &gc032aInfo;
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
            if (p->isCamIfcStart)
            {
                p->isCamIfcStart = false;
                if (drvCampStopTransfer(p->nPixcels, p->capturedata))
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
                        OSI_LOGD(0x10007d7a, "cam 032a campture release %x", p->capturedata);
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
            if (p->isCamIfcStart)
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
            hwp_camera->spi_camera_reg4 &= ~(1 << 0);
            drvCamCmdSetFifoRst();
            hwp_camera->spi_camera_reg4 |= (1 << 0);
            if (p->isCamIfcStart == true)
            {
                p->isCamIfcStart = false;
                if (drvCampStopTransfer(p->nPixcels, p->previewdata[p->page_turn]))
                {
                    OSI_LOGD(0x10007d7b, "cam 032a preview release %x", p->previewdata[p->page_turn]);
#ifndef CONFIG_CAMERA_SINGLE_BUFFER
                    p->page_turn = 1 - p->page_turn;
#endif
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
                        drvCamClrIrqMask();
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

bool camGc032aOpen(void)
{
    OSI_LOGI(0x10007d7c, "camGc032aOpen");
    sensorInfo_t *p = &gc032aInfo;
    osiRequestSysClkActive(&gcCamCLK);
    drvCamSetPdn(false);
    osiDelayUS(1000);
    prvCamGc032aPowerOn();
    osiDelayUS(1000);
    drvCamSetMCLK(p->sensorClk);
    osiDelayUS(1000);
    drvCamSetPdn(true);
    osiDelayUS(1000);
    drvCamSetPdn(false);

    if (!prvCamGc032aI2cOpen(p->i2c_name, p->baud))
    {
        OSI_LOGE(0x10007d7d, "cam prvCamGc032aI2cOpen fail");
        prvCamGc032aI2cClose();
        prvCamGc032aPowerOff();
        return false;
    }
    if (!prvCamGc032aRginit(&gc032aInfo))
    {
        OSI_LOGE(0x10007d7e, "cam prvCamGc032aRginit fail");
        prvCamGc032aI2cClose();
        prvCamGc032aPowerOff();
        return false;
    }
    drvCampRegInit(&gc032aInfo);
    drvCamSetIrqHandler(prvCamIsrCB, NULL);
    p->isCamOpen = true;
    drvCameraControllerEnable(true);
    return true;
}

void camGc032aClose(void)
{
    sensorInfo_t *p = &gc032aInfo;
    if (p->isCamOpen)
    {
        osiReleaseClk(&gcCamCLK);
        drvCamSetPdn(true);
        drvCamDisableMCLK();
        prvCamGc032aPowerOff();
        drvCamSetPdn(false);
        prvCamGc032aI2cClose();
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

void camGc032aGetId(uint8_t *data, uint8_t len)
{
    sensorInfo_t *p = &gc032aInfo;
    if (p->isCamOpen)
    {
        if (data == NULL || len < 1)
            return;
        prvCamWriteOneReg(0xf4, 0x1c);
        osiDelayUS(2);
        prvCamReadReg(0xf0, data, len);
    }
}

bool camGc032aCheckId(void)
{
    sensorInfo_t *p = &gc032aInfo;
    uint8_t sensorID[2] = {0, 0};
    if (!p->isCamOpen)
    {
        drvCamSetPdn(false);
        osiDelayUS(5);
        prvCamGc032aPowerOn();
        drvCamSetMCLK(p->sensorClk);
        drvCamSetPdn(true);
        osiDelayUS(5);
        drvCamSetPdn(false);
        osiDelayUS(5);
        if (!prvCamGc032aI2cOpen(p->i2c_name, p->baud))
        {
            OSI_LOGE(0x10007d7d, "cam prvCamGc032aI2cOpen fail");
            return false;
        }
        if (!p->isCamOpen)
        {
            p->isCamOpen = true;
        }
        camGc032aGetId(sensorID, 2);
        OSI_LOGI(0x10007d67, "cam get id 0x%x,0x%x", sensorID[0], sensorID[1]);
        if ((p->sensorid[0] == sensorID[0]) && (p->sensorid[1] == sensorID[1]))
        {
            OSI_LOGI(0x10007d68, "check id successful");
            camGc032aClose();
            return true;
        }
        else
        {
            camGc032aClose();
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

void camGc032aCaptureImage(uint32_t size)
{
    sensorInfo_t *p = &gc032aInfo;
    if (size != 0)
    {
        cameraIrqCause_t mask = {0, 0, 0, 0};
        drvCamSetIrqMask(mask);
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

void camGc032aPrevStart(uint32_t size)
{
    sensorInfo_t *p = &gc032aInfo;
    OSI_LOGI(0x10007d7f, "camGc032aPrevStart p->preview_page=%d ", p->preview_page);
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
        drvCamSetIrqMask(mask);
        drvCameraControllerEnable(true);
    }
}

void camGc032aPrevNotify(void)
{
    sensorInfo_t *p = &gc032aInfo;
    if (p->isCamOpen && !p->isStopPrev)
    {
        if (p->preview_page == 0)
        {
            cameraIrqCause_t mask = {0, 0, 0, 0};

            p->camCapStatus = campPreviewState1;
            p->preview_page++;
            mask.fend = 1;
            drvCamSetIrqMask(mask);
        }
#ifndef CONFIG_CAMERA_SINGLE_BUFFER
        else
        {
            if (p->preview_page < 2)
                p->preview_page++;
        }
#endif
    }
}

void camGc032aStopPrev(void)
{
    sensorInfo_t *p = &gc032aInfo;
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

void camGc032aSetFalsh(uint8_t level)
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

void camGc032aBrightness(uint8_t level)
{
}

void camGc032aContrast(uint8_t level)
{
}

void camGc032aImageEffect(uint8_t effect_type)
{
}
void camGc032aEv(uint8_t level)
{
}

void camGc032aAWB(uint8_t mode)
{
}

void camGc032aGetSensorInfo(sensorInfo_t **pSensorInfo)
{
    OSI_LOGI(0x10007d80, "CamGc032aGetSensorInfo %08x", &gc032aInfo);
    *pSensorInfo = &gc032aInfo;
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
sensorInfo_t *quec_getGc032aInfo()
{
    sensorInfo_t *Info = &gc032aInfo;
    return Info;
}
#endif

bool camGc032aReg(SensorOps_t *pSensorOpsCB)
{
    uint8_t ret;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
    ret = quec_camGc032aCheckId();
#else
    ret = camGc032aCheckId();
#endif
    if (ret)
    {
        pSensorOpsCB->cameraOpen = camGc032aOpen;
        pSensorOpsCB->cameraClose = camGc032aClose;
        pSensorOpsCB->cameraGetID = camGc032aGetId;
        pSensorOpsCB->cameraCaptureImage = camGc032aCaptureImage;
        pSensorOpsCB->cameraStartPrev = camGc032aPrevStart;
        pSensorOpsCB->cameraPrevNotify = camGc032aPrevNotify;
        pSensorOpsCB->cameraStopPrev = camGc032aStopPrev;
        pSensorOpsCB->cameraSetAWB = camGc032aAWB;
        pSensorOpsCB->cameraSetBrightness = camGc032aBrightness;
        pSensorOpsCB->cameraSetContrast = camGc032aContrast;
        pSensorOpsCB->cameraSetEv = camGc032aEv;
        pSensorOpsCB->cameraSetImageEffect = camGc032aImageEffect;
        pSensorOpsCB->cameraGetSensorInfo = camGc032aGetSensorInfo;
        pSensorOpsCB->cameraFlashSet = camGc032aSetFalsh;
        return true;
    }
    return false;
}
