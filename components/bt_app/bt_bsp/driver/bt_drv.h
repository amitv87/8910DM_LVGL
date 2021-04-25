#ifndef BT_DRV_H
#define BT_DRV_H

#include "sci_types.h"

bool bt_chip_startup(void);
void bt_chip_shutdown(void);
void UartDrv_Configure(uint32 baudrate);
bool UartDrv_Start(void);
void UartDrv_Stop(void);
void bt_disable_deep_sleep(void);
bool bt_chip_enter_sleep_check(void);

#endif
