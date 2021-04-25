#ifndef __OSI_STORAGE_H__
#define __OSI_STORAGE_H__

typedef enum
{
    LINK_KEY_SITEM,
    LINK_VOLUME_SITEM,
} storage_item_t;
#define BT_SETTING_KEY "linkkey"
int osi_write(int item, UINT8 *src, int length);
int osi_read(int item, UINT8 *src, int length);

#endif /* __OSI_STORAGE_H__ */

