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

#include "pin_reg.h"
#include "drv_gpio.h"
#include "hwregs.h"
#include "cfw.h"
#include "hal_adi_bus.h"
#include <drv_names.h>
#include <drv_uart.h>
#include <drv_axidma.h>
#include <osi_api.h>
#include "diag.h"
#include "string.h"
#include <stdlib.h>
#include "../cmddef.h"
#include "drv_adc.h"
#include "osi_compiler.h"

#define BBAT_NAME OSI_MAKE_TAG('B', 'B', 'A', 'T')
#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('B', 'B', 'A', 'T')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#include <osi_log.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BBAT_DELAYUS(us) osiDelayUS(us)

#define BBAT_FAILURE 0x01
#define BBAT_SUCCESS 0x00
#define AUTOTEST_NVRAM_LEN 8

//static osiPmSource_t *gBbatPmSource = NULL;
//static char start_message[] = "bbat-uart-test\n";

typedef uint32_t (*DIAG_AUTOTEST_CALLBACK)(
    const uint8_t *src_ptr,
    uint16_t src_len);

typedef enum _DEVICE_AUTOTEST_ID_E
{
    DEVICE_AUTOTEST_KEYPAD = 1,
    DEVICE_AUTOTEST_LCD_SPI = 3,
    DEVICE_AUTOTEST_CAMERA_IIC = 4,
    DEVICE_AUTOTEST_CAMERA_SPI = 6,
    DEVICE_AUTOTEST_GPIO = 7, //and TP test
    DEVICE_AUTOTEST_SIM = 9,
    DEVICE_AUTOTEST_MIC = 10,
    DEVICE_AUTOTEST_SPEAK = 11, //speak&&receiver&&earphone
    DEVICE_AUTOTEST_UART = 12,  //DEVICE_AUTOTEST_MISC = 12,  //lcd backlight,vibrator,keypadbacklight
    DEVICE_AUTOTEST_FM = 13,
    DEVICE_AUTOTEST_IIC_DEV = 17, //speak&&receiver&&earphone
    DEVICE_AUTOTEST_CHARGE = 18,
    DEVICE_AUTOTEST_NVRAM_R = 19,
    DEVICE_AUTOTEST_NVRAM_W = 20,
    DEVICE_AUTOTEST_ADC = 24,
    DEVICE_AUTOTEST_CHECK = 28,
    DEVICE_AUTOTEST_VIB = 30,
    DEVICE_AUTOTEST_USB = 31,
    DEVICE_AUTOTEST_MAX_F
} DEVICE_AUTOTEST_ID_E;

typedef enum
{
    MN_DUAL_SYS_1 = 0,
    MN_DUAL_SYS_2 = 1,
    MN_DUAL_SYS_MAX
} MN_DUAL_SYS_E;

typedef enum
{
    KEYPAD_KEYIN0 = 0x01,
    KEYPAD_REST_N,
    KEYPAD_EXT_RSTN_IN,
    KEYPAD_PWR

} AUTOTEST_KEYPAD_E;

typedef enum
{
    SIM_OK = 0x00,
    SIM_FAIL
} AUTOTEST_SIM_E;

typedef enum
{
    ADC_GET_VALUE = 0x0,
    ADC_GET_RAW
} AUTOTEST_ADC_E;

typedef enum
{
    VIB_OPEN = 0x01,
    VIB_CLOSE
} AUTOTEST_VIB_E;

typedef enum
{
    USB_OTG_DISABLE = 0x01,
    USB_READ_ID = 0x2,
    USB_VBUS_ON = 0x3,
    USB_VBUS_OFF
} AUTOTEST_USB_E;

typedef enum
{
    GPIO_GET_VALUE = 0x0,
    GPIO_SET_VALUE = 0x1,
    GPIO_SETBACK = 0x2
} AUTOTEST_GPIO_E;

/**---------------------------------------------------------------------------*
 **                         Data Structures                                   *
 **---------------------------------------------------------------------------*/
typedef struct
{
    diagMsgHead_t msg_head;
    uint8_t sim_select;
} diagMsgSim_t;

typedef struct
{
    diagMsgHead_t msg_head;
    uint8_t command;
    uint8_t gpio_id;
    uint8_t gpio_value;
    uint8_t reserved;
} diagMsgGPIO_t;

typedef struct
{
    diagMsgHead_t msg_head;
    uint8_t channel;
    uint8_t scale;
} diagMsgADC_t;

typedef struct
{
    diagMsgHead_t msg_head;
    uint16_t factory_name;
    uint16_t worker_name;
    uint16_t work_name;
    uint16_t reserved;
} diagMsgNvram_t;

