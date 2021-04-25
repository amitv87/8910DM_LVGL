
#ifndef __J_AES_H__
#define __J_AES_H__

#include <stdio.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

void handleErrors(void) ;
void hex_print(const char *name, const unsigned char *buf, size_t len);

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,  
  unsigned char *iv, unsigned char *ciphertext);
  
 int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,  
  unsigned char *iv, unsigned char *plaintext);

#ifdef __cplusplus
}
#endif


#endif //__J_BASE_64_H__

