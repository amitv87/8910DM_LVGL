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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include "hal_config.h"
#include "atr_config.h"
#include "osi_log.h"
#include "osi_api.h"
#include "at_cfg.h"
#include "at_setting_codec.h"
#include "drv_rtc.h"
#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AT_CFGFN_GLOBAL CONFIG_FS_AP_NVM_DIR "/at_cfg.nv"
#define AT_CFGFN_PROFILE CONFIG_FS_AP_NVM_DIR "/at_profile%d.nv"
#define AT_CFGFN_AUTOSAVE CONFIG_FS_AP_NVM_DIR "/at_autosave.nv"
#define AT_CFGFN_TCPIP CONFIG_FS_AP_NVM_DIR "/at_tcpip.nv"

#ifdef CONFIG_ATR_CFG_IN_ONE_FILE
#define AT_CFGFN_COMBINED CONFIG_FS_AP_NVM_DIR "/at_combined.nv"
#define AT_GLOBAL_SIZE (64)
#define AT_PROFILE_SIZE (512)
#define AT_AUTOSAVE_SIZE (3000)
typedef struct
{
    uint16_t global_size;
    uint16_t profile_size[2];
    uint16_t autosave_size;
    char global_data[AT_GLOBAL_SIZE];
    char profile_data[2][AT_PROFILE_SIZE];
    char autosave_data[AT_AUTOSAVE_SIZE];
} atCfgCombined_t;
static atCfgCombined_t gAtCfgCombined;
#endif

atSetting_t gAtSetting;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
atSetting_t gAtSettingSave;
#endif
extern const atSetting_t gAtDefaultSetting;
extern const at_tcpip_setting_t gDefaultCIPSettings;

static void _shutdownCb(void *ctx, osiShutdownMode_t mode)
{
    if (mode == OSI_SHUTDOWN_PSM_SLEEP)
    {
        int strm_size = atCfgProfileEncode(&gAtSetting, NULL, 0);
        if (strm_size < 0)
            return;

        void *buf = malloc(strm_size);
        if (buf == NULL)
            return;

        int enc_size = atCfgProfileEncode(&gAtSetting, buf, strm_size);
        if (enc_size < 0)
            return;

        if (!osiPsmDataSave(OSI_PSMDATA_OWNER_AT, buf, enc_size))
            OSI_LOGE(0, "AT psm save failed mode/0x%x size/%d/%d", mode, strm_size, enc_size);
    }
}

#ifndef CONFIG_ATR_CFG_IN_ONE_FILE
static bool atCfgLoadGlobal(atSetting_t *p)
{
    const char *file_name = AT_CFGFN_GLOBAL;
    ssize_t file_size = vfs_sfile_size(file_name);
    if (file_size <= 0)
        return false;

    void *buf = malloc(file_size);
    if (buf == NULL)
        return false;

    bool ok = (vfs_sfile_read(file_name, buf, file_size) == file_size &&
               atCfgGlobalDecode(p, buf, file_size));
    if (!ok)
        OSI_LOGE(0, "failed to load global cfg, file size/%d", file_size);

    free(buf);
    return true;
}

static bool atCfgStoreGlobal(atSetting_t *p)
{
    const char *file_name = AT_CFGFN_GLOBAL;
    int strm_size = atCfgGlobalEncode(p, NULL, 0);
    if (strm_size < 0)
        return false;

    void *buf = malloc(strm_size);
    if (buf == NULL)
        return false;

    int enc_size = atCfgGlobalEncode(p, buf, strm_size);
    bool ok = (enc_size > 0 && vfs_sfile_write(file_name, buf, enc_size) == enc_size);
    if (!ok)
        OSI_LOGE(0, "failed to save global cfg, strm size/%d enc size/%d", strm_size, enc_size);

    free(buf);
    return true;
}

