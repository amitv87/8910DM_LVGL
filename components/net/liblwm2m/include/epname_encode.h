#include <stdio.h>

int to_encode(unsigned char *szin, unsigned char *szout, unsigned char *szPwd);

void generate_encode_register_epname(uint8_t simid, uint8_t *pwd, uint8_t *appKey, uint8_t *version, uint8_t *epnametmp);

void generate_encode_update_epname(uint8_t *pwd, uint8_t *appKey, uint8_t *szEPI);
