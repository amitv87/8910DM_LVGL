#include "osi.h"
#include "osi_timer.h"




int osi_init(void)
{
    osi_timer_init();
    return 0;
}

int osi_reset(void)
{
    osi_timer_reset();
    return 0;
}

