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

#include "osi_api.h"
#include "osi_log.h"

#include "hwregs.h"
#include "hal_chip.h"
#include "hal_config.h"

// =============================================================================
//  MACROS
// =============================================================================

#define hwp_Ifc hwp_audioIfc
#define CHIP_HAS_AUDIO_IFC
#define WITHOUT_AIF_TX_STB

#define AIF_RECORD_INTERUPT_ID SYS_IRQ_ID_AIF_APB_0
#define AIF_PLAY_INTERUPT_ID SYS_IRQ_ID_AIF_APB_1

#define AIF_SOURCE_CLOCK 48000000

#define VOICE_BCK_LRCK_RATIO_MIN 16
#define VOICE_BCK_LRCK_RATIO_MAX 47
#define AUDIO_BCK_LRCK_RATIO_MIN 32
#define AUDIO_BCK_LRCK_RATIO_MAX 94

/// Indices for the user handler array.
#define RECORD 0
#define PLAY 1
#define AIF2RECORD 2
#define AIF2PLAY 3

#define INT8 int8_t
#define UINT8 uint8_t
#define INT16 int16_t
#define UINT16 uint16_t
#define INT32 int32_t
#define UINT32 uint32_t
#define BOOL bool
#define VOID void
#define CONST
#define PUBLIC
#define SRVAPI
#define PROTECTED
#define LOCAL static
#define PRIVATE static

#define TRUE 1
#define FALSE 0

#define hal_SysEnterCriticalSection() osiEnterCritical()
#define hal_SysExitCriticalSection(critical) osiExitCritical(critical)

#include "cpheaders/hal_error.h"
#include "cpheaders/hal_aif.h"

// =============================================================================
// HAL_AIF_XFER_HANDLER_T
// -----------------------------------------------------------------------------
/// Type privately used to store both handler for each direction
// =============================================================================
typedef struct
{
    HAL_AIF_HANDLER_T halfHandler;
    HAL_AIF_HANDLER_T endHandler;
} HAL_AIF_XFER_HANDLER_T;

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
/// Kept in mind the openness status of the AIF
PRIVATE BOOL g_halAifOpened = FALSE;

/// User handler called when the playing reaches the middle of the buffer
PRIVATE HAL_AIF_XFER_HANDLER_T g_halAifHandlers[4] =
    {{(void *)NULL, (void *)NULL}, {(void *)NULL, (void *)NULL}, {(void *)NULL, (void *)NULL}, {(void *)NULL, (void *)NULL}};

/// Global variable to remember the state of the Rx and Tx interfaces, to
/// deal with a hardware feature ...
PRIVATE BOOL g_halAifPlaying = FALSE;
PRIVATE BOOL g_halAifRecording = FALSE;

/// Use to store the constants in the control register
PRIVATE UINT32 g_halAifControlReg = 0;

PRIVATE HAL_AIF_IF_T g_halAifActiveDai[HAL_AIF_ID_QTY] = {HAL_AIF_IF_QTY, HAL_AIF_IF_QTY};

UINT32 g_AUD_TOP_CTL = 0;
UINT32 g_Cfg_AIF_CFG = 0;
UINT32 g_Cfg_MISC_CFG = 0;

PRIVATE BOOL g_halUseAif2Flag = FALSE;
PUBLIC UINT32 g_halPingPangFlag = 0;

// =============================================================================
// hal_AifSetAif2Flag
// -----------------------------------------------------------------------------
/// Set the aif2 flag.
// =============================================================================
PUBLIC VOID hal_AifSetAif2Flag(BOOL flag)
{
    g_halUseAif2Flag = flag;
}

PUBLIC VOID hal_AifSetI2sGpio(VOID)
{
    //i2s
    hwp_iomux->pad_gpio_0_cfg_reg |= (0x3);
    hwp_iomux->pad_gpio_1_cfg_reg |= (0x3);
    hwp_iomux->pad_gpio_2_cfg_reg |= (0x3);
    hwp_iomux->pad_gpio_3_cfg_reg |= (0x3);
}

PUBLIC BOOL hal_AifIsDaiBusy(HAL_AIF_IF_T aif_if)
{
    if ((aif_if & g_halAifActiveDai[0]) && (g_halAifActiveDai[0] != HAL_AIF_IF_QTY))
        return TRUE;
    else
        return FALSE;
}

PUBLIC VOID hal_AifSetAifApbClk(BOOL on)
{
    if (on)
        hwp_sysCtrl->clk_aif_apb_enable |= SYS_CTRL_ENABLE_AIF_APB_CLK_ID_AUD_2AD;
    else
        hwp_sysCtrl->clk_aif_apb_disable |= SYS_CTRL_DISABLE_AIF_APB_CLK_ID_AUD_2AD;
}

PUBLIC VOID hal_AifSetAifOtherClk(BOOL on)
{
    if (on)
        hwp_sysCtrl->clk_others_enable |= SYS_CTRL_ENABLE_OC_ID_BCK1;
    else
        hwp_sysCtrl->clk_others_disable |= SYS_CTRL_DISABLE_OC_ID_BCK1;
}

// =============================================================================
// hal_newAifSetSampleRate
// -----------------------------------------------------------------------------
/// Set the SampleRate.
// =============================================================================

PUBLIC HAL_ERR_T hal_newAifSetSampleRate(CONST HAL_AIF_CONFIG_T *config, HAL_AIF_FREQ_T sampleRate, BOOL isPlay)
{
    UINT32 DAC_SRC_CTL = 0;
    UINT32 ADC_SRC_CTL = 0;
    UINT32 DAC_SRC_STEP = 0;

    OSI_LOGI(0, "hal_newAifSetSampleRate,:interface=%d,sampleRate=%d,isplay=%d\n",
             config->interface, sampleRate, isPlay);

    if (isPlay)
    {
        if ((config->interface == HAL_AIF_IF_PARALLEL) || (config->interface == HAL_AIF_IF_PARALLEL_STEREO) ||
            (config->interface == HAL_AIF_IF_SERIAL_IN_PARALLEL_OUT) || (config->interface == HAL_AIF_IF_SERIAL_1_IN_PARALLEL_OUT) ||
            (config->interface == HAL_AIF_IF_SERIAL_2_IN_PARALLEL_OUT))
        {
            switch (sampleRate)
            {
            // AIF_SOURCE_CLOCK = 24576000
            case HAL_AIF_FREQ_8000HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(10);
                DAC_SRC_STEP = 2 * 160;
                break;
            case HAL_AIF_FREQ_9600HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(9);
                DAC_SRC_STEP = 24 * 16;
                break;
            case HAL_AIF_FREQ_11025HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(8);
                DAC_SRC_STEP = 3 * 147;
                break;
            case HAL_AIF_FREQ_12000HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(7);
                DAC_SRC_STEP = 3 * 160;
                break;
            case HAL_AIF_FREQ_16000HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(6);
                DAC_SRC_STEP = 4 * 160;
                break;
            case HAL_AIF_FREQ_22050HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(5);
                DAC_SRC_STEP = 6 * 147;
                break;
            case HAL_AIF_FREQ_24000HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(4);
                DAC_SRC_STEP = 6 * 160;
                break;
            case HAL_AIF_FREQ_32000HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(3);
                DAC_SRC_STEP = 8 * 160;
                break;
            case HAL_AIF_FREQ_44100HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(2);
                DAC_SRC_STEP = 12 * 147;
                break;
            case HAL_AIF_FREQ_48000HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(1);
                DAC_SRC_STEP = 12 * 160;
                break;
            case HAL_AIF_FREQ_96000HZ:
                DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(0);
                DAC_SRC_STEP = 24 * 160;
                break;
            default:
                //OSI_ASSERT(FALSE, "Invalid sample rate: %d", sampleRate);
                break;
            }

            hwp_aud2ad->dac_src_ctl &= ~(AUD_2AD_DAC_FS_MODE(0xf));
            hwp_aud2ad->dac_src_ctl |= DAC_SRC_CTL;

            hwp_aud2ad->dac_src_step = DAC_SRC_STEP;
        }
    }

    else
    {
        if ((config->interface == HAL_AIF_IF_PARALLEL) || (config->interface == HAL_AIF_IF_PARALLEL_STEREO))
        {
            ADC_SRC_CTL = AUD_2AD_ADC_SRC_N(sampleRate / 1000 / 4) | AUD_2AD_ADC1_SRC_N(sampleRate / 1000 / 4);
        }

        hwp_aud2ad->adc_src_ctl = ADC_SRC_CTL;
    }
    return HAL_ERR_NO;
}

