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

#ifndef _IMAGE_SENSOR_H_
#define _IMAGE_SENSOR_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include "drv_camera.h"
#include "drv_config.h"

typedef struct
{
    bool (*cameraOpen)(void);
    void (*cameraClose)(void);
    void (*cameraGetID)(uint8_t *data, uint8_t len);
    void (*cameraSetBrightness)(uint8_t level);
    void (*cameraSetContrast)(uint8_t level);
    void (*cameraSetImageEffect)(uint8_t effect_type);
    void (*cameraSetEv)(uint8_t level);
    void (*cameraSetAWB)(uint8_t mode);
    void (*cameraCaptureImage)(uint32_t size);
    void (*cameraStartPrev)(uint32_t size);
    void (*cameraStopPrev)(void);
    void (*cameraPrevNotify)(void);
    void (*cameraGetSensorInfo)(sensorInfo_t **pSensorInfo);
    void (*cameraFlashSet)(uint8_t level);
} SensorOps_t;

//general sensor drv need ping-pang buffer for continus data mode
#ifdef CONFIG_CAMERA_SINGLE_BUFFER
#define SENSOR_FRAMEBUFFER_NUM (1)
#else
#define SENSOR_FRAMEBUFFER_NUM (2)
#endif

typedef struct image_dev_tag
{
    bool poweron_flag;
    uint32_t nPixcels;
    uint8_t framebuffer_count;
    sensorInfo_t *pSensorInfo;
    SensorOps_t *pSensorOpsApi;
    uint16_t *pFramebuffer[SENSOR_FRAMEBUFFER_NUM];
} IMAGE_DEV_T, *IMAGE_DEV_T_PTR;

typedef struct cam_dev_tag
{
    char *pNamestr;
    uint32_t img_width;
    uint32_t img_height;
    uint32_t nPixcels;
} CAM_DEV_T, *CAM_DEV_T_PTR;

bool drvCamInit(void);
void drvCamGetSensorInfo(CAM_DEV_T *pCamDevice);
bool drvCamPowerOn(void);
bool drvCamStartPreview(void);
uint16_t *drvCamPreviewDQBUF(void);
void drvCamPreviewQBUF(uint16_t *pFrameBuf);
bool drvCamStopPreview(void);
bool drvCamCaptureImage(uint16_t **pFrameBuf);
bool drvCamGePrevStatus(void);
void drvCamClose(void);


#ifdef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
SensorOps_t *prvCamLoad(void);
IMAGE_DEV_T *quec_getImageSensor(void);

#endif

OSI_EXTERN_C_END

#endif