static bool atCfgLoadProfile(atSetting_t *p, int profile)
{
    char file_name[VFS_PATH_MAX];
    sprintf(file_name, AT_CFGFN_PROFILE, profile);

    ssize_t file_size = vfs_sfile_size(file_name);
    if (file_size <= 0)
        return false;

    void *buf = malloc(file_size);
    if (buf == NULL)
        return false;

    bool ok = (vfs_sfile_read(file_name, buf, file_size) == file_size &&
               atCfgProfileDecode(p, buf, file_size));
    if (!ok)
        OSI_LOGE(0, "failed to load profile %d cfg, file size/%d", profile, file_size);

    free(buf);
    return true;
}

static bool atCfgStoreProfile(atSetting_t *p, int profile)
{
    char file_name[VFS_PATH_MAX];
    sprintf(file_name, AT_CFGFN_PROFILE, profile);

    int strm_size = atCfgProfileEncode(p, NULL, 0);
    if (strm_size < 0)
        return false;

    void *buf = malloc(strm_size);
    if (buf == NULL)
        return false;

    int enc_size = atCfgProfileEncode(p, buf, strm_size);

    bool ok = (enc_size > 0 && vfs_sfile_write(file_name, buf, enc_size) == enc_size);
    if (!ok)
    {
        OSI_LOGE(0, "failed to save profile %d cfg, strm size/%d enc size/%d",
                 profile, strm_size, enc_size);
    }

    free(buf);
    return ok;
}

static bool atCfgLoadAutoSave(atSetting_t *p)
{
    const char *file_name = AT_CFGFN_AUTOSAVE;
    ssize_t file_size = vfs_sfile_size(file_name);
    if (file_size <= 0)
        return false;

    void *buf = malloc(file_size);
    if (buf == NULL)
        return false;

    bool ok = (vfs_sfile_read(file_name, buf, file_size) == file_size &&
               atCfgAutoSaveDecode(p, buf, file_size));
    if (!ok)
        OSI_LOGE(0, "failed to load autosave cfg, file size/%d", file_size);

    free(buf);
    return true;
}

static bool atCfgStoreAutoSave(atSetting_t *p)
{
    const char *file_name = AT_CFGFN_AUTOSAVE;
    int strm_size = atCfgAutoSaveEncode(p, NULL, 0);
    if (strm_size < 0)
        return false;

    void *buf = malloc(strm_size);
    if (buf == NULL)
        return false;

    int enc_size = atCfgAutoSaveEncode(p, buf, strm_size);
    bool ok = (enc_size > 0 && vfs_sfile_write(file_name, buf, enc_size) == enc_size);
    if (!ok)
        OSI_LOGE(0, "failed to save autosave cfg, strm size/%d enc size/%d", strm_size, enc_size);

    free(buf);
    return true;
}
#endif

void atCfgInit(void)
{
    vfs_sfile_init(AT_CFGFN_GLOBAL);
    vfs_sfile_init(AT_CFGFN_AUTOSAVE);
    for (int n = 0; n < CONFIG_ATR_PROFILE_COUNT; n++)
    {
        char file_name[VFS_PATH_MAX];
        sprintf(file_name, AT_CFGFN_PROFILE, n);
        vfs_sfile_init(file_name);
    }

    gAtSetting = gAtDefaultSetting;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    memcpy(&gAtSettingSave, &gAtDefaultSetting, sizeof(atSetting_t));
#endif

#ifdef CONFIG_ATR_CFG_IN_ONE_FILE
    atCfgCombined_t *p = &gAtCfgCombined;
    vfs_sfile_init(AT_CFGFN_COMBINED);
    vfs_sfile_read(AT_CFGFN_COMBINED, p, sizeof(*p));

    if (p->global_size > 0)
        atCfgGlobalDecode(&gAtSetting, p->global_data, p->global_size);
    if (gAtSetting.profile >= CONFIG_ATR_PROFILE_COUNT)
        gAtSetting.profile = 0;
    if (p->profile_size[gAtSetting.profile] > 0)
        atCfgProfileDecode(&gAtSetting, p->profile_data[gAtSetting.profile], p->profile_size[gAtSetting.profile]);
    if (p->autosave_size > 0)
        atCfgAutoSaveDecode(&gAtSetting, p->autosave_data, p->autosave_size);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        memcpy(&gAtSettingSave, &gAtSetting, sizeof(atSetting_t));
#endif   
#else
    atCfgLoadGlobal(&gAtSetting);
    if (gAtSetting.profile >= CONFIG_ATR_PROFILE_COUNT)
        gAtSetting.profile = 0;
    atCfgLoadProfile(&gAtSetting, gAtSetting.profile);
    atCfgLoadAutoSave(&gAtSetting);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    memcpy(&gAtSettingSave, &gAtSetting, sizeof(atSetting_t));
#endif
#endif    
    if (osiGetBootMode() == OSI_BOOTMODE_PSM_RESTORE)
    {
        int psm_size = osiPsmDataRestore(OSI_PSMDATA_OWNER_AT, NULL, 0);
        if (psm_size > 0)
        {
            void *buf = malloc(psm_size);
            if (buf != NULL)
            {
                osiPsmDataRestore(OSI_PSMDATA_OWNER_AT, buf, psm_size);
                if (!atCfgProfileDecode(&gAtSetting, buf, psm_size))
                    OSI_LOGE(0, "AT psm restore decode failed size/%d", psm_size);               
                free(buf);
            }
        }
        else
        {
            OSI_LOGE(0, "AT psm restore data not exist");
        }
    }

    osiRegisterShutdownCallback(_shutdownCb, NULL);
}