PUBLIC HAL_ERR_T hal_newAif1Cofig(CONST HAL_AIF_CONFIG_T *config)
{
    UINT32 controlReg = 0;
    UINT32 serialCfgReg = 0;
    UINT32 lrck;
    UINT32 bck;
    UINT32 clkDivider;
    UINT32 bckLrckDivider;
    UINT32 bParallel = FALSE;
    UINT32 DAC_SRC_CTL = 0;
    UINT32 ADC_SRC_CTL = 0;
    UINT32 DAC_SRC_STEP = 0;

    // Pointer to the serial config
    HAL_AIF_SERIAL_CFG_T *serialCfg;

    OSI_LOGI(0, "hal_newAif1Cofig interface:%d\n", config->interface);
    OSI_LOGI(0, "hal_newAif1Cofig sampleRate:%d\n", config->sampleRate);
    OSI_LOGI(0, "hal_newAif1Cofig channelNb:%d\n", config->channelNb);

    //step 1 Clr aif  and aud top
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->reg_dbg = SYS_CTRL_PROTECT_UNLOCK;
    hal_AifSetAifApbClk(TRUE);

    hwp_sysCtrl->aif_apb_rst_clr = SYS_CTRL_CLR_AIF_APB_RST(SYS_CTRL_CLR_AIF1_APB_RST_ID_AIF);

    if ((config->interface == HAL_AIF_IF_PARALLEL) || (config->interface == HAL_AIF_IF_PARALLEL_STEREO) ||
        (config->interface == HAL_AIF_IF_SERIAL_IN_PARALLEL_OUT) || (config->interface == HAL_AIF_IF_SERIAL_1_IN_PARALLEL_OUT) ||
        (config->interface == HAL_AIF_IF_SERIAL_2_IN_PARALLEL_OUT))
    {
        hwp_sysCtrl->aif_apb_rst_clr = SYS_CTRL_CLR_AIF_APB_RST(SYS_CTRL_CLR_AIF_APB_RST_ID_AUD_2AD);
        bParallel = TRUE;
    }
    //maybe need seting some clk to auto gate
    hal_SysExitCriticalSection(status);

    //step 2, seting dai and aif interface
    //FM and AUD_TOP mux selecte
    g_AUD_TOP_CTL = hwp_aud2ad->aud_top_ctl;

    if ((config->interface == HAL_AIF_IF_PARALLEL) || (config->interface == HAL_AIF_IF_PARALLEL_STEREO))
    {
        ADC_SRC_CTL = AUD_2AD_ADC_SRC_N(config->sampleRate / 1000 / 4) | AUD_2AD_ADC1_SRC_N(config->sampleRate / 1000 / 4);
    }

    hwp_aud2ad->adc_src_ctl = ADC_SRC_CTL;

    if (bParallel)
    {
        switch (config->sampleRate)
        {
        // AIF_SOURCE_CLOCK = 24576000
        case HAL_AIF_FREQ_8000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(10);
            DAC_SRC_STEP = 2 * 160;
            break;
        case HAL_AIF_FREQ_9600HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(9);
            DAC_SRC_STEP = 24 * 16;
            break;
        case HAL_AIF_FREQ_11025HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(8);
            DAC_SRC_STEP = 3 * 147;
            break;
        case HAL_AIF_FREQ_12000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(7);
            DAC_SRC_STEP = 3 * 160;
            break;
        case HAL_AIF_FREQ_16000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(6);
            DAC_SRC_STEP = 4 * 160;
            break;
        case HAL_AIF_FREQ_22050HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(5);
            DAC_SRC_STEP = 6 * 147;
            break;
        case HAL_AIF_FREQ_24000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(4);
            DAC_SRC_STEP = 6 * 160;
            break;
        case HAL_AIF_FREQ_32000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(3);
            DAC_SRC_STEP = 8 * 160;
            break;
        case HAL_AIF_FREQ_44100HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(2);
            DAC_SRC_STEP = 12 * 147;
            break;
        case HAL_AIF_FREQ_48000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(1);
            DAC_SRC_STEP = 12 * 160;
            break;
        case HAL_AIF_FREQ_96000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(0);
            DAC_SRC_STEP = 24 * 160;
            break;
        default:
            //OSI_ASSERT(FALSE, "Invalid sample rate: %d", config->sampleRate);
            break;
        }

        hwp_aud2ad->dac_src_ctl &= ~(AUD_2AD_DAC_FS_MODE(0xf));
        hwp_aud2ad->dac_src_ctl |= DAC_SRC_CTL;

        hwp_aud2ad->dac_src_step = DAC_SRC_STEP;

        hwp_aud2ad->aud_top_ctl &= ~(AUD_2AD_ADC_IIS_SEL(3));
        hwp_aud2ad->aud_top_ctl &= ~(AUD_2AD_DAC_IIS_SEL(3));
        hwp_aud2ad->aud_top_ctl &= ~(AUD_2AD_ADC_SINC_IN_SEL(3));

        hwp_aud2ad->aud_top_ctl |= AUD_2AD_ADC_EN_R | AUD_2AD_ADC_EN_L | AUD_2AD_DAC_EN_R | AUD_2AD_DAC_EN_L | AUD_2AD_ADC1_EN_L | AUD_2AD_ADC1_EN_R | AUD_2AD_ADC_SINC_IN_SEL(0);
        if (config->PARALLEL_FOR_FM)
        {
            hwp_aud2ad->aud_top_ctl |= AUD_2AD_ADC_IIS_SEL(2) | AUD_2AD_DAC_IIS_SEL(2); //AIF1 use FM
            //g_AUD_TOP_CTL |= AUD_2AD_ADC_IIS_SEL(1)|AUD_2AD_DAC_IIS_SEL(2); //AIF2 use FM
        }
        else
        {
            hwp_aud2ad->aud_top_ctl |= AUD_2AD_ADC_IIS_SEL(1) | AUD_2AD_DAC_IIS_SEL(1); //aif1
            //g_AUD_TOP_CTL|= AUD_2AD_ADC_IIS_SEL(2)|AUD_2AD_DAC_IIS_SEL(1);//aif2
        }
        hwp_aud2ad->aud_clr = AUD_2AD_ADC_CLR | AUD_2AD_DAC_CLR | AUD_2AD_ADC1_CLR;
        //hal_TimDelay(1);
        osiDelayUS(1);
        hwp_aud2ad->aud_clr = 0;
        hwp_aud2ad->audif_fifo_ctl = AUD_2AD_ADC_FIFO_AF_LVL(3);

        if (config->channel == HAL_AIF_CHANNEL_MONO_R)
            hwp_aud2ad->aud_iis_ctl |= AUD_2AD_ADC_LR_SEL;
        else
            hwp_aud2ad->aud_iis_ctl &= ~(AUD_2AD_ADC_LR_SEL);
    }

    //Step3 AIF Input mux select
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_AIF1_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_AIF2_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S1_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S2_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S3_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S1_BCK_LRCK_OEN);
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S2_BCK_LRCK_OEN);
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S3_BCK_LRCK_OEN);

    if ((config->interface == HAL_AIF_IF_SERIAL) || (config->interface == HAL_AIF_IF_SERIAL_IN_PARALLEL_OUT))
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_AIF1_SEL(2) | SYS_CTRL_AIF2_SEL(7);
        //g_Cfg_AIF_CFG |=SYS_CTRL_AIF1_SEL(7)|SYS_CTRL_AIF2_SEL(2);
    }
    else if ((config->interface == HAL_AIF_IF_SERIAL_1) || (config->interface == HAL_AIF_IF_SERIAL_1_IN_PARALLEL_OUT))
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_AIF1_SEL(3) | SYS_CTRL_AIF2_SEL(7);
        //g_Cfg_AIF_CFG |=SYS_CTRL_AIF1_SEL(7)|SYS_CTRL_AIF2_SEL(3);
    }
    else if ((config->interface == HAL_AIF_IF_SERIAL_2) || (config->interface == HAL_AIF_IF_SERIAL_2_IN_PARALLEL_OUT))
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_AIF1_SEL(4) | SYS_CTRL_AIF2_SEL(7);
        OSI_LOGI(0, "hal_newAif1Cofig:HAL_AIF_IF_SERIAL_2,hwp_sysCtrl->Cfg_AIF_CFG:0x%x\n", hwp_sysCtrl->cfg_aif_cfg);
        //g_Cfg_AIF_CFG |=SYS_CTRL_AIF1_SEL(7)|SYS_CTRL_AIF2_SEL(4);
    }
    else
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_AIF1_SEL(7) | SYS_CTRL_AIF2_SEL(7);
        //g_Cfg_AIF_CFG |=SYS_CTRL_AIF1_SEL(7)|SYS_CTRL_AIF2_SEL(7);
    }

    //Step 4 AIF Output mux select
    hwp_sysCtrl->cfg_misc_cfg &= ~(SYS_CTRL_AIF1_I2S_BCK_SEL(7));
    OSI_LOGI(0, "hal_newAif1Cofig hwp_sysCtrl->Cfg_MISC_CFG:0x%x\n", hwp_sysCtrl->cfg_misc_cfg);

    if (config->interface == HAL_AIF_IF_SERIAL)
    {
        OSI_LOGI(0, "hal_newAif1Cofig: HAL_AIF_IF_SERIAL master:0x%x\n", config->serialCfg->aifIsMaster);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_SEL(0);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_SEL(7);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_SEL(7);

        if (config->serialCfg->aifIsMaster)
        {
            hwp_sysCtrl->cfg_aif_cfg &= (~SYS_CTRL_I2S1_BCK_LRCK_OEN); //i2s1 bck,lrck output enable
            //bit2 is 0, use clk 26M audio, bit2 is 1, use 133M pll(for debug)
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF1_I2S_BCK_SEL(3);
        }
        else //slave
        {
            hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_BCK_LRCK_OEN; //  i2s1 bck,lrck output disable
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF1_I2S_BCK_SEL(0);
        }
    }
    else if (config->interface == HAL_AIF_IF_SERIAL_1)
    {
        OSI_LOGI(0, "hal_newAif1Cofig:HAL_AIF_IF_SERIAL_1 master:0x%x\n", config->serialCfg->aifIsMaster);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_SEL(7);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_SEL(0);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_SEL(7);

        if (config->serialCfg->aifIsMaster)
        {
            hwp_sysCtrl->cfg_aif_cfg &= (~SYS_CTRL_I2S2_BCK_LRCK_OEN); //i2s2 bck,lrck output enable
            //bit2 is 0, use clk 26M audio, bit2 is 1, use 133M pll(for debug)
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF1_I2S_BCK_SEL(3);
        }
        else //slave
        {
            hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_BCK_LRCK_OEN; //  i2s2 bck,lrck output disable
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF1_I2S_BCK_SEL(1);
        }
    }
    else if (config->interface == HAL_AIF_IF_SERIAL_2)
    {
        OSI_LOGI(0, "hal_newAif1Cofig:HAL_AIF_IF_SERIAL_2 master:0x%x\n", config->serialCfg->aifIsMaster);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_SEL(7);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_SEL(7);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_SEL(0);

        if (config->serialCfg->aifIsMaster)
        {
            hwp_sysCtrl->cfg_aif_cfg &= (~SYS_CTRL_I2S3_BCK_LRCK_OEN); //i2s3 bck,lrck output enable
            //bit2 is 0, use clk 26M audio, bit2 is 1, use 133M pll(for debug)
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF1_I2S_BCK_SEL(3);
        }
        else
        {
            hal_AifSetAifOtherClk(TRUE);
            hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_BCK_LRCK_OEN;    //  i2s3 bck,lrck output disable
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF1_I2S_BCK_SEL(2); //from huapeng
        }
    }
    else
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_SEL(7);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_SEL(7);
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_SEL(7);
        OSI_LOGI(0, "hal_newAif1Cofig:barrel hwp_sysCtrl->Cfg_AIF_CFG:0x%x\n", hwp_sysCtrl->cfg_aif_cfg);

        //bit2 is 0, use clk 26M audio, bit2 is 1, use 133M pll(for debug)
        hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF1_I2S_BCK_SEL(3);
    }

    if (config->interface == HAL_AIF_IF_PARALLEL)
    {
        // Analog mode
        controlReg |= AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA | AIF_LOOP_BACK_NORMAL;
        serialCfgReg |= AIF_MASTER_MODE_MASTER; // Well, it's analog mode ...
#ifdef SUPPORT_DUAL_MIC
        controlReg |= AIF_PARALLEL_IN2_EN_ENABLE;
#endif
    }
    else if (config->interface == HAL_AIF_IF_PARALLEL_STEREO)
    {
#if (CHIP_HAS_STEREO_DAC == 1)
        controlReg |= AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA | AIF_LOOP_BACK_NORMAL | AIF_TX_STB_MODE;
        // Parallel stereo out expects stereo data
        if (config->channelNb == HAL_AIF_MONO)
        {
            serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_MONO_STEREO_DUPLI;
        }
        else
        {
            serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_STEREO_STEREO;
        }
#ifdef SUPPORT_DUAL_MIC
        controlReg |= AIF_PARALLEL_IN2_EN_ENABLE;
#endif

#else
        OSI_ASSERT(FALSE, "This chip has not the Parallel Stereo Interface.");
#endif
    }
    else
    {
        // Digital (serial) modes
        OSI_ASSERT(config->serialCfg != NULL, "hal_AifOpen in serial mode without serial config\n");

        if (config->interface == HAL_AIF_IF_SERIAL ||
            config->interface == HAL_AIF_IF_SERIAL_1 ||
            config->interface == HAL_AIF_IF_SERIAL_2)
        {
            // Serial in and serial out
            controlReg |= AIF_PARALLEL_OUT_CLR_PARA |
                          AIF_PARALLEL_IN_CLR_PARA |
                          AIF_LOOP_BACK_NORMAL;
        }
        else
        {
            // Serial in and parallel out
            controlReg |= AIF_PARALLEL_OUT_SET_PARA |
                          AIF_PARALLEL_IN_CLR_PARA |
                          AIF_LOOP_BACK_NORMAL |
                          AIF_TX_STB_MODE;
            // Parallel stereo out expects stereo data
            if (config->channelNb == HAL_AIF_MONO)
            {
                serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_MONO_STEREO_DUPLI;
            }
            else
            {
                serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_STEREO_STEREO;
            }
        }

        // Mode
        serialCfg = config->serialCfg;
        OSI_ASSERT((serialCfg->mode < HAL_SERIAL_MODE_QTY),
                   "Improper serial mode");
        serialCfgReg |= AIF_SERIAL_MODE(serialCfg->mode);
        OSI_LOGI(0, "hal_newAif1Cofig serialCfg->mode:%d\n", serialCfg->mode);

        // TODO FIXME Implement the clock configuration
        // here from the sample rate given at the aif opening

        // FIXME
        //select i2s0 ,1,2 interface, change to  Cfg_AIF_CFG in AUD TOP

        // Master
        if (serialCfg->aifIsMaster)
        {
            serialCfgReg |= AIF_MASTER_MODE_MASTER;
        }
        else
        {
            serialCfgReg |= AIF_MASTER_MODE_SLAVE;
        }

        // LSB first
        if (serialCfg->lsbFirst)
        {
            serialCfgReg |= AIF_LSB_LSB;
        }
        else
        {
            serialCfgReg |= AIF_LSB_MSB;
        }

        // LRCK polarity
        if (serialCfg->polarity)
        {
            serialCfgReg |= AIF_LRCK_POL_LEFT_L_RIGHT_H;
        }
        else
        {
            serialCfgReg |= AIF_LRCK_POL_LEFT_H_RIGHT_L;
        }

        // Rx delay
        OSI_ASSERT(serialCfg->rxDelay < HAL_AIF_RX_DELAY_QTY,
                   "Improper delay for serial Rx");
        serialCfgReg |= AIF_RX_DLY(serialCfg->rxDelay);

        // Tx delay
        if (serialCfg->txDelay == HAL_AIF_TX_DELAY_ALIGN)
        {
            serialCfgReg |= AIF_TX_DLY_ALIGN;
        }
        else
        {
            serialCfgReg |= AIF_TX_DLY_DLY_1;
        }

        // Rx Mode
        if (serialCfg->rxMode == HAL_AIF_RX_MODE_STEREO_STEREO)
        {
            serialCfgReg |= AIF_RX_MODE_STEREO_STEREO;
        }
        else
        {
            serialCfgReg |= AIF_RX_MODE_STEREO_MONO_FROM_L;
        }

        // Tx Mode
        OSI_ASSERT(serialCfg->txMode < HAL_AIF_TX_MODE_QTY,
                   "Improper mode for serial Tx");
        serialCfgReg |= AIF_TX_MODE(serialCfg->txMode);

        // LRCK frequency ...
        lrck = serialCfg->fs;
        bck = lrck * serialCfg->bckLrckRatio;

        if (serialCfg->aifIsMaster)
        {
            // note , the clk source is 26m, not is 24576000 or 2257920,   so the bck divider is vary rough, maby use   othe clk for bck out
            // We get the BCK from the the audio clock.
            clkDivider = (26000000 / bck);
            hwp_sysCtrl->cfg_clk_audiobck1_div = SYS_CTRL_AUDIOBCK1_DIVIDER(clkDivider);
            hwp_sysCtrl->cfg_clk_audiobck1_div = SYS_CTRL_AUDIOBCK1_DIVIDER(clkDivider) | SYS_CTRL_AUDIOBCK1_UPDATE;
        }

        // The ratio is calculated differently depending
        // on whether we're using voice or audio mode.
        if (serialCfg->mode == HAL_SERIAL_MODE_VOICE)
        {
            // Check the ratio
            OSI_LOGI(0, "HAL: AIF: In voice mode, ratio is from %d to %d,bckLrckRatio:%d ",
                     VOICE_BCK_LRCK_RATIO_MIN,
                     VOICE_BCK_LRCK_RATIO_MAX,
                     serialCfg->bckLrckRatio);

            OSI_ASSERT(serialCfg->bckLrckRatio >= VOICE_BCK_LRCK_RATIO_MIN && serialCfg->bckLrckRatio <= VOICE_BCK_LRCK_RATIO_MAX,
                       "Improper bckLrckRatio ");

            bckLrckDivider = serialCfg->bckLrckRatio - 16;
        }
        else
        {
            OSI_LOGI(0, "HAL: AIF: In audio mode, ratio is from %d to %d, bckLrckRatio:%d ",
                     AUDIO_BCK_LRCK_RATIO_MIN,
                     AUDIO_BCK_LRCK_RATIO_MAX,
                     serialCfg->bckLrckRatio);

            OSI_ASSERT(serialCfg->bckLrckRatio >= AUDIO_BCK_LRCK_RATIO_MIN && serialCfg->bckLrckRatio <= AUDIO_BCK_LRCK_RATIO_MAX,
                       "Improper bckLrckRatio ");
            bckLrckDivider = serialCfg->bckLrckRatio / 2 - 16;
        }

        serialCfgReg |= AIF_BCK_LRCK(bckLrckDivider);
        OSI_LOGI(0, "hal_newAif1Cofig: bckLrckRatio=0x%x,bckLrckDivider=0x%x\n", serialCfg->bckLrckRatio, bckLrckDivider);
        OSI_LOGI(0, "hal_newAif1Cofig: invertBck=0x%x\n", serialCfg->invertBck);
        OSI_LOGI(0, "hal_newAif1Cofig: outputHalfCycleDelay = 0x%x\n", serialCfg->outputHalfCycleDelay);
        OSI_LOGI(0, "hal_newAif1Cofig: inputHalfCycleDelay = 0x%x\n", serialCfg->inputHalfCycleDelay);

        // BCK polarity
        if (serialCfg->invertBck)
        {
            serialCfgReg |= AIF_BCK_POL_INVERT;
        }
        else
        {
            serialCfgReg |= AIF_BCK_POL_NORMAL;
        }

        // Output Half Cycle Delay
        if (serialCfg->outputHalfCycleDelay)
        {
            serialCfgReg |= AIF_OUTPUT_HALF_CYCLE_DLY_DLY;
        }
        else
        {
            serialCfgReg |= AIF_OUTPUT_HALF_CYCLE_DLY_NO_DLY;
        }

        // Input Half Cycle Delay
        if (serialCfg->inputHalfCycleDelay)
        {
            serialCfgReg |= AIF_INPUT_HALF_CYCLE_DLY_DLY;
        }
        else
        {
            serialCfgReg |= AIF_INPUT_HALF_CYCLE_DLY_NO_DLY;
        }

        // BckOut gating
        if (serialCfg->enableBckOutGating)
        {
            serialCfgReg |= AIF_BCKOUT_GATE_GATED;
        }
        else
        {
            serialCfgReg |= AIF_BCKOUT_GATE_NO_GATE;
        }
    }

    // Write register
    hwp_aif1->serial_ctrl = serialCfgReg;
    OSI_LOGI(0, "hal_newAif1Cofig: serial_ctrl = 0x%x\n", serialCfgReg);

    if ((config->channel == HAL_AIF_CHANNEL_STEREO) || (config->PARALLEL_FOR_FM))
    {
        hwp_aif1->fm_record_ctrl |= AIF_RECORD_EN;
    }
    else
    {
        hwp_aif1->fm_record_ctrl &= ~(AIF_RECORD_EN);
    }

    // hwp_aif1->ctrl = controlReg;
    g_halAifControlReg = controlReg;
    OSI_LOGI(0, "hal_newAif1Cofig: ctrl = 0x%x\n", controlReg);
    OSI_LOGI(0, "hal_newAif1Cofig success\n");

    return HAL_ERR_NO; /*clean warning*/
}

