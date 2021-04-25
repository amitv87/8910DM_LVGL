#ifdef CONFIG_AT_MYNET_TCPIP_SUPPORT
#include "at_command.h"
#include "ats_config.h"

void AT_TCPIP_CmdFunc_MYNETSRV(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYNETOPEN(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYNETSEND(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYNETREAD(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYNETCLOSE(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYNETACK(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYNETCON(atCommand_t *pstPara);
void AT_TCPIP_CmdFunc_MYNETCREATE(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYNETACCEPT(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYNETACT(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYIPFILTER(atCommand_t *pParam);
void AT_TCPIP_CmdFunc_MYNETURC(atCommand_t *cmd);
void AT_TCPIP_CmdFunc_MYSOCKETLED(atCommand_t *cmd);

#endif
