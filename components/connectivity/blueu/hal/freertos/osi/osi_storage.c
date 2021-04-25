#include "osi_timer.h"
#include "osi_allocator.h"
#include "osi_list.h"
#include "osi_mutex.h"
#include "osi_storage.h"
#include "bt_types.h"
#include "string.h"


int osi_write(int item, UINT8 *src, int length)
{
    int ret = 0;
    //int i = 0,num = 0;
    char setting_key[20];
    memset(setting_key, 0, 20);

    switch (item)
    {
        case LINK_KEY_SITEM:
        {

        }
        break;

    }

    return ret;
}

int osi_read(int item, UINT8 *src, int length)
{
    int ret = 0;
    //int i = 0,num = 0;
    //int len_tmp = 0;
    char setting_key[20];
    memset(setting_key, 0, 20);

    switch (item)
    {
        case LINK_KEY_SITEM:
        {
        }
        break;

    }
    return ret;
}

