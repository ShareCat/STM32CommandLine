/******************************************************************************
 * File: TEA_Encrypt.h
 * Author: Bean
 * Email: notrynohigh@outlook.com
 * Every one can use this file free !
 ******************************************************************************/

#ifndef __TEA_H__
#define __TEA_H__

/******************************************************************************
 *  basic data type
 ******************************************************************************/
typedef unsigned char  TEA_U8;
typedef signed char    TEA_S8;
typedef unsigned short TEA_U16;
typedef signed short   TEA_S16;
typedef unsigned int   TEA_U32;
typedef signed int     TEA_S32;

#define TEA_NULL    ((void *)0)
/******************************************************************************
 *  define
 ******************************************************************************/
#define TEA_DEBUG_ENABLE      1

#if TEA_DEBUG_ENABLE
    #define TEA_DEBUG(...)    printf(__VA_ARGS__)
#else
    #define TEA_DEBUG(...)
#endif

/******************************************************************************
 *  typedef enum
 ******************************************************************************/
typedef enum {
    TEA_ERROR,
    TEA_SUCCESS,
    TEA_MEMORY_ERROR,
    TEA_OTHERS
} TEA_ErrorCode_t;

/******************************************************************************
 * public functions
 ******************************************************************************/

TEA_ErrorCode_t TEA_Config128bitsKey(TEA_U8 *key);
TEA_ErrorCode_t TEA_ConfigEncryptTimes(TEA_U8 t);

TEA_ErrorCode_t TEA_Encrypt(TEA_U8 *text, TEA_U32 size);
TEA_ErrorCode_t TEA_Decrypt(TEA_U8 *text, TEA_U32 size);

#endif /* __TEA_H__ */
/******************************************************************************
 *  Reserved !
 ******************************************************************************/