PUBLIC HAL_ERR_T hal_newAif2Cofig(CONST HAL_AIF_CONFIG_T *config)
{
    UINT32 controlReg = 0;
    UINT32 serialCfgReg = 0;
    UINT32 lrck;
    UINT32 bck;
    UINT32 clkDivider;
    UINT32 bckLrckDivider;
    UINT32 bParallel = FALSE;
    UINT32 DAC_SRC_CTL = 0;
    UINT32 ADC_SRC_CTL = 0;
    UINT32 DAC_SRC_STEP = 0;

    // Pointer to the serial config
    HAL_AIF_SERIAL_CFG_T *serialCfg;

    OSI_LOGI(0, "hal_newAif2Cofig interface:%d\n", config->interface);
    OSI_LOGI(0, "hal_newAif2Cofig sampleRate:%d\n", config->sampleRate);
    OSI_LOGI(0, "hal_newAif2Cofig channelNb:%d\n", config->channelNb);

    //step 1 Clr aif  and aud top
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->reg_dbg = SYS_CTRL_PROTECT_UNLOCK;
    hal_AifSetAifApbClk(TRUE);

    hwp_sysCtrl->aif_apb_rst_clr = SYS_CTRL_CLR_AIF_APB_RST(SYS_CTRL_CLR_AIF2_APB_RST_ID_AIF);

    if ((config->interface == HAL_AIF_IF_PARALLEL) || (config->interface == HAL_AIF_IF_PARALLEL_STEREO) ||
        (config->interface == HAL_AIF_IF_SERIAL_IN_PARALLEL_OUT) || (config->interface == HAL_AIF_IF_SERIAL_1_IN_PARALLEL_OUT) ||
        (config->interface == HAL_AIF_IF_SERIAL_2_IN_PARALLEL_OUT))
    {
        hwp_sysCtrl->aif_apb_rst_clr = SYS_CTRL_CLR_AIF_APB_RST(SYS_CTRL_CLR_AIF_APB_RST_ID_AUD_2AD);
        bParallel = TRUE;
    }

    //maybe need seting some clk to auto gate
    //hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);

    //step 2, seting dai and aif interface
    //FM and AUD_TOP mux selecte

    if ((config->interface == HAL_AIF_IF_PARALLEL) || (config->interface == HAL_AIF_IF_PARALLEL_STEREO))
    {
        ADC_SRC_CTL = AUD_2AD_ADC_SRC_N(config->sampleRate / 1000 / 4) | AUD_2AD_ADC1_SRC_N(config->sampleRate / 1000 / 4);
    }

    hwp_aud2ad->adc_src_ctl = ADC_SRC_CTL;

    if (bParallel)
    {
        switch (config->sampleRate)
        {
        // AIF_SOURCE_CLOCK = 24576000
        case HAL_AIF_FREQ_8000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(10);
            DAC_SRC_STEP = 2 * 160;
            break;
        case HAL_AIF_FREQ_9600HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(9);
            DAC_SRC_STEP = 24 * 16;
            break;
        case HAL_AIF_FREQ_11025HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(8);
            DAC_SRC_STEP = 3 * 147;
            break;
        case HAL_AIF_FREQ_12000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(7);
            DAC_SRC_STEP = 3 * 160;
            break;
        case HAL_AIF_FREQ_16000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(6);
            DAC_SRC_STEP = 4 * 160;
            break;
        case HAL_AIF_FREQ_22050HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(5);
            DAC_SRC_STEP = 6 * 147;
            break;
        case HAL_AIF_FREQ_24000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(4);
            DAC_SRC_STEP = 6 * 160;
            break;
        case HAL_AIF_FREQ_32000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(3);
            DAC_SRC_STEP = 8 * 160;
            break;
        case HAL_AIF_FREQ_44100HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(2);
            DAC_SRC_STEP = 12 * 147;
            break;
        case HAL_AIF_FREQ_48000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(1);
            DAC_SRC_STEP = 12 * 160;
            break;
        case HAL_AIF_FREQ_96000HZ:
            DAC_SRC_CTL = AUD_2AD_DAC_FS_MODE(0);
            DAC_SRC_STEP = 24 * 160;
            break;
        default:
            //HAL_ASSERT(FALSE, "Invalid sample rate: %d", config->sampleRate);
            break;
        }

        hwp_aud2ad->dac_src_ctl &= ~(AUD_2AD_DAC_FS_MODE(0xf));
        hwp_aud2ad->dac_src_ctl |= DAC_SRC_CTL;

        hwp_aud2ad->dac_src_step = DAC_SRC_STEP;

        hwp_aud2ad->aud_top_ctl &= ~(AUD_2AD_ADC_IIS_SEL(3));
        hwp_aud2ad->aud_top_ctl &= ~(AUD_2AD_DAC_IIS_SEL(3));
        hwp_aud2ad->aud_top_ctl &= ~(AUD_2AD_ADC_SINC_IN_SEL(3));

        hwp_aud2ad->aud_top_ctl |= AUD_2AD_ADC_EN_R | AUD_2AD_ADC_EN_L | AUD_2AD_DAC_EN_R | AUD_2AD_DAC_EN_L | AUD_2AD_ADC1_EN_L | AUD_2AD_ADC1_EN_R | AUD_2AD_ADC_SINC_IN_SEL(0);
        if (config->PARALLEL_FOR_FM)
        {
            hwp_aud2ad->aud_top_ctl |= AUD_2AD_ADC_IIS_SEL(1) | AUD_2AD_DAC_IIS_SEL(2); //AIF2 use FM
        }
        else
        {
            hwp_aud2ad->aud_top_ctl |= AUD_2AD_ADC_IIS_SEL(2) | AUD_2AD_DAC_IIS_SEL(1); //aif2
        }

        hwp_aud2ad->aud_clr = AUD_2AD_ADC_CLR | AUD_2AD_DAC_CLR | AUD_2AD_ADC1_CLR;
        //hal_TimDelay(1);
        osiDelayUS(1);
        hwp_aud2ad->aud_clr = 0;
        hwp_aud2ad->audif_fifo_ctl = AUD_2AD_ADC_FIFO_AF_LVL(3);

        if (config->channel == HAL_AIF_CHANNEL_MONO_R)
            hwp_aud2ad->aud_iis_ctl |= AUD_2AD_ADC_LR_SEL;
        else
            hwp_aud2ad->aud_iis_ctl &= ~(AUD_2AD_ADC_LR_SEL);
    }

    //Step3 AIF Input mux select
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_AIF1_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_AIF2_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S1_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S2_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S3_SEL(7));
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S1_BCK_LRCK_OEN);
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S2_BCK_LRCK_OEN);
    hwp_sysCtrl->cfg_aif_cfg &= ~(SYS_CTRL_I2S3_BCK_LRCK_OEN);
    if ((config->interface == HAL_AIF_IF_SERIAL) || (config->interface == HAL_AIF_IF_SERIAL_IN_PARALLEL_OUT))
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_AIF1_SEL(7) | SYS_CTRL_AIF2_SEL(2);
    }
    else if ((config->interface == HAL_AIF_IF_SERIAL_1) || (config->interface == HAL_AIF_IF_SERIAL_1_IN_PARALLEL_OUT))
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_AIF1_SEL(7) | SYS_CTRL_AIF2_SEL(3);
    }
    else if ((config->interface == HAL_AIF_IF_SERIAL_2) || (config->interface == HAL_AIF_IF_SERIAL_2_IN_PARALLEL_OUT))
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_AIF1_SEL(7) | SYS_CTRL_AIF2_SEL(4);
    }
    else
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_AIF1_SEL(7) | SYS_CTRL_AIF2_SEL(7);
    }

    //Step 4 AIF Output mux select
    hwp_sysCtrl->cfg_misc_cfg &= ~(SYS_CTRL_AIF2_I2S_BCK_SEL(7));
    if (config->interface == HAL_AIF_IF_SERIAL)
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_SEL(1); //aif 2
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_SEL(7); //aif 2
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_SEL(7); //aif 2
        if (config->serialCfg->aifIsMaster)
        {
            hwp_sysCtrl->cfg_aif_cfg &= (~SYS_CTRL_I2S1_BCK_LRCK_OEN); //i2s1 bck,lrck output enable
            //bit2 is 0, use clk 26M audio, bit2 is 1, use 133M pll(for debug)
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF2_I2S_BCK_SEL(3);
        }
        else //slave
        {
            hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_BCK_LRCK_OEN; //  i2s1 bck,lrck output disable
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF2_I2S_BCK_SEL(0);
        }
    }
    else if (config->interface == HAL_AIF_IF_SERIAL_1)
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_SEL(7); //aif 2
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_SEL(1); //aif 2
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_SEL(7); //aif 2
        if (config->serialCfg->aifIsMaster)
        {
            hwp_sysCtrl->cfg_aif_cfg &= (~SYS_CTRL_I2S2_BCK_LRCK_OEN); //i2s2 bck,lrck output enable
            //bit2 is 0, use clk 26M audio, bit2 is 1, use 133M pll(for debug)
            g_Cfg_MISC_CFG = SYS_CTRL_AIF2_I2S_BCK_SEL(3);
        }
        else //slave
        {
            hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_BCK_LRCK_OEN; //  i2s2 bck,lrck output disable
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF2_I2S_BCK_SEL(1);
        }
    }
    else if (config->interface == HAL_AIF_IF_SERIAL_2)
    {
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_SEL(7); //aif 2
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_SEL(7); //aif 2
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_SEL(1); //aif 2
        if (config->serialCfg->aifIsMaster)
        {
            hwp_sysCtrl->cfg_aif_cfg &= (~SYS_CTRL_I2S3_BCK_LRCK_OEN); //i2s3 bck,lrck output enable
            //bit2 is 0, use clk 26M audio, bit2 is 1, use 133M pll(for debug)
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF2_I2S_BCK_SEL(3);
        }
        else
        {
            hal_AifSetAifOtherClk(TRUE);
            hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_BCK_LRCK_OEN;    //  i2s3 bck,lrck output disable
            hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF2_I2S_BCK_SEL(2); //from huapeng
        }
    }
    else
    {

        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S1_SEL(7); //aif 2
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S2_SEL(7); //aif 2
        hwp_sysCtrl->cfg_aif_cfg |= SYS_CTRL_I2S3_SEL(7); //aif 2

        //bit2 is 0, use clk 26M audio, bit2 is 1, use 133M pll(for debug)
        hwp_sysCtrl->cfg_misc_cfg |= SYS_CTRL_AIF2_I2S_BCK_SEL(3);
    }

    if (config->interface == HAL_AIF_IF_PARALLEL)
    {
        // Analog mode
        controlReg |= AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA | AIF_LOOP_BACK_NORMAL;
        serialCfgReg |= AIF_MASTER_MODE_MASTER; // Well, it's analog mode ...
#ifdef SUPPORT_DUAL_MIC
        controlReg |= AIF_PARALLEL_IN2_EN_ENABLE;
#endif
    }
    else if (config->interface == HAL_AIF_IF_PARALLEL_STEREO)
    {
#if (CHIP_HAS_STEREO_DAC == 1)
        controlReg |= AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA | AIF_LOOP_BACK_NORMAL | AIF_TX_STB_MODE;
        // Parallel stereo out expects stereo data
        if (config->channelNb == HAL_AIF_MONO)
        {
            serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_MONO_STEREO_DUPLI;
        }
        else
        {
            serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_STEREO_STEREO;
        }
#ifdef SUPPORT_DUAL_MIC
        controlReg |= AIF_PARALLEL_IN2_EN_ENABLE;
#endif

#else
        OSI_ASSERT(FALSE, "This chip has not the Parallel Stereo Interface.");
#endif
    }
    else
    {
        // Digital (serial) modes
        OSI_ASSERT(config->serialCfg != NULL, "hal_AifOpen in serial mode without serial config\n");

        if (config->interface == HAL_AIF_IF_SERIAL ||
            config->interface == HAL_AIF_IF_SERIAL_1 ||
            config->interface == HAL_AIF_IF_SERIAL_2)
        {
            // Serial in and serial out
            controlReg |= AIF_PARALLEL_OUT_CLR_PARA |
                          AIF_PARALLEL_IN_CLR_PARA |
                          AIF_LOOP_BACK_NORMAL;
        }
        else
        {
            // Serial in and parallel out
            controlReg |= AIF_PARALLEL_OUT_SET_PARA |
                          AIF_PARALLEL_IN_CLR_PARA |
                          AIF_LOOP_BACK_NORMAL |
                          AIF_TX_STB_MODE;
            // Parallel stereo out expects stereo data
            if (config->channelNb == HAL_AIF_MONO)
            {
                serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_MONO_STEREO_DUPLI;
            }
            else
            {
                serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_STEREO_STEREO;
            }
        }

        // Mode
        serialCfg = config->serialCfg;
        OSI_ASSERT((serialCfg->mode < HAL_SERIAL_MODE_QTY),
                   "Improper serial mode");
        serialCfgReg |= AIF_SERIAL_MODE(serialCfg->mode);

        // TODO FIXME Implement the clock configuration
        // here from the sample rate given at the aif opening

        // FIXME
        //select i2s0 ,1,2 interface, change to  Cfg_AIF_CFG in AUD TOP

        // Master
        if (serialCfg->aifIsMaster)
        {
            serialCfgReg |= AIF_MASTER_MODE_MASTER;
        }
        else
        {
            serialCfgReg |= AIF_MASTER_MODE_SLAVE;
        }

        // LSB first
        if (serialCfg->lsbFirst)
        {
            serialCfgReg |= AIF_LSB_LSB;
        }
        else
        {
            serialCfgReg |= AIF_LSB_MSB;
        }

        // LRCK polarity
        if (serialCfg->polarity)
        {
            serialCfgReg |= AIF_LRCK_POL_LEFT_L_RIGHT_H;
        }
        else
        {
            serialCfgReg |= AIF_LRCK_POL_LEFT_H_RIGHT_L;
        }

        // Rx delay
        OSI_ASSERT(serialCfg->rxDelay < HAL_AIF_RX_DELAY_QTY,
                   "Improper delay for serial Rx");
        serialCfgReg |= AIF_RX_DLY(serialCfg->rxDelay);

        // Tx delay
        if (serialCfg->txDelay == HAL_AIF_TX_DELAY_ALIGN)
        {
            serialCfgReg |= AIF_TX_DLY_ALIGN;
        }
        else
        {
            serialCfgReg |= AIF_TX_DLY_DLY_1;
        }

        // Rx Mode
        if (serialCfg->rxMode == HAL_AIF_RX_MODE_STEREO_STEREO)
        {
            serialCfgReg |= AIF_RX_MODE_STEREO_STEREO;
        }
        else
        {
            serialCfgReg |= AIF_RX_MODE_STEREO_MONO_FROM_L;
        }

        // Tx Mode
        OSI_ASSERT(serialCfg->txMode < HAL_AIF_TX_MODE_QTY,
                   "Improper mode for serial Tx");
        serialCfgReg |= AIF_TX_MODE(serialCfg->txMode);

        // LRCK frequency ...
        lrck = serialCfg->fs;
        bck = lrck * serialCfg->bckLrckRatio;

        if (serialCfg->aifIsMaster)
        {
            // note , the clk source is 26m, not is 24576000 or 2257920,   so the bck divider is vary rough, maby use   othe clk for bck out
            // We get the BCK from the the audio clock.
            clkDivider = (26000000 / bck);
            hwp_sysCtrl->cfg_clk_audiobck1_div = SYS_CTRL_AUDIOBCK1_DIVIDER(clkDivider);
            hwp_sysCtrl->cfg_clk_audiobck1_div = SYS_CTRL_AUDIOBCK1_DIVIDER(clkDivider) | SYS_CTRL_AUDIOBCK1_UPDATE;
        }

        // The ratio is calculated differently depending
        // on whether we're using voice or audio mode.
        if (serialCfg->mode == HAL_SERIAL_MODE_VOICE)
        {
            // Check the ratio
            OSI_LOGI(0, "HAL: AIF: In voice mode, ratio is from %d to %d,bckLrckRatio:%d ",
                     VOICE_BCK_LRCK_RATIO_MIN,
                     VOICE_BCK_LRCK_RATIO_MAX,
                     serialCfg->bckLrckRatio);

            OSI_ASSERT(serialCfg->bckLrckRatio >= VOICE_BCK_LRCK_RATIO_MIN && serialCfg->bckLrckRatio <= VOICE_BCK_LRCK_RATIO_MAX,
                       "Improper bckLrckRatio ");

            bckLrckDivider = serialCfg->bckLrckRatio - 16;
        }
        else
        {
            OSI_LOGI(0, "HAL: AIF: In voice mode, ratio is from %d to %d, bckLrckRatio:%d ",
                     VOICE_BCK_LRCK_RATIO_MIN,
                     VOICE_BCK_LRCK_RATIO_MAX,
                     serialCfg->bckLrckRatio);

            OSI_ASSERT(serialCfg->bckLrckRatio >= AUDIO_BCK_LRCK_RATIO_MIN && serialCfg->bckLrckRatio <= AUDIO_BCK_LRCK_RATIO_MAX,
                       "Improper bckLrckRatio ");
            bckLrckDivider = serialCfg->bckLrckRatio / 2 - 16;
        }

        serialCfgReg |= AIF_BCK_LRCK(bckLrckDivider);

        OSI_LOGI(0, "hal_newAif2Cofig: invertBck=%x\n", serialCfg->invertBck);
        OSI_LOGI(0, "hal_newAif2Cofig: outputHalfCycleDelay = %x\n", serialCfg->outputHalfCycleDelay);
        OSI_LOGI(0, "hal_newAif2Cofig: inputHalfCycleDelay = %x\n", serialCfg->inputHalfCycleDelay);

        // BCK polarity
        if (serialCfg->invertBck)
        {
            serialCfgReg |= AIF_BCK_POL_INVERT;
        }
        else
        {
            serialCfgReg |= AIF_BCK_POL_NORMAL;
        }

        // Output Half Cycle Delay
        if (serialCfg->outputHalfCycleDelay)
        {
            serialCfgReg |= AIF_OUTPUT_HALF_CYCLE_DLY_DLY;
        }
        else
        {
            serialCfgReg |= AIF_OUTPUT_HALF_CYCLE_DLY_NO_DLY;
        }

        // Input Half Cycle Delay
        if (serialCfg->inputHalfCycleDelay)
        {
            serialCfgReg |= AIF_INPUT_HALF_CYCLE_DLY_DLY;
        }
        else
        {
            serialCfgReg |= AIF_INPUT_HALF_CYCLE_DLY_NO_DLY;
        }

        // BckOut gating
        if (serialCfg->enableBckOutGating)
        {
            serialCfgReg |= AIF_BCKOUT_GATE_GATED;
        }
        else
        {
            serialCfgReg |= AIF_BCKOUT_GATE_NO_GATE;
        }
    }

    // Write register
    hwp_aif2->serial_ctrl = serialCfgReg;

    if ((config->channel == HAL_AIF_CHANNEL_STEREO) || (config->PARALLEL_FOR_FM))
    {
        hwp_aif2->fm_record_ctrl |= AIF_RECORD_EN;
    }
    else
    {
        hwp_aif2->fm_record_ctrl &= ~(AIF_RECORD_EN);
    }
    // hwp_aif2->ctrl = controlReg;
    g_halAifControlReg = controlReg;

    OSI_LOGI(0, ":hal_newAif2Cofig success\n");

    return HAL_ERR_NO; /*clean warning*/
}

