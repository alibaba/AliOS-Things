#ifndef MD5_H
#define MD5_H
 
typedef struct
{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];   
}MD5_CTX;
 
void JDMD5Init(MD5_CTX *context);
void JDMD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
void JDMD5Final(MD5_CTX *context,unsigned char digest[16]);
 
#endif
