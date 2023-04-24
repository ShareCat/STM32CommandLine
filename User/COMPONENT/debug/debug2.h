/**
  ******************************************************************************
  * @file:      debug.h
  * @author:    孙关平
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     调试命令行
  * @attention:
  ******************************************************************************
  */

#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define __DEBUG__           1       /* 1，表示开启调试 */
#define __COLOR_DEBUG__     1       /* 1，表示开启颜色log */


/* Macro config --------------------------------------------------------------*/

#if __DEBUG__
    #define PRINTF(...)         printf(__VA_ARGS__)
    #define SPRINTF(...)        sprintf(__VA_ARGS__)

#if __COLOR_DEBUG__
#define LOGE(fmt, args...)   do {                                           \
                                TERMINAL_FONT_RED();                        \
                                PRINTF("[LOGE] %s(%d): "fmt,                \
                                    __FUNCTION__, __LINE__, ##args);        \
                                TERMINAL_FONT_WHITE();                      \
                            } while(0)
#define LOGI(fmt, args...)   do {                                           \
                                TERMINAL_FONT_CYAN();                       \
                                PRINTF("[LOGI] %s(%d): "fmt,                \
                                    __FUNCTION__, __LINE__, ##args);        \
                                TERMINAL_FONT_WHITE();                      \
                            } while(0)
#define LOGD(fmt, args...)   do {                                           \
                                TERMINAL_FONT_YELLOW();                     \
                                PRINTF("[LOGD] %s(%d): "fmt,                \
                                    __FUNCTION__, __LINE__, ##args);        \
                                TERMINAL_FONT_WHITE();                      \
                            } while(0)
#else
#define LOGE(fmt, args...)   do {                                           \
                                PRINTF("[LOGE] %s(%d): "fmt,                \
                                    __FUNCTION__, __LINE__, ##args);        \
                            } while(0)
#define LOGI(fmt, args...)   do {                                           \
                                PRINTF("[LOGI] %s(%d): "fmt,                \
                                    __FUNCTION__, __LINE__, ##args);        \
                            } while(0)
#define LOGD(fmt, args...)   do {                                           \
                                PRINTF("[LOGD] %s(%d): "fmt,                \
                                    __FUNCTION__, __LINE__, ##args);        \
                            } while(0)
#endif

#else
    #define PRINTF(...)     ;
    #define SPRINTF(...)    ;
#endif /* __DEBUG__ */


/* 控制终端显示---------------------------------------------------------BEGIN */

/* 字体颜色 */
#define TERMINAL_FONT_BLACK()       PRINTF("\033[1;30m")
#define TERMINAL_FONT_L_RED()       PRINTF("\033[0;31m")    /* 浅红色 */
#define TERMINAL_FONT_RED()         PRINTF("\033[1;31m")    /* 红色 */
#define TERMINAL_FONT_GREEN()       PRINTF("\033[1;32m")
#define TERMINAL_FONT_YELLOW()      PRINTF("\033[1;33m")
#define TERMINAL_FONT_BLUE()        PRINTF("\033[1;34m")
#define TERMINAL_FONT_PURPLE()      PRINTF("\033[1;35m")
#define TERMINAL_FONT_CYAN()        PRINTF("\033[1;36m")
#define TERMINAL_FONT_WHITE()       PRINTF("\033[1;37m")

/* 背景颜色 */
#define TERMINAL_BACK_BLACK()       PRINTF("\033[1;40m")
#define TERMINAL_BACK_L_RED()       PRINTF("\033[0;41m")    /* 浅红色 */
#define TERMINAL_BACK_RED()         PRINTF("\033[1;41m")    /* 红色 */
#define TERMINAL_BACK_GREEN()       PRINTF("\033[1;42m")
#define TERMINAL_BACK_YELLOW()      PRINTF("\033[1;43m")
#define TERMINAL_BACK_BLUE()        PRINTF("\033[1;44m")
#define TERMINAL_BACK_PURPLE()      PRINTF("\033[1;45m")
#define TERMINAL_BACK_CYAN()        PRINTF("\033[1;46m")
#define TERMINAL_BACK_WHITE()       PRINTF("\033[1;47m")

/* 控制终端显示-----------------------------------------------------------END */

#endif /* __DEBUG_H */