// =============================================================================
// hal_AifOpen
// -----------------------------------------------------------------------------
/// Open the AIF and set the appropriate config.
/// When the analog interface is used, leave the \c serialCfg field of the
/// \c config parameter at 0.
///
/// @param config Pointer to the configuration of the Audio InterFace.
/// @return HAL_ERR_NO if everything is alright or HAL_ERR_RESOURCE_BUSY if
/// the AIF has already been opened.
// =============================================================================
PUBLIC HAL_ERR_T hal_AifOpen(CONST HAL_AIF_CONFIG_T *config)
{
    OSI_LOGI(0, "hal_AifOpen g_halAifOpened=%x\n", g_halAifOpened);

    // Already opened ?
    if (g_halAifOpened)
    {
        OSI_LOGI(0, "hal_AifOpen aif opened already\n");

        return HAL_ERR_RESOURCE_BUSY;
    }

    OSI_LOGI(0, "hal_AifOpen g_halAifActiveDai=%x\n", g_halAifActiveDai[0]);
    OSI_LOGI(0, "hal_AifOpen g_halAifActiveDai=%x\n", g_halAifActiveDai[1]);

    if (hal_AifIsDaiBusy(config->interface))
    {
        OSI_LOGI(0, "hal_AifOpen hal_AifIsDaiBusy\n");
        return HAL_ERR_RESOURCE_BUSY;
    }
    g_AUD_TOP_CTL = 0x88990000;

    OSI_LOGI(0, "hal_AifOpen\n");
    if (!g_halUseAif2Flag)
        hal_newAif1Cofig(config);
    else
        hal_newAif2Cofig(config);

    // Initializes global variables
    g_halAifHandlers[RECORD].halfHandler = NULL;
    g_halAifHandlers[RECORD].endHandler = NULL;
    g_halAifHandlers[PLAY].halfHandler = NULL;
    g_halAifHandlers[PLAY].endHandler = NULL;
    g_halAifHandlers[AIF2RECORD].halfHandler = NULL;
    g_halAifHandlers[AIF2RECORD].endHandler = NULL;
    g_halAifHandlers[AIF2PLAY].halfHandler = NULL;
    g_halAifHandlers[AIF2PLAY].endHandler = NULL;

    g_halAifPlaying = FALSE;
    g_halAifRecording = FALSE;
    g_halAifActiveDai[HAL_AIF_ID_AIF1] = config->interface;
    g_halAifActiveDai[HAL_AIF_ID_AIF2] = config->interface;

    g_halAifOpened = TRUE;

    return HAL_ERR_NO;
}

