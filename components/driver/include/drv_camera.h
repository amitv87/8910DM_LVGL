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

#ifndef _DRV_CAMP_H_
#define _DRV_CAMP_H_

OSI_EXTERN_C_BEGIN

#include <stdint.h>
#include "drv_i2c.h"
#include "drv_ifc.h"
#include "hal_chip.h"
#include "osi_api.h"

typedef struct
{
    uint8_t addr;
    uint8_t data;
} cameraReg_t;

typedef struct
{
    uint8_t overflow : 1;
    uint8_t fstart : 1;
    uint8_t fend : 1;
    uint8_t dma : 1;
} cameraIrqCause_t;

typedef enum
{
    //CAM_NPIX_UXGA = (1600*1200),
    CAM_NPIX_VGA = (640 * 480),
} CAM_SIZE_T;

// enum: image format about sensor output
typedef enum
{
    CAM_FORMAT_RGB565,
    CAM_FORMAT_YUV,
    CAM_FORMAT_RAW8,
    CAM_FORMAT_RAW10,
} cameraImageFormat_t;

typedef enum
{
    SPI_MODE_NO = 0,
    SPI_MODE_SLAVE,
    SPI_MODE_MASTER1,
    SPI_MODE_MASTER2_1,
    SPI_MODE_MASTER2_2,
    SPI_MODE_MASTER2_4,
    SPI_MODE_UNDEF,
} camSpiMode_t;

typedef enum
{
    SPI_OUT_Y0_U0_Y1_V0 = 0,
    SPI_OUT_Y0_V0_Y1_U0,
    SPI_OUT_U0_Y0_V0_Y1,
    SPI_OUT_U0_Y1_V0_Y0,
    SPI_OUT_V0_Y1_U1_Y0,
    SPI_OUT_V0_Y0_U0_Y1,
    SPI_OUT_Y1_V0_Y0_U0,
    SPI_OUT_Y1_U0_Y0_V0,
} camSpiYuv_t;

typedef enum
{
    camCaptureIdle = 0,
    camCaptureState1,
    camCaptureState2,
    campPreviewState1,
    campPreviewState2,
} camCapture_t;

typedef enum
{
    ROW_RATIO_1_1,
    ROW_RATIO_1_2,
    ROW_RATIO_1_3,
    ROW_RATIO_1_4,
} camRowRatio_t;

typedef enum
{
    COL_RATIO_1_1,
    COL_RATIO_1_2,
    COL_RATIO_1_3,
    COL_RATIO_1_4,
} camColRatio_t;

typedef enum
{
    camSpi_In,
    camCsi_In,
} camInputSensor_t;

typedef struct sensorInfo
{
    const char *name; // name of sensor
    drvI2cBps_t baud;
    uint8_t salve_i2c_addr_w;    // salve i2c write address
    uint8_t salve_i2c_addr_r;    // salve i2c read address
    uint8_t reg_addr_value_bits; // bit0: 0: i2c register value is 8 bit, 1: i2c register value is 16 bit
    uint8_t sensorid[2];
    uint16_t spi_pixels_per_line;   // max width of source image
    uint16_t spi_pixels_per_column; // max height of source image

    uint16_t rstActiveH;    // 1: high level valid; 0: low level valid
    uint16_t rstPulseWidth; // Unit: ms. Less than 200ms
    uint16_t pdnActiveH;    // 1: high level valid; 0: low level valid
    uint16_t dstWinColStart;
    uint16_t dstWinColEnd;
    uint16_t dstWinRowStart;
    uint16_t dstWinRowEnd;
    uint16_t spi_ctrl_clk_div;
    uint32_t i2c_name;
    uint32_t nPixcels;
    uint8_t captureDrops;
    uint8_t preview_page;
    uint8_t page_turn;
    uint8_t *capturedata;
    uint8_t *previewdata[2];
    bool isFirstFrame;
    bool isStopPrev;
    bool isFirst;
    bool isCamIfcStart;
    bool scaleEnable;
    bool cropEnable;
    bool dropFrame;
    bool spi_href_inv;
    bool spi_little_endian_en;
    bool isCapture;
    bool isCamOpen;
    bool ddr_en;
    cameraVDD_t avdd_val; // voltage of avdd
    cameraVDD_t dvdd_val;
    cameraClk_t sensorClk;
    camRowRatio_t rowRatio;
    camColRatio_t colRatio;
    cameraImageFormat_t image_format; // define in SENSOR_IMAGE_FORMAT_E enum,
    camSpiMode_t camSpiMode;
    camSpiYuv_t camYuvMode;
    camCapture_t camCapStatus;
    camInputSensor_t sensorType;
    drvIfcChannel_t *camp_ipc;
    drvI2cMaster_t *i2c_p;
    osiSemaphore_t *cam_sem_capture;
    osiSemaphore_t *cam_sem_preview;

} sensorInfo_t;

void unittestCam(void);
void drvCamSetPdn(bool pdnActivH);
void drvCamSetRst(bool rstActiveH);
void drvCampRegInit(sensorInfo_t *config);
void drvCamClrIrqMask(void);
void drvCamSetIrqMask(cameraIrqCause_t mask);
void drvCamCmdSetFifoRst(void);
void drvCamSetIrqHandler(osiIrqHandler_t hanlder, void *ctx);
void drvCamDisableIrq(void);
void drvCamInitIfc(void);
void drvCampStartTransfer(uint32_t size, uint8_t *data);
bool drvWaitCamIfcDone(void);
void drvCamStopIfc(void);
bool drvCampStopTransfer(uint32_t size, uint8_t *data);
void drvCampIfcDeinit(void);
void drvCamDisableMCLK(void);
void drvCamSetMCLK(cameraClk_t Mclk);
void drvCameraControllerEnable(bool enable);
void drvCampRegDeInit(void);
void drvCamSetPowerLevel(uint32_t id, cameraVDD_t mv);

OSI_EXTERN_C_END

#endif
