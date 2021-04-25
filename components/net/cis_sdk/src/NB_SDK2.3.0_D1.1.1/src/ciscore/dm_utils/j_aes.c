
#include "j_aes.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef CIS_EMBED
#include <unistd.h>
#else
#include <io.h>
#include <process.h>
#endif
#include "cis_internals.h"
#include <openssl/aes.h> 
#include <openssl/evp.h>  


void handleErrors(void)  
{  
  ERR_print_errors_fp(stderr);  
  abort();  
}

void hex_print(const char *name, const unsigned char *buf, size_t len)
{
    size_t i;
    fprintf(stderr, "%s ", name);
    for (i = 0; i < len; i++)
        fprintf(stderr, "%02X", buf[i]);
    fputs("\n", stderr);
}  

void sha256(char *src,int srclen,unsigned char *resultT )
{
	//unsigned char resultT[SHA256_DIGEST_LENGTH] = {0};  
    int i;  
    EVP_MD_CTX evp;  
    char *p;  
	EVP_Digest(src, strlen(src), resultT, NULL, EVP_sha256(),NULL); 	
    //hex_print("sha256:", resultT, 32);
}
  
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,  
  unsigned char *iv, unsigned char *ciphertext)  
{  
  EVP_CIPHER_CTX *ctx;  
  
  int len;  
  
  int ciphertext_len;  
  
  /* Create and initialise the context */  
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();  
  
  /* Initialise the encryption operation. IMPORTANT - ensure you use a key 
   * and IV size appropriate for your cipher 
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The 
   * IV size for *most* modes is the same as the block size. For AES this 
   * is 128 bits */  
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))  
    handleErrors();  
  
  /* Provide the message to be encrypted, and obtain the encrypted output. 
   * EVP_EncryptUpdate can be called multiple times if necessary 
   */  
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))  
    handleErrors();  
  ciphertext_len = len;  
  
  /* Finalise the encryption. Further ciphertext bytes may be written at 
   * this stage. 
   */  
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();  
  ciphertext_len += len;  
  
  /* Clean up */  
  EVP_CIPHER_CTX_free(ctx);  
  
  return ciphertext_len;  
}  
  
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,  
  unsigned char *iv, unsigned char *plaintext)  
{  
  EVP_CIPHER_CTX *ctx;  
  
  int len;  
  
  int plaintext_len;  
  
  /* Create and initialise the context */  
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();  
  
  /* Initialise the decryption operation. IMPORTANT - ensure you use a key 
   * and IV size appropriate for your cipher 
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The 
   * IV size for *most* modes is the same as the block size. For AES this 
   * is 128 bits */  
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))  
    handleErrors();  
  
  /* Provide the message to be decrypted, and obtain the plaintext output. 
   * EVP_DecryptUpdate can be called multiple times if necessary 
   */  
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))  
    handleErrors();  
  plaintext_len = len;  
  
  /* Finalise the decryption. Further plaintext bytes may be written at 
   * this stage. 
   */  
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();  
  plaintext_len += len;  
  
  /* Clean up */  
  EVP_CIPHER_CTX_free(ctx);  
  
  return plaintext_len;  
}