// =============================================================================
// hal_AifLoopBack
// -----------------------------------------------------------------------------
/// Set AIF loopback mode
// =============================================================================
PUBLIC VOID hal_AifLoopBack(BOOL loop)
{
    if (loop == TRUE)
    {
        g_halAifControlReg |= AIF_LOOP_BACK_LOOPBACK;
        hwp_aif1->ctrl =
            (hwp_aif1->ctrl & (~(AIF_OUT_UNDERFLOW | AIF_IN_OVERFLOW))) | AIF_LOOP_BACK_LOOPBACK;
    }
    else
    {
        g_halAifControlReg &= ~(AIF_LOOP_BACK_LOOPBACK);
        hwp_aif1->ctrl = hwp_aif1->ctrl &
                         ~(AIF_LOOP_BACK_LOOPBACK | AIF_OUT_UNDERFLOW | AIF_IN_OVERFLOW);
    }
}

// =============================================================================
// hal_AifClose
// -----------------------------------------------------------------------------
/// Close the AIF.
// =============================================================================
PUBLIC VOID hal_AifClose(VOID)
{
    OSI_LOGI(0, "aud_:hal_AifClose start\n");
    hwp_aif1->ctrl = 0;
    hwp_aif1->serial_ctrl = AIF_MASTER_MODE_MASTER;
    hwp_aif1->side_tone = 0;

    hwp_aif2->ctrl = 0;
    hwp_aif2->serial_ctrl = AIF_MASTER_MODE_MASTER;
    hwp_aif2->side_tone = 0;

#ifndef WITHOUT_AIF_TX_STB
    hwp_sysCtrl->cfg_aif_tx_stb_div = 0;
#endif

    // Reset AIF module, to force output lines (I2S clocks and data lines)
    // to be low.
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->reg_dbg = SYS_CTRL_PROTECT_UNLOCK;

    hwp_sysCtrl->aif_apb_rst_set = SYS_CTRL_CLR_AIF_APB_RST(SYS_CTRL_CLR_AIF1_APB_RST_ID_AIF);
    hwp_sysCtrl->aif_apb_rst_set = SYS_CTRL_CLR_AIF_APB_RST(SYS_CTRL_CLR_AIF2_APB_RST_ID_AIF);
    hwp_sysCtrl->aif_apb_rst_set = SYS_CTRL_CLR_AIF_APB_RST(SYS_CTRL_CLR_AIF_APB_RST_ID_AUD_2AD);

    hal_SysExitCriticalSection(status);

    g_halAifOpened = FALSE;

    g_halAifPlaying = FALSE;
    g_halAifRecording = FALSE;
    g_halAifControlReg = 0;
    g_halAifActiveDai[HAL_AIF_ID_AIF1] = HAL_AIF_IF_QTY;
    g_halAifActiveDai[HAL_AIF_ID_AIF2] = HAL_AIF_IF_QTY;
    hwp_aud2ad->aud_top_ctl = g_AUD_TOP_CTL;

    OSI_LOGI(0, "aud_:hal_AifClose success\n");
}

