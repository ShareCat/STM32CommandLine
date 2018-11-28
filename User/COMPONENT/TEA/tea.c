/******************************************************************************
 * File: TEA_Encrypt.c
 * Author: Bean
 * Email: notrynohigh@outlook.com
 * Every one can use this file free !
 ******************************************************************************/
/** Include -----------------------------------------------------------------*/
#include "stdio.h"
#include "tea.h"

/** defined -----------------------------------------------------------------*/
#define TEA_DELTA                   0x9e3779b9
#define TEA_KEY_LEN                 (16)
#define DEFAULT_ENCRYPT_TIMES       (16)

/** global variable --------------------------------------------------------*/
static TEA_U8 gTEA_EncryptTimes = DEFAULT_ENCRYPT_TIMES;
static TEA_U8 gTEA_KeyBuf[TEA_KEY_LEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};



/** static functions -------------------------------------------------------*/
static TEA_ErrorCode_t TEA_EncryptGroup(TEA_U32 *text, TEA_U32 *key)
{
    TEA_U32 sum = 0, v0 = text[0], v1 = text[1];
    TEA_U32 k0 = key[0], k1 = key[1], k2 = key[2], k3 = key[3];
    TEA_U8 i = 0;

    if (text == TEA_NULL || key == TEA_NULL) {
        return TEA_ERROR;
    }

    for (i = 0; i < gTEA_EncryptTimes; i++) {
        sum += TEA_DELTA;
        v0 += (v1 << 4) + k0 ^ v1 + sum ^ (v1 >> 5) + k1;
        v1 += (v0 << 4) + k2 ^ v0 + sum ^ (v0 >> 5) + k3;
    }

    text[0] = v0;
    text[1] = v1;
    return TEA_SUCCESS;
}

static TEA_ErrorCode_t TEA_DecryptGroup(TEA_U32 *text, TEA_U32 *key)
{
    TEA_U32 sum = TEA_DELTA * gTEA_EncryptTimes, v0 = text[0], v1 = text[1];
    TEA_U32 k0 = key[0], k1 = key[1], k2 = key[2], k3 = key[3];
    TEA_U8 i = 0;

    if (text == TEA_NULL || key == TEA_NULL) {
        return TEA_ERROR;
    }

    for (i = 0; i < gTEA_EncryptTimes; i++) {
        v1 -= (v0 << 4) + k2 ^ v0 + sum ^ (v0 >> 5) + k3;
        v0 -= (v1 << 4) + k0 ^ v1 + sum ^ (v1 >> 5) + k1;
        sum -= TEA_DELTA;
    }

    text[0] = v0;
    text[1] = v1;
    return TEA_SUCCESS;
}




/** public functions -------------------------------------------------------*/
TEA_ErrorCode_t TEA_Config128bitsKey(TEA_U8 *key)
{
    TEA_U8 i = 0;

    if (key == TEA_NULL) {
        return TEA_ERROR;
    }

    for (i = 0; i < TEA_KEY_LEN; i++) {
        gTEA_KeyBuf[i] = key[i];
    }

    return TEA_SUCCESS;
}


TEA_ErrorCode_t TEA_ConfigEncryptTimes(TEA_U8 t)
{
    gTEA_EncryptTimes = (t < DEFAULT_ENCRYPT_TIMES) ? DEFAULT_ENCRYPT_TIMES : t;
    return TEA_SUCCESS;
}



TEA_ErrorCode_t TEA_Encrypt(TEA_U8 *text, TEA_U32 size)
{
    TEA_U32 number = size >> 3;
    TEA_U8  i = 0;

    if (text == NULL || size < 8) {
        TEA_DEBUG("at least eight characters !\n\r");
        return TEA_ERROR;
    }

    for (i = 0; i < number; i++) {
        TEA_EncryptGroup(&(((TEA_U32 *)text)[i * 2]), (TEA_U32 *)gTEA_KeyBuf);
    }

    return TEA_SUCCESS;
}


TEA_ErrorCode_t TEA_Decrypt(TEA_U8 *text, TEA_U32 size)
{
    TEA_U32 number = size >> 3;
    TEA_U8  i = 0;

    if (text == NULL || size < 8) {
        TEA_DEBUG("at least eight characters !\n\r");
        return TEA_ERROR;
    }

    for (i = 0; i < number; i++) {
        TEA_DecryptGroup(&(((TEA_U32 *)text)[i * 2]), (TEA_U32 *)gTEA_KeyBuf);
    }

    return TEA_SUCCESS;
}
/******************************************************************************
* end !
******************************************************************************/








