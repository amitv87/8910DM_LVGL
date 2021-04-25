#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
//#include "cfw.h"

#define VERSION uint8_t
typedef enum
{
    Idle,
    Downloading,
    Downloaded,
    Updating,
} STATUS;

bool getVersion(VERSION ver1, VERSION ver2);
int32_t getBatteryLevel()
{
    //zhangyi del for porting 20180709
    //CFW_EMOD_BATTERY_INFO pBatInfo;

    //CFW_EmodGetBatteryInfo(&pBatInfo);
    //return pBatInfo.nChargeInfo.nBcl;
    return 100;
}

int32_t getAvailableSize();
uint32_t getSavedBytes();
bool changeStatus(STATUS status);
int32_t writeBytes(uint8_t *addr, uint8_t *buffer, uint32_t size, uint32_t offset);
bool update();