// =============================================================================
// hal_AifStream
// -----------------------------------------------------------------------------
/// Play or record a stream from or to a buffer
/// The buffer start address must be aligned on a 32-bit address, and the size
/// must be a multiple of 32 bytes.
/// @param xfer Describes the buffer and size
/// @param direction Define the direction: PLAY for play, RECORD to record.
// =============================================================================
PRIVATE HAL_ERR_T hal_AifStream(CONST HAL_AIF_STREAM_T *xfer, UINT32 direction)
{
    UINT32 irqMask = 0;
    INT16 times_delay = 0;

    while ((hwp_Ifc->ch[direction].status & AUDIO_IFC_ENABLE))
    {
        //sxr_Sleep(1 MS_WAITING);  //need change
        osiDelayUS(1000);
        times_delay++;
        if (times_delay > 200)
        {
            OSI_LOGI(0, "aud_:hal_AifStream HAL_ERR_RESOURCE_BUSY \n");
            OSI_ASSERT(0, ("aud_:hal_AifStream wait ifc ch status TimeOut!")); /*assert to do*/
            return HAL_ERR_RESOURCE_BUSY;
        }
    }

    // Assert on word alignement
    OSI_ASSERT(((UINT32)xfer->startAddress) % 4 == 0, "AUDIO IFC transfer start address not aligned");

    // Size must be a multiple of 32 bytes
    OSI_ASSERT((xfer->length) % 32 == 0, "AUDIO IFC transfer size not mult. of 32-bits");

    hwp_Ifc->ch[direction].start_addr = (UINT32)xfer->startAddress;
    hwp_Ifc->ch[direction].fifo_size = xfer->length;

    if (xfer->halfHandler != NULL)
    {
        g_halAifHandlers[direction].halfHandler = xfer->halfHandler;
        irqMask |= AUDIO_IFC_HALF_FIFO;
    }
    else
    {
        g_halAifHandlers[direction].halfHandler = NULL;
    }

    if (xfer->endHandler != NULL)
    {
        g_halAifHandlers[direction].endHandler = xfer->endHandler;
        irqMask |= AUDIO_IFC_END_FIFO;
    }
    else
    {
        g_halAifHandlers[direction].endHandler = NULL;
    }

    irqMask = AUDIO_IFC_HALF_FIFO | AUDIO_IFC_END_FIFO; //hxy
    hwp_Ifc->ch[direction].int_mask = irqMask;          //hxy

    if (xfer->playSyncWithRecord)
    {
        if ((direction == PLAY) || (direction == AIF2PLAY))
        {
            // Not to start the play stream
            return HAL_ERR_NO;
        }
        else if ((direction == RECORD) || (direction == AIF2RECORD))
        {
            // Start the play stream just before starting the record one
            if (!g_halUseAif2Flag)
                hwp_Ifc->ch[PLAY].control = AUDIO_IFC_ENABLE;
            else
                hwp_Ifc->ch[AIF2PLAY].control = AUDIO_IFC_ENABLE;
            //sxr_Sleep(13 MS_WAITING);
        }
    }

    hwp_Ifc->ch[direction].control = AUDIO_IFC_ENABLE;

    return HAL_ERR_NO;
}