typedef struct
{
    uint8_t pin_num;
    uint8_t gpio_real; // real gpio
    volatile uint32_t *reg;
    uint32_t value;
} autoTestPmPinFunc_t;

#define PIN_NUM_MAX 67
const autoTestPmPinFunc_t autodev_pm_func[] =
    {
        {PIN_GPIO4, 4, &hwp_iomux->pad_gpio_4_cfg, 0},
    };

autoTestPmPinFunc_t autodev_restore[PIN_NUM_MAX];

//static uint16_t keypadcode = 0;

static DIAG_AUTOTEST_CALLBACK diagAutotestCallback[DEVICE_AUTOTEST_MAX_F] = {NULL};

static void _restoreIomuxValue(void)
{
    uint8_t k;

    memcpy((uint8_t *)&autodev_restore[0], (uint8_t *)&autodev_pm_func[0], sizeof(autodev_pm_func));
    for (k = 0; k < PIN_NUM_MAX; k++)
    {
        autodev_restore[k].value = 0xffffffff;
    }
}

void _autotestGenerateRspMsg(diagMsgHead_t *msgHeadPtr) // Inputed pointer of message header struct.
{
    diagMsgHead_t head =
        {
            .seq_num = msgHeadPtr->seq_num,
            .len = sizeof(diagMsgHead_t),
            .type = msgHeadPtr->type,
            .subtype = msgHeadPtr->subtype,
        };
    //diagOutputPacket(&head, sizeof(head));
    diagOutputPacket2(&head, NULL, 0);
    return;
}
static uint8_t _getIndexOfGpio(uint8_t virtualGpio)
{
    uint8_t k;
    for (k = 0; k < OSI_ARRAY_SIZE(autodev_pm_func); k++)
    {
        if (virtualGpio == autodev_pm_func[k].pin_num)
            return k;
    }
    return 0xff;
}

// realGpio id will <=31,
static uint32_t _handleGpioAutotest(
    const uint8_t *src_ptr, // Pointer of the input message.
    uint16_t src_len        // Size of the source buffer in uint8_t.
)
{
    uint8_t value = 0, k;
    uint8_t virtuGpio = 0;
    uint8_t result = BBAT_SUCCESS;
    uint8_t realGpio = 0;

    diagMsgHead_t respMsg;

    diagMsgHead_t *msg_head = (diagMsgHead_t *)src_ptr;
    diagMsgGPIO_t *msgHeadGpio = (diagMsgGPIO_t *)src_ptr;
    OSI_LOGI(0, "bbat: _handleGpioAutotest");

    memcpy(&respMsg, msg_head, sizeof(diagMsgHead_t));

    if (msgHeadGpio->gpio_id > PIN_NUM_MAX)
    {
        result = BBAT_FAILURE;
        goto Fail;
    }
    virtuGpio = _getIndexOfGpio(msgHeadGpio->gpio_id);

    if (virtuGpio == 0xff)
    {
        return 0;
    }
    realGpio = autodev_pm_func[virtuGpio].gpio_real;

    _restoreIomuxValue();
    switch ((AUTOTEST_GPIO_E)(msgHeadGpio->command))
    {
    case GPIO_GET_VALUE:
        if ((autodev_pm_func[virtuGpio].reg) != NULL)
        {
            (autodev_restore[virtuGpio].value) = *(autodev_pm_func[virtuGpio].reg);
            *(autodev_pm_func[virtuGpio].reg) = (autodev_pm_func[virtuGpio].value);
        }

        hwp_gpio->gpio_oen_set_in_l = (1 << realGpio);
        value = hwp_gpio->gpio_val_l & (1 << realGpio);
        break;

    case GPIO_SET_VALUE:

        if ((autodev_pm_func[virtuGpio].reg) != NULL)
        {

            (autodev_restore[virtuGpio].value) = *(autodev_pm_func[virtuGpio].reg);

            *(autodev_pm_func[virtuGpio].reg) = (autodev_pm_func[virtuGpio].value);
        }

        if (msgHeadGpio->gpio_value)
            hwp_gpio->gpio_set_l = (1 << realGpio);
        else
            hwp_gpio->gpio_clr_l = (1 << realGpio);
        hwp_gpio->gpio_oen_set_out_l = (1 << realGpio);

        break;
    case GPIO_SETBACK:
    {
        for (k = 0; k < PIN_NUM_MAX; k++)
        {
            if (0xffffffff != autodev_restore[k].value)
            {

                if ((autodev_pm_func[virtuGpio].reg) != NULL)
                    *(autodev_restore[k].reg) = autodev_restore[k].value;
            }
        }
    }
    break;

    default:
        result = BBAT_FAILURE;
        break;
    }
Fail:
    if (BBAT_FAILURE == result)
    {
        msg_head->subtype = BBAT_FAILURE;
        _autotestGenerateRspMsg(msg_head);
    }
    else
    {
        respMsg.seq_num = msg_head->seq_num;
        respMsg.len = sizeof(diagMsgHead_t) + 1;
        respMsg.type = msg_head->type,
        respMsg.subtype = result,

        diagOutputPacket2(&respMsg, &value, 1);
    }
    return 1;
}

