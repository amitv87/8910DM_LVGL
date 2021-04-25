#ifndef DM_ENDPOINT_H
#define DM_ENDPOINT_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>
#include <stdint.h>
#include <cis_internals.h>
int prv_getDmUpdateQueryLength(st_context_t * contextP,st_server_t * server);
void dmSdkInit(void *DMconfig);
int prv_getDmUpdateQuery(st_context_t * contextP,st_server_t * server,uint8_t * buffer,size_t length);
int genDmUpdateEndpointName(char **data,void *dmconfig);
int genDmRegEndpointName(char ** data,void *dmconfig);
int dm_read_info_encode(char *szin,char **szout, char *pwd);
#ifdef __cplusplus
}
#endif
#endif