// =============================================================================
// hal_AifPlayStream
// -----------------------------------------------------------------------------
/// Play a stream, copied from a buffer in memory to the AIF fifos, in infinite
/// mode: when the end of the buffer is reached, playing continues from the
/// beginning.
/// The buffer start address must be aligned on a 32-bit address, and the size
/// must be a multiple of 32 bytes.
///
/// @param playedStream Pointer to the played stream. A stream pointing to
/// a NULL buffer (startAddress field) only enable the audio, without
/// playing data from anywhere.
/// @return HAL_ERR_NO if everything is alright or HAL_ERR_RESOURCE_BUSY if
/// a play is already in process.
// =============================================================================
PUBLIC HAL_ERR_T hal_AifPlayStream(CONST HAL_AIF_STREAM_T *playedStream)
{
    UINT32 status;
    HAL_ERR_T errStatus = HAL_ERR_NO;

    OSI_LOGI(0, "aud_:hal_AifPlayStream start\n");
    status = hal_SysEnterCriticalSection();
    if (playedStream->startAddress != NULL)
    {
        if (!g_halUseAif2Flag)
            errStatus = hal_AifStream((HAL_AIF_STREAM_T *)playedStream, PLAY);
        else
            errStatus = hal_AifStream((HAL_AIF_STREAM_T *)playedStream, AIF2PLAY);
    }

    if (errStatus == HAL_ERR_NO)
    {
        g_halAifPlaying = TRUE;
        // Allow symbols to be sent
        if (!g_halUseAif2Flag)
        {
            hwp_aif1->ctrl = (g_halAifControlReg | AIF_ENABLE_H_ENABLE) & (~AIF_TX_OFF_TX_OFF); //hxy
        }
        else
        {
            hwp_aif2->ctrl = (g_halAifControlReg | AIF_ENABLE_H_ENABLE) & (~AIF_TX_OFF_TX_OFF); //hxy
        }
    }
    hal_SysExitCriticalSection(status);

    OSI_LOGI(0, "aud_:hal_AifPlayStream success\n");

    return errStatus;
}

// =============================================================================
// hal_AifRecordStream
// -----------------------------------------------------------------------------
/// Record a stream, copied from the AIF fifos to a buffer in memory, in infinite
/// mode: when the end of the buffer is reached, playing continues from the
/// beginning.
/// The buffer start address must be aligned on a 32-bit address, and the size
/// must be a multiple of 32 bytes.
///
/// @param recordedStream Pointer to the recorded stream. A stream pointing to
/// a NULL buffer (startAddress field) only enable
/// the audio, without recording data from anywhere.
///
// =============================================================================
PUBLIC HAL_ERR_T hal_AifRecordStream(CONST HAL_AIF_STREAM_T *recordedStream)
{
    UINT32 status;
    HAL_ERR_T errStatus = HAL_ERR_NO;

    OSI_LOGI(0, "aud_:hal_AifRecordStream start\n");

    status = hal_SysEnterCriticalSection();
    if (recordedStream->startAddress != NULL)
    {
        if (!g_halUseAif2Flag)
            errStatus = hal_AifStream(recordedStream, RECORD);
        else
            errStatus = hal_AifStream(recordedStream, AIF2RECORD);
    }

    if (errStatus == HAL_ERR_NO)
    {
        g_halAifRecording = TRUE;

        // In Loopback mode, do not start now - the Tx will do it
        // This is usefull for synchronization purpose
        if ((!g_halAifPlaying) && ((g_halAifControlReg & AIF_LOOP_BACK_LOOPBACK) == 0))
        {
            // Need to start the clock: write 4 data not to send
            OSI_LOGI(0, "aud_:hal_AifRecordStream g_halAifControlReg=0x%x\n", g_halAifControlReg);
            if (!g_halUseAif2Flag)
            {
                hwp_aif1->ctrl = g_halAifControlReg | AIF_ENABLE_H_ENABLE | AIF_TX_OFF_TX_OFF;
                hwp_aif1->data = 0;
                hwp_aif1->data = 0;
                hwp_aif1->data = 0;
                hwp_aif1->data = 0;
            }
            else
            {
                hwp_aif2->ctrl = g_halAifControlReg | AIF_ENABLE_H_ENABLE | AIF_TX_OFF_TX_OFF;
                hwp_aif2->data = 0;
                hwp_aif2->data = 0;
                hwp_aif2->data = 0;
                hwp_aif2->data = 0;
            }
        }
    }
    hal_SysExitCriticalSection(status);

    OSI_LOGI(0, "aud_:hal_AifRecordStream success\n");
    return errStatus;
}

// =============================================================================
// hal_AifStopPlay
// -----------------------------------------------------------------------------
/// Stop playing a buffer
// =============================================================================
PUBLIC HAL_ERR_T hal_AifStopPlay(VOID)
{
    UINT32 times_delay = 0;
    HAL_ERR_T ret = HAL_ERR_NO;

    OSI_LOGI(0, "aud_:hal_AifStopPlay start\n");
    g_halAifPlaying = FALSE;

    if (!g_halUseAif2Flag)
    {
        OSI_LOGI(0, "aud_:hal_AifStopPlay AUDIO_IFC_DISABLE\n");
        hwp_Ifc->ch[PLAY].control = AUDIO_IFC_DISABLE;
        hwp_Ifc->ch[PLAY].int_mask = 0;
        //sxr_Sleep(1 MS_WAITING);
        osiDelayUS(1000);

        while ((hwp_Ifc->ch[PLAY].status & AUDIO_IFC_ENABLE))
        {
            //sxr_Sleep(1 MS_WAITING);
            osiDelayUS(1000);
            hwp_Ifc->ch[PLAY].control = AUDIO_IFC_DISABLE;
            hwp_Ifc->ch[PLAY].int_mask = 0;

            times_delay++;
            if (times_delay > 10)
            {
                OSI_LOGI(0, "aud_:hal_Aif1StopPlay FAIL\n");
                //HAL_ASSERT(0, ("aud_:hal_AifStopPlay wait ifc ch status TimeOut!"));/*assert to do*/
                ret = HAL_ERR_RESOURCE_BUSY;
            }
        }
    }
    else
    {
        hwp_Ifc->ch[AIF2PLAY].control = AUDIO_IFC_DISABLE;
        hwp_Ifc->ch[AIF2PLAY].int_mask = 0;
        // sxr_Sleep(1 MS_WAITING);
        osiDelayUS(1000);

        times_delay = 0;
        while ((hwp_Ifc->ch[AIF2PLAY].status & AUDIO_IFC_ENABLE))
        {
            //sxr_Sleep(1 MS_WAITING);
            osiDelayUS(1000);
            hwp_Ifc->ch[AIF2PLAY].control = AUDIO_IFC_DISABLE;
            hwp_Ifc->ch[AIF2PLAY].int_mask = 0;

            times_delay++;
            if (times_delay > 10)
            {
                OSI_LOGI(0, "aud_:hal_Aif2StopPlay FAIL\n");
                //HAL_ASSERT(0, ("aud_:hal_AifStopPlay wait ifc ch status TimeOut!"));/*assert to do*/
                ret = HAL_ERR_RESOURCE_BUSY;
            }
        }
    }

    if (!g_halUseAif2Flag)
    {
        g_halAifHandlers[PLAY].halfHandler = NULL;
        g_halAifHandlers[PLAY].endHandler = NULL;
    }
    else
    {
        g_halAifHandlers[AIF2PLAY].halfHandler = NULL;
        g_halAifHandlers[AIF2PLAY].endHandler = NULL;
    }

    // Disable the AIF if not recording
    UINT32 status = hal_SysEnterCriticalSection();
    if (!g_halAifRecording)
    {
        // To have the clock allowing the disabling.
        if (!g_halUseAif2Flag)
            hwp_aif1->ctrl = AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA;
        else
            hwp_aif2->ctrl = AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA;
    }

    OSI_LOGI(0, "aud_:hal_AifStopPlay success\n");

    hal_SysExitCriticalSection(status);

    return ret;
}