static uint32_t _handleAdcAutotest(
    const uint8_t *src_ptr, // Pointer of the input message.
    uint16_t src_len        // Size of the source buffer in uint8_t.
)
{
#if 0
    uint16_t value = 0;
    uint8_t result = BBAT_SUCCESS;

    diagMsgHead_t respMsg;
    diagMsgADC_t *msg_head = (diagMsgADC_t *)src_ptr;
    memcpy(&respMsg, msg_head, sizeof(diagMsgHead_t));
    OSI_LOGI(0, "bbat: _handleAdcAutotest");

    drvAdcInit();
    value = (uint16_t)drvAdcGetChannelVolt(msg_head->channel, msg_head->scale);
    OSI_LOGI(0, "bbat: _handleAdcAutotest,channel:%d,scale:%d", msg_head->channel, msg_head->scale);

    if (BBAT_FAILURE == result)
    {
        OSI_LOGI(0, "bbat: _handleAdcAutotest,failure");
        respMsg.subtype = BBAT_FAILURE;
        _autotestGenerateRspMsg(&respMsg);
    }
    else
    {
        OSI_LOGI(0, "bbat: _handleAdcAutotest,success");
        //respMsg.seq_num = msg_head->msg_head.seq_num;
        respMsg.len = sizeof(diagMsgHead_t) + 2;
        //respMsg.type = msg_head->msg_head.type;
        respMsg.subtype = result;

        diagOutputPacket2(&respMsg, &value, 2);
    }
#endif 
    return 1;
}


static uint32_t _handleSIMAutotest(
    const uint8_t *src_ptr, // Pointer of the input message.
    uint16_t src_len        // Size of the source buffer in uint8_t.
)
{
    AUTOTEST_SIM_E ret = 0;
    uint8_t sim_status = false; //, res;
    diagMsgHead_t respMsg;

    diagMsgSim_t *msg_head = (diagMsgSim_t *)src_ptr;

    memcpy(&respMsg, msg_head, sizeof(diagMsgHead_t));

    OSI_LOGI(0, "bbat: _handleSIMAutotest");
    switch ((MN_DUAL_SYS_E)(msg_head->sim_select))
    {
    case MN_DUAL_SYS_1:
        sim_status = CFW_GetSimStatus(0);
        if ((sim_status == 1) || (sim_status == 2))
        {
            ret = SIM_OK;
        }
        else
        {
            ret = SIM_FAIL;
        }
        break;

    case MN_DUAL_SYS_2:
        sim_status = CFW_GetSimStatus(1);
        if ((sim_status == 1) || (sim_status == 2))
        {
            ret = SIM_OK;
        }
        else
        {
            ret = SIM_FAIL;
        }
        break;

    default:
        ret = SIM_FAIL;
        break;
    }

    respMsg.len = sizeof(diagMsgHead_t) + 1;
    respMsg.subtype = ret;
    //res = (uint8_t)ret;
    diagOutputPacket2(&respMsg, NULL, 0);

    return 1;
}

static uint32_t _handleAutotestCheck(
    const uint8_t *src_ptr, // Pointer of the input message.
    uint16_t src_len        // Size of the source buffer in uint8_t.
)
{
    diagMsgHead_t *msg_head = (diagMsgHead_t *)src_ptr;

    diagMsgHead_t respMsg;
    memcpy(&respMsg, msg_head, sizeof(diagMsgHead_t));
    respMsg.subtype = BBAT_SUCCESS;
    _autotestGenerateRspMsg(&respMsg);
    return 1;
}

uint32_t _autotestReadTestResult(void *result_buf_ptr, uint32_t buf_len)
{
    return BBAT_SUCCESS;
}

static int32_t _autotestReadNV(uint8_t *buffer)
{
    uint8_t nvitem[64] = {0};
    uint32_t nv_length = 64;
    uint32_t return_code = BBAT_SUCCESS;

    return_code = _autotestReadTestResult((void *)nvitem, nv_length);

    if (return_code == BBAT_SUCCESS)
    {
        memcpy(buffer, nvitem + sizeof(nvitem) - AUTOTEST_NVRAM_LEN, AUTOTEST_NVRAM_LEN);
    }

    return return_code;
}