bool atCfgSave(uint8_t profile, const atChannelSetting_t *chsetting)
{
    if (profile >= CONFIG_ATR_PROFILE_COUNT)
        return false;

    if (chsetting != NULL)
    {
        gAtSetting.chsetting = *chsetting;
#ifdef CONFIG_QUEC_PROJECT_FEATURE
        gAtSettingSave.chsetting = *chsetting;
#endif
    }

#ifdef CONFIG_ATR_CFG_IN_ONE_FILE
    atCfgCombined_t *p = &gAtCfgCombined;
#ifndef CONFIG_QUEC_PROJECT_FEATURE
    p->profile_size[profile] = atCfgProfileEncode(&gAtSetting, p->profile_data[profile], AT_PROFILE_SIZE);
#else
    p->profile_size[profile] = atCfgProfileEncode(&gAtSettingSave, p->profile_data[profile], AT_PROFILE_SIZE);
#endif
    return vfs_sfile_write(AT_CFGFN_COMBINED, p, sizeof(*p)) == sizeof(*p);
#else
#ifndef CONFIG_QUEC_PROJECT_FEATURE
    return atCfgStoreProfile(&gAtSetting, profile); 
#else 
    return atCfgStoreProfile(&gAtSettingSave, profile); 
#endif
#endif
}

bool atCfgAutoSave(void)
{
#ifdef CONFIG_ATR_CFG_IN_ONE_FILE
    atCfgCombined_t *p = &gAtCfgCombined;
    p->autosave_size = atCfgAutoSaveEncode(&gAtSetting, p->autosave_data, AT_AUTOSAVE_SIZE);
    return vfs_sfile_write(AT_CFGFN_COMBINED, p, sizeof(*p)) == sizeof(*p);
#else
    return atCfgStoreAutoSave(&gAtSetting);
#endif
}

bool atCfgGlobalSave(void)
{
#ifdef CONFIG_ATR_CFG_IN_ONE_FILE
    atCfgCombined_t *p = &gAtCfgCombined;
    p->global_size = atCfgGlobalEncode(&gAtSetting, p->global_data, AT_GLOBAL_SIZE);
    return vfs_sfile_write(AT_CFGFN_COMBINED, p, sizeof(*p)) == sizeof(*p);
#else
    return atCfgStoreGlobal(&gAtSetting);
#endif
}

