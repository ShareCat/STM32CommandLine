/**
  ******************************************************************************
  * @file:      
  * @author:    Cat  
  * @version:   V1.0  
  * @date:      2018-1-23
  * @brief:     
  * @attention: 
  ******************************************************************************
  */

  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "project_conf.h"   /* 工程配置文件，编译前要查看，确定 */

/* Macro ---------------------------------------------------------------------*/
//#define STM32F0             /* 选择是STM32F0，如果是STM32F1就需要注释掉 */

/* Includes ------------------------------------------------------------------*/

#ifdef STM32F0
#include "stm32f0xx.h"      /* 包含STM32F0的头文件 */
//#define STM32F0_CPU_72M   /* STM32F0默认最高48M主频，可以倍频到72M */
#else
#include "stm32f10x.h"      /* 包含STM32F1的头文件，默认是    STM32F1 */
#endif


/* stm32直接操作寄存器的方法控制IO */
#define	IO_HIGH(p,i)		{p->BSRR=i;}	/* 输出为高电平 */
#define IO_LOW(p,i)		    {p->BRR=i;}	    /* 输出低电平 */
#define IO_TOGGLE(p,i)      {p->ODR ^=i;}   /* 输出反转状态 */


/* Exported types ------------------------------------------------------------*/
#if 0
typedef union  
{  
    unsigned char byte;
    struct   
    {  
        unsigned char bit0:1;
        unsigned char bit1:1;
        unsigned char bit2:1;
        unsigned char bit3:1;
        unsigned char bit4:1;
        unsigned char bit5:1;
        unsigned char bit6:1;
        unsigned char bit7:1;
    }bits;  
}BYTE;
#endif

/* Exported constants --------------------------------------------------------*/

//位或置1  如:a |= SETBIT0
enum
{
    SETBIT0 = 0x0001,   SETBIT1 = 0x0002,	SETBIT2 = 0x0004,	SETBIT3 = 0x0008,
    SETBIT4 = 0x0010,	SETBIT5 = 0x0020,	SETBIT6 = 0x0040,	SETBIT7 = 0x0080,
    SETBIT8 = 0x0100,	SETBIT9 = 0x0200,	SETBIT10 = 0x0400,  SETBIT11 = 0x0800,
    SETBIT12 = 0x1000,  SETBIT13 = 0x2000,	SETBIT14 = 0x4000,  SETBIT15 = 0x8000		
};

//位与清0  如:a &= CLRBIT0
enum
{
    CLRBIT0 = 0xFFFE,   CLRBIT1 = 0xFFFD,	CLRBIT2 = 0xFFFB,	CLRBIT3 = 0xFFF7,	
    CLRBIT4 = 0xFFEF,	CLRBIT5 = 0xFFDF,	CLRBIT6 = 0xFFBF,	CLRBIT7 = 0xFF7F,
    CLRBIT8 = 0xFEFF,	CLRBIT9 = 0xFDFF,	CLRBIT10 = 0xFBFF,  CLRBIT11 = 0xF7FF,
    CLRBIT12 = 0xEFFF,  CLRBIT13 = 0xDFFF,	CLRBIT14 = 0xBFFF,  CLRBIT15 = 0x7FFF
};

//位与选择  如: a = b&CHSBIT0
enum
{
    CHSBIT0 = 0x0001,   CHSBIT1 = 0x0002,	CHSBIT2 = 0x0004,   CHSBIT3 = 0x0008,
    CHSBIT4 = 0x0010,	CHSBIT5 = 0x0020,	CHSBIT6 = 0x0040,	CHSBIT7 = 0x0080,
    CHSBIT8 = 0x0100,	CHSBIT9 = 0x0200,	CHSBIT10 = 0x0400,  CHSBIT11 = 0x0800,
    CHSBIT12 = 0x1000,  CHSBIT13 = 0x2000,	CHSBIT14 = 0x4000,  CHSBIT15 = 0x8000		
};

//分时执行状态
enum
{
    STEP0 = 0,         STEP1 = 1,         STEP2 = 2,         STEP3 = 3,
    STEP4 = 4,         STEP5 = 5,         STEP6 = 6,         STEP7 = 7,
    STEP8 = 8,         STEP9 = 9,         STEP10 = 10,       STEP11 = 11,
    STEP12 = 12,       STEP13 = 13,       STEP14 = 14,       STEP15 = 15,
    STEP16 = 16,       STEP17 = 17,       STEP18 = 18,       STEP19 = 19,
    STEP20 = 20,       STEP21 = 21,       STEP22 = 22,       STEP23 = 23,
    STEP24 = 24,       STEP25 = 25,       STEP26 = 26,       STEP27 = 27,
    STEP28 = 28,       STEP29 = 29,       STEP30 = 30,       STEP31 = 31,
    STEP32 = 32,       STEP33 = 33,       STEP34 = 34,       STEP35 = 35
};


/* Exported macro ------------------------------------------------------------*/
#ifndef     NULL
#define     NULL            (0)
#endif
#define     TRUE            (1)
#define     FALSE           (0)
#define     ZERO            (0)
#define     ONE         	(1)
#define     TWO         	(2)
#define     THREE       	(3)
#define     FOUR        	(4)
#define     FIVE        	(5)
#define     SIX         	(6)
#define     SEVEN       	(7)
#define     EIGHT       	(8)
#define     NINE        	(9)
#define     TEN         	(10)
#define     HUNDRED     	(100)
#define     THOUSAND    	(1000)
#define     HALFBYTEMAX 	(15)
#define     ONEBYTEMAX  	(255)


//两变量交换
#define _SWAP(a, b)     ((a) ^= (b), (b) ^= (a), (a) ^= (b))
//数组大小，不能用来比较字符串大小，字符串比较需要用strlen
#define _ARRAY_SIZE(a)  ((sizeof(a)) / (sizeof(a[0])))

/* 大小端转换宏 */
#define BIG_LITTLE_SWAP16(A)        ((((uint16_t)(A) & 0xff00) >> 8)        \
                                    | (((uint16_t)(A) & 0x00ff) << 8))

#define BIG_LITTLE_SWAP32(A)        ((((uint32_t)(A) & 0xff000000) >> 24)   \
                                    | (((uint32_t)(A) & 0x00ff0000) >> 8)   \
                                    | (((uint32_t)(A) & 0x0000ff00) << 8)   \
                                    | (((uint32_t)(A) & 0x000000ff) << 24))

#define _SET_BIT(target, bit) ((target) |= (unsigned long)(((unsigned long)1) << (bit)))
#define _CLR_BIT(target, bit) ((target) &= (unsigned long)(~(((unsigned long)1) << (bit))))
#define _GET_BIT(target, bit) ((target) & (((unsigned long)1) << (bit)))
#define _FLP_BIT(target, bit) ((target) ^= (((unsigned long)1) << (bit))) 



/* Exported functions ------------------------------------------------------- */

#endif /* __GLOBAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
