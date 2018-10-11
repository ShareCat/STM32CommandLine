

#ifndef __MD5_H
#define __MD5_H

#include <string.h>


//---------------------------------------------------------------------
typedef unsigned long UINT4;
typedef unsigned char *POINTER;

//----------------------------结构体定义----------------------------------------
/* MD5 context. */
typedef struct {
    /* state (ABCD) */
    /*四个32bits数，用于存放最终计算得到的消息摘要。当消息长度〉512bits时，也
    用于存放每个512bits的中间结果*/
    UINT4 state[4];

    /* number of bits, modulo 2^64 (lsb first) */
    /*存储原始信息的bits数长度,不包括填充的bits，最长为 2^64 bits，
    因为2^64是一个64位数的最大值*/
    UINT4 count[2];

    /* input buffer */
    /*存放输入的信息的缓冲区，512bits*/
    unsigned char buffer[64];
} MD5_CTX;

//----------------------------外部函数接口--------------------------------------
extern void md5_init(MD5_CTX *);
extern void md5_update(MD5_CTX *, unsigned char *, unsigned int);
extern void md5_final(unsigned char [16], MD5_CTX *);
extern void md5_calculation(unsigned char * input, unsigned long blen, unsigned char * output);

#endif /* __MD5_H */