bool atCfgResetFactory(atChannelSetting_t *chsetting)
{
#ifdef CONFIG_ATR_CFG_IN_ONE_FILE
    atCfgCombined_t *p = &gAtCfgCombined;
    p->profile_size[0] = p->profile_size[1] = 0;
    p->autosave_size = 0;
    memset(p->profile_data[0], 0, AT_PROFILE_SIZE);
    memset(p->profile_data[1], 0, AT_PROFILE_SIZE);
    memset(p->autosave_data, 0, AT_AUTOSAVE_SIZE);
    return vfs_sfile_write(AT_CFGFN_COMBINED, p, sizeof(*p)) == sizeof(*p);
#else
    atSetting_t *nset = (atSetting_t *)malloc(sizeof(atSetting_t));
    memcpy(nset, &gAtDefaultSetting, sizeof(atSetting_t));
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    uint32_t temp_ipr = gAtSetting.ipr;
    uint8_t temp_ifc_rx = gAtSetting.ifc_rx;
    uint8_t temp_ifc_tx = gAtSetting.ifc_tx;
    uint8_t temp_icf_format = gAtSetting.icf_format;
    uint8_t temp_icf_parity = gAtSetting.icf_parity;
    nset->ipr = gAtSettingSave.ipr;
    nset->ifc_rx = gAtSettingSave.ifc_rx;
    nset->ifc_tx = gAtSettingSave.ifc_tx;
    nset->icf_format = gAtSettingSave.icf_format;
    nset->icf_parity = gAtSettingSave.icf_parity; 
#endif
    if (!atCfgStoreProfile(nset, gAtSetting.profile) ||
        !atCfgStoreAutoSave(nset) ||
        !atCfgLoadProfile(&gAtSetting, gAtSetting.profile) ||
        !atCfgLoadAutoSave(&gAtSetting))
    {
        free(nset);
        return false;
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    memcpy(&gAtSettingSave, &gAtSetting, sizeof(atSetting_t));
    gAtSetting.ipr = temp_ipr;
    gAtSetting.ifc_rx = temp_ifc_rx;
    gAtSetting.ifc_tx = temp_ifc_tx;
    gAtSetting.icf_format = temp_icf_format;
    gAtSetting.icf_parity = temp_icf_parity;
#endif        
    free(nset);
    drvRtcRemoveAllAlarms();
    osiSetEpochTime(CONFIG_KERNEL_MIN_UTC_SECOND * 1000LL);
    drvRtcUpdateTime();
    if (chsetting != NULL)
        *chsetting = gAtSetting.chsetting;

    return true;
#endif
}

bool atCfgResetProfile(uint8_t profile, atChannelSetting_t *chsetting)
{
    if (profile >= CONFIG_ATR_PROFILE_COUNT)
        return false;
#ifdef CONFIG_ATR_CFG_IN_ONE_FILE
    atCfgCombined_t *p = &gAtCfgCombined;

#ifndef CONFIG_QUEC_PROJECT_FEATURE
    if (p->profile_size[gAtSetting.profile] > 0)
        atCfgProfileDecode(&gAtSetting, p->profile_data[gAtSetting.profile], p->profile_size[gAtSetting.profile]);
#else
    if (p->profile_size[gAtSettingSave.profile] > 0)
    {
        atCfgProfileDecode(&gAtSettingSave, p->profile_data[gAtSettingSave.profile], p->profile_size[gAtSettingSave.profile]);
    }
#endif
#else
#ifndef CONFIG_QUEC_PROJECT_FEATURE
    atCfgLoadProfile(&gAtSetting, profile);
#else    
    uint32_t temp_ipr = gAtSetting.ipr;
    uint8_t temp_ifc_rx = gAtSetting.ifc_rx;
    uint8_t temp_ifc_tx = gAtSetting.ifc_tx;
    uint8_t temp_icf_format = gAtSetting.icf_format;
    uint8_t temp_icf_parity = gAtSetting.icf_parity;
    atCfgLoadProfile(&gAtSetting, profile);
    memcpy(&gAtSettingSave, &gAtSetting, sizeof(atSetting_t));
    gAtSetting.ipr = temp_ipr;
    gAtSetting.ifc_rx = temp_ifc_rx;
    gAtSetting.ifc_tx = temp_ifc_tx;
    gAtSetting.icf_format = temp_icf_format;
    gAtSetting.icf_parity = temp_icf_parity;
#endif
#endif

    if (chsetting != NULL)
        *chsetting = gAtSetting.chsetting;
    return true;
}