// =============================================================================
// hal_AifStopRecord
// -----------------------------------------------------------------------------
/// Stop playing a buffer
// =============================================================================
PUBLIC HAL_ERR_T hal_AifStopRecord(VOID)
{
    UINT32 times_delay = 0;
    HAL_ERR_T ret = HAL_ERR_NO;

    OSI_LOGI(0, "aud_:hal_AifStopRecord start\n");
    g_halAifRecording = FALSE;

    if (!g_halUseAif2Flag)
    {
        OSI_LOGI(0, "aud_:hal_AifStopRecord AUDIO_IFC_DISABLE\n");
        hwp_Ifc->ch[RECORD].control = AUDIO_IFC_DISABLE;
        hwp_Ifc->ch[RECORD].int_mask = 0;
        //sxr_Sleep(1 MS_WAITING);
        osiDelayUS(1000);

        while ((hwp_Ifc->ch[RECORD].status & AUDIO_IFC_ENABLE))
        {
            //sxr_Sleep(1 MS_WAITING);
            osiDelayUS(1000);
            hwp_Ifc->ch[RECORD].control = AUDIO_IFC_DISABLE;
            hwp_Ifc->ch[RECORD].int_mask = 0;

            times_delay++;
            if (times_delay > 10)
            {
                OSI_LOGE(0, "aud_:hal_Aif1StopRECORD FAIL\n");
                OSI_ASSERT(0, ("aud_:hal_AifStopRECORD wait ifc ch status TimeOut!")); /*assert to do*/
                ret = HAL_ERR_RESOURCE_BUSY;
            }
        }
    }
    else
    {
        hwp_Ifc->ch[AIF2RECORD].control = AUDIO_IFC_DISABLE;
        hwp_Ifc->ch[AIF2RECORD].int_mask = 0;
        //sxr_Sleep(1 MS_WAITING);
        osiDelayUS(1000);

        times_delay = 0;
        while ((hwp_Ifc->ch[AIF2RECORD].status & AUDIO_IFC_ENABLE))
        {
            //sxr_Sleep(1 MS_WAITING);
            osiDelayUS(1000);
            hwp_Ifc->ch[AIF2RECORD].control = AUDIO_IFC_DISABLE;
            hwp_Ifc->ch[AIF2RECORD].int_mask = 0;

            times_delay++;
            if (times_delay > 10)
            {
                OSI_LOGE(0, "aud_:hal_Aif2StopRECORD FAIL\n");
                OSI_ASSERT(0, ("aud_:hal_AifStopRECORD wait ifc ch status TimeOut!")); /*assert to do*/
                ret = HAL_ERR_RESOURCE_BUSY;
            }
        }
    }

    if (!g_halUseAif2Flag)
    {
        g_halAifHandlers[RECORD].halfHandler = NULL;
        g_halAifHandlers[RECORD].endHandler = NULL;
    }
    else
    {
        g_halAifHandlers[AIF2RECORD].halfHandler = NULL;
        g_halAifHandlers[AIF2RECORD].endHandler = NULL;
    }

    // Disable the AIF if not recording
    UINT32 status = hal_SysEnterCriticalSection();
    if (!g_halAifPlaying)
    {
        if (!g_halUseAif2Flag)
            hwp_aif1->ctrl = AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA;
        else
            hwp_aif2->ctrl = AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA;
    }

    OSI_LOGI(0, "aud_:hal_AifStopRecord success\n");
    hal_SysExitCriticalSection(status);

    return ret;
}

// =============================================================================
// hal_AifPlayReachedHalf
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the middle and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifPlayReachedHalf(VOID)
{
    BOOL ret = FALSE;

    if (!g_halUseAif2Flag)
    {
        if (hwp_Ifc->ch[PLAY].status & AUDIO_IFC_IHF)
        {
            hwp_Ifc->ch[PLAY].int_clear = AUDIO_IFC_HALF_FIFO;
            ret = TRUE;
        }
    }
    else
    {
        if (hwp_Ifc->ch[AIF2PLAY].status & AUDIO_IFC_IHF)
        {
            hwp_Ifc->ch[AIF2PLAY].int_clear = AUDIO_IFC_HALF_FIFO;
            ret = TRUE;
        }
    }

    return ret;
}

// =============================================================================
// hal_AifPlayReachedEnd
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the end and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifPlayReachedEnd(VOID)
{
    BOOL ret = FALSE;

    if (!g_halUseAif2Flag)
    {
        if (hwp_Ifc->ch[PLAY].status & AUDIO_IFC_IEF)
        {
            hwp_Ifc->ch[PLAY].int_clear = AUDIO_IFC_END_FIFO;
            ret = TRUE;
        }
    }
    else
    {
        if (hwp_Ifc->ch[AIF2PLAY].status & AUDIO_IFC_IEF)
        {
            hwp_Ifc->ch[AIF2PLAY].int_clear = AUDIO_IFC_END_FIFO;
            ret = TRUE;
        }
    }

    return ret;
}

// =============================================================================
// hal_AifRecordReachedHalf
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the middle and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifRecordReachedHalf(VOID)
{
    BOOL ret = FALSE;

    if (!g_halUseAif2Flag)
    {
        if (hwp_Ifc->ch[RECORD].status & AUDIO_IFC_IHF)
        {
            hwp_Ifc->ch[RECORD].int_clear = AUDIO_IFC_HALF_FIFO;
            ret = TRUE;
        }
    }
    else
    {
        if (hwp_Ifc->ch[AIF2RECORD].status & AUDIO_IFC_IHF)
        {
            hwp_Ifc->ch[AIF2RECORD].int_clear = AUDIO_IFC_HALF_FIFO;
            ret = TRUE;
        }
    }

    return ret;
}

// =============================================================================
// hal_AifRecordReachedEnd
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the end and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifRecordReachedEnd(VOID)
{
    BOOL ret = FALSE;

    if (!g_halUseAif2Flag)
    {
        if (hwp_Ifc->ch[RECORD].status & AUDIO_IFC_IEF)
        {
            hwp_Ifc->ch[RECORD].int_clear = AUDIO_IFC_END_FIFO;
            ret = TRUE;
        }
    }
    else
    {
        if (hwp_Ifc->ch[AIF2RECORD].status & AUDIO_IFC_IEF)
        {
            hwp_Ifc->ch[AIF2RECORD].int_clear = AUDIO_IFC_END_FIFO;
            ret = TRUE;
        }
    }

    return ret;
}

// =============================================================================
// hal_AifIrqHandler
// -----------------------------------------------------------------------------
/// Handler called by the IRQ module when a AUDIO-IFC interrupt occurs.
///
// =============================================================================
PUBLIC VOID hal_AifIrqHandler(UINT32 direction)
{
    //OSI_LOGI(0,  "aud_:hal_AifIrqHandler:%d\n",direction);

    UINT32 status;
    //BOOL halfFifoInt = FALSE;
    //BOOL endFifoInt = FALSE;

    status = hwp_Ifc->ch[direction].status & (AUDIO_IFC_CAUSE_IHF | AUDIO_IFC_CAUSE_IEF);
    // Clear cause
    //hwp_Ifc->ch[direction].int_clear = status;

    if (status & AUDIO_IFC_CAUSE_IHF)
    {
        //halfFifoInt = TRUE;
        g_halPingPangFlag = 0;
    }

    if (status & AUDIO_IFC_CAUSE_IEF)
    {
        //endFifoInt = TRUE;
        g_halPingPangFlag = 1;
    }
}

PUBLIC VOID hal_AifIrqClrStatus(UINT32 direction)
{
    UINT32 status = hwp_Ifc->ch[direction].status & (AUDIO_IFC_CAUSE_IHF | AUDIO_IFC_CAUSE_IEF);
    // Clear cause
    hwp_Ifc->ch[direction].int_clear = status;
}

// =============================================================================
// hal_AifPause
// -----------------------------------------------------------------------------
/// Pauses the streaming through the AIF
/// @param pause If \c TRUE, pauses a running stream. If \c FALSE, resume a
/// pause stream.
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC HAL_ERR_T hal_AifPause(BOOL pause)
{
    if (pause)
    {
        if (!g_halUseAif2Flag)
            hwp_aif1->ctrl &= ~AIF_ENABLE_ENABLE;
        else
            hwp_aif2->ctrl &= ~AIF_ENABLE_ENABLE;
    }
    else
    {
        if (!g_halUseAif2Flag)
            hwp_aif1->ctrl |= AIF_ENABLE_ENABLE;
        else
            hwp_aif2->ctrl |= AIF_ENABLE_ENABLE;
    }

    return HAL_ERR_NO;
}

// =============================================================================
// hal_AifGetIfcStatusRegPtr
// -----------------------------------------------------------------------------
/// Get a pointer to the IFC Curr_AHB_Addr register.
/// Used to Read this register by VoC.
/// @return INT32* pointer to the IFC Curr_AHB_Addr register.
// =============================================================================
PUBLIC INT32 *hal_AifGetIfcStatusRegPtr(VOID)
{
    // NOTE : If this IFC register (or the IFC itself) does not exist, the function should return NULL
    if (!g_halUseAif2Flag)
        return (INT32 *)&(hwp_Ifc->ch[PLAY].cur_ahb_addr);
    else
        return (INT32 *)&(hwp_Ifc->ch[AIF2PLAY].cur_ahb_addr);
}

// =============================================================================
// hal_AifGetOverflowStatus
// -----------------------------------------------------------------------------
/// Returns Rx and Tx Overflow status bits
// =============================================================================
PUBLIC UINT32 hal_AifGetOverflowStatus(VOID)
{
    if (!g_halUseAif2Flag)
        return (hwp_aif1->ctrl & (AIF_OUT_UNDERFLOW | AIF_IN_OVERFLOW));
    else
        return (hwp_aif2->ctrl & (AIF_OUT_UNDERFLOW | AIF_IN_OVERFLOW));
}