static uint32_t _autotestResultRead(
    const uint8_t *src_ptr, // Pointer of the input message.
    uint16_t src_len        // Size of the source buffer in uint8_t.
)
{
    uint8_t result = 0;
    uint8_t read_data[AUTOTEST_NVRAM_LEN + 1];
    diagMsgHead_t *msg_head = (diagMsgHead_t *)src_ptr;

    diagMsgHead_t respMsg;
    memcpy(&respMsg, msg_head, sizeof(diagMsgHead_t));
    result = _autotestReadNV(read_data);
    if (BBAT_SUCCESS == result)
    {
        //respMsg.seq_num = msg_head->seq_num;
        respMsg.len = sizeof(diagMsgHead_t) + AUTOTEST_NVRAM_LEN;
        //respMsg.type = msg_head->type;
        respMsg.subtype = BBAT_SUCCESS;

        diagOutputPacket2(&respMsg, &read_data, AUTOTEST_NVRAM_LEN);
    }
    else
    {
        respMsg.subtype = BBAT_FAILURE;
        _autotestGenerateRspMsg(&respMsg);
    }
    return 1;
}

static int32_t _autotestWriteNV(uint8_t *buffer)
{
    return BBAT_SUCCESS;
}

static uint32_t _autotestResultWrite(
    const uint8_t *src_ptr, // Pointer of the input message.
    uint16_t src_len        // Size of the source buffer in uint8_t.
)
{
    uint8_t result = BBAT_SUCCESS;
    uint8_t read_data[AUTOTEST_NVRAM_LEN + 1] = {0};
    diagMsgHead_t *msg_head = (diagMsgHead_t *)src_ptr;
    diagMsgHead_t respMsg;
    memcpy(&respMsg, msg_head, sizeof(diagMsgHead_t));
    memcpy(read_data, (uint8_t *)(src_ptr + sizeof(diagMsgHead_t)), sizeof(diagMsgNvram_t) - sizeof(diagMsgHead_t));
    result = _autotestWriteNV((uint8_t *)read_data);

    if (BBAT_SUCCESS != result)
    {
        result = BBAT_FAILURE;
    }
    respMsg.subtype = result;
    _autotestGenerateRspMsg(&respMsg);
    return 1;
}

static void _diagInitAutotestCallback(void)
{
    int type = 0;

    for (type = 0; type < DEVICE_AUTOTEST_MAX_F; type++)
    {
        diagAutotestCallback[type] = NULL;
    }
}

static void _diagRegisterAutotestCallback(uint32_t type, DIAG_AUTOTEST_CALLBACK routine)
{
    if (type < DEVICE_AUTOTEST_MAX_F)
        diagAutotestCallback[type] = routine;
}

static bool _handleDeviceAutotest(const diagMsgHead_t *cmd, void *ctx)
{
    uint8_t cmd_subtype;
    cmd_subtype = cmd->subtype;

    if (cmd_subtype < DEVICE_AUTOTEST_MAX_F)
    {
        if (NULL != diagAutotestCallback[cmd_subtype])
        {
            if (diagAutotestCallback[cmd_subtype]((uint8_t *)cmd, cmd->len)) // has been handled.
            {
                return true;
            }
        }
    }
    return true;
}






void _drvRegisterDeviceAutoTestCmdRoutine(void)
{
    OSI_LOGI(0, "bbat: Register Cmd Routine!");

    _diagInitAutotestCallback();


    _diagRegisterAutotestCallback(DEVICE_AUTOTEST_GPIO, _handleGpioAutotest);
    _diagRegisterAutotestCallback(DEVICE_AUTOTEST_SIM, _handleSIMAutotest);
    _diagRegisterAutotestCallback(DEVICE_AUTOTEST_ADC, _handleAdcAutotest);

    _diagRegisterAutotestCallback(DEVICE_AUTOTEST_NVRAM_R, _autotestResultRead);
    _diagRegisterAutotestCallback(DEVICE_AUTOTEST_NVRAM_W, _autotestResultWrite);
    _diagRegisterAutotestCallback(DEVICE_AUTOTEST_CHECK, _handleAutotestCheck);
}

void diagAutoTestInit(void)
{
    OSI_LOGI(0, "bbat: diagAutoTestInit ");

    // gBbatPmSource = osiPmSourceCreate(BBAT_NAME, NULL, NULL);
    // osiPmWakeLock(gBbatPmSource);

    _drvRegisterDeviceAutoTestCmdRoutine();
    diagRegisterCmdHandle(DIAG_DEVICE_AUTOTEST_F, _handleDeviceAutotest, NULL);
}

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
