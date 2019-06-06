/**
  ******************************************************************************
  * @file:      sys_command_line.h
  * @author:    Cat
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     command line
  * @attention:
  ******************************************************************************
  */


#ifndef __SYS_COMMAND_LINE_H
#define __SYS_COMMAND_LINE_H


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sys_queue.h"


/* Macro config --------------------------------------------------------------*/
#define CLI_ENABLE          TRUE                    /* command line enable/disable */
#define CLI_HISTORY         TRUE                    /* history command enable/disable */
#define HISTORY_MAX         10                      /* maxium number ofhistory command */
#define CLI_PRINTF          TRUE                    /* printf for debug */
#define CLI_BAUDRATE        ((uint32_t)115200)      /* band rate */
#define USART_INIT(baud)    debug_usart_init(baud)  /* usart init. */
#define SYSTEM_REBOOT()     NVIC_SystemReset()      /* MCU reboot */


#define KEY_UP              "\x1b\x5b\x41"  /* [up] key: 0x1b 0x5b 0x41 */
#define KEY_DOWN            "\x1b\x5b\x42"  /* [down] key: 0x1b 0x5b 0x42 */
#define KEY_RIGHT           "\x1b\x5b\x43"  /* [right] key: 0x1b 0x5b 0x43 */
#define KEY_LEFT            "\x1b\x5b\x44"  /* [left] key: 0x1b 0x5b 0x44 */
#define KEY_ENTER           '\r'            /* [enter] key */
#define KEY_BACKSPACE       '\b'            /* [backspace] key */


#if CLI_ENABLE
    #define CLI_INIT(...)       cli_init(__VA_ARGS__)
    #define CLI_RUN(...)        cli_run(__VA_ARGS__)
#else
    #define CLI_INIT(...)       ;
    #define CLI_RUN(...)        ;
#endif /* CLI_ENABLE */


#if CLI_PRINTF
    #define PRINTF(...)         printf(__VA_ARGS__)
    #define SPRINTF(...)        sprintf(__VA_ARGS__)

enum {
    E_FONT_BLACK,
    E_FONT_L_RED,
    E_FONT_RED,
    E_FONT_GREEN,
    E_FONT_YELLOW,
    E_FONT_BLUE,
    E_FONT_PURPLE,
    E_FONT_CYAN,
    E_FONT_WHITE,
};

    #define PRINTF_COLOR(c, ...)    do {                            \
                                        switch (c) {                \
                                            case E_FONT_BLACK:      \
                                            TERMINAL_FONT_BLACK();  \
                                            break;                  \
                                            case E_FONT_L_RED:      \
                                            TERMINAL_FONT_L_RED();  \
                                            break;                  \
                                            case E_FONT_RED:        \
                                            TERMINAL_FONT_RED();    \
                                            break;                  \
                                            case E_FONT_GREEN:      \
                                            TERMINAL_FONT_GREEN();  \
                                            break;                  \
                                            case E_FONT_YELLOW:     \
                                            TERMINAL_FONT_YELLOW(); \
                                            break;                  \
                                            case E_FONT_BLUE:       \
                                            TERMINAL_FONT_BLUE();   \
                                            break;                  \
                                            case E_FONT_PURPLE:     \
                                            TERMINAL_FONT_PURPLE(); \
                                            break;                  \
                                            case E_FONT_CYAN:       \
                                            TERMINAL_FONT_CYAN();   \
                                            break;                  \
                                            case E_FONT_WHITE:      \
                                            TERMINAL_FONT_WHITE();  \
                                            break;                  \
                                        }                           \
                                        printf(__VA_ARGS__);        \
                                        TERMINAL_FONT_GREEN();      \
                                    } while(0)
#else
    #define PRINTF(...)         ;
    #define SPRINTF(...)        ;
    #define PRINTF_COLOR(c, ...);
#endif /* CLI_PRINTF */




/* assert---------------------------------------------------------------BEGIN */

/* #val：使用#把宏参数变为一个字符串,用##把两个宏参数贴合在一起 */
#define ASSERT(val)                                                     \
if (!(val))                                                             \
{                                                                       \
    PRINTF("(%s) has assert failed at %s.\n", #val, __FUNCTION__);      \
    while (1);                                                          \
}

/* assert-----------------------------------------------------------------END */




/* debug----------------------------------------------------------------BEGIN */

#define ERR(fmt)        do {                                            \
                            TERMINAL_FONT_RED();                        \
                            PRINTF("### ERROR ###: "fmt"\r\n");         \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define ERRA(fmt, ...)  do {                                            \
                            TERMINAL_FONT_RED();                        \
                            PRINTF("### ERROR ### %s(%d): "fmt"\r\n",   \
                                __FUNCTION__, __LINE__, __VA_ARGS__);   \
                            TERMINAL_FONT_GREEN();                      \
                        }while(0)
#define LOG(fmt)        do {                                            \
                            TERMINAL_FONT_CYAN();                       \
                            PRINTF("[Log]: "fmt"\r\n");                 \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define LOGA(fmt, ...)  do {                                            \
                            TERMINAL_FONT_CYAN();                       \
                            PRINTF("[Log]: "fmt"\r\n", __VA_ARGS__);    \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define DBG(fmt)        do {                                            \
                            TERMINAL_FONT_YELLOW();                     \
                            PRINTF("[Debug] %s(%d): "fmt"\r\n",         \
                                __FUNCTION__, __LINE__);                \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define DBGA(fmt, ...)  do {                                            \
                            TERMINAL_FONT_YELLOW();                     \
                            PRINTF("[Debug] %s(%d): "fmt"\r\n",         \
                                __FUNCTION__, __LINE__, __VA_ARGS__);   \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define DIE(fmt)        do {                                            \
                            TERMINAL_FONT_RED();                        \
                            TERMINAL_HIGH_LIGHT();                      \
                            PRINTF("### DIE ### %s(%d): "fmt"\r\n",     \
                                __FUNCTION__, __LINE__);                \
                        } while(1) /* infinite loop */
#define NL1()           do { PRINTF("\r\n"); } while(0)
#define NL2()           do { PRINTF("\r\n\r\n"); } while(0)
#define NL3()           do { PRINTF("\r\n\r\n\r\n"); } while(0)

/* debug------------------------------------------------------------------END */




/* terminal display-----------------------------------------------------BEGIN */

/*
    @links: http://blog.csdn.net/yangguihao/article/details/47734349
            http://blog.csdn.net/kevinshq/article/details/8179252


    @terminal setting commands:
        \033[0m     reset all
        \033[1m     set high brightness
        \03[4m      underline
        \033[5m     flash
        \033[7m     reverse display
        \033[8m     blanking
        \033[30m    --  \033[37m  set font color
        \033[40m    --  \033[47m  set background color
        \033[nA     cursor up up n lines
        \033[nB     cursor move up n lines
        \033[nC     cursor move right n lines
        \033[nD     cursor left up n lines
        \033[y;xH   set cursor position
        \033[2J     clear all display
        \033[K      clear line
        \033[s      save cursor position
        \033[u      restore cursor position
        \033[?25l   cursor invisible
        \33[?25h    cursor visible


    @background color: 40--49           @font color: 30--39
        40: BLACK                           30: black
        41: RED                             31: red
        42: GREEN                           32: green
        43: YELLOW                          33: yellow
        44: BLUE                            34: blue
        45: PURPLE                          35: purple
        46: CYAN                            36: deep green
        47: WHITE                           37: white
*/

/* font color */
#define TERMINAL_FONT_BLACK()       PRINTF("\033[1;30m")
#define TERMINAL_FONT_L_RED()       PRINTF("\033[0;31m")    /* light red */
#define TERMINAL_FONT_RED()         PRINTF("\033[1;31m")    /* red */
#define TERMINAL_FONT_GREEN()       PRINTF("\033[1;32m")
#define TERMINAL_FONT_YELLOW()      PRINTF("\033[1;33m")
#define TERMINAL_FONT_BLUE()        PRINTF("\033[1;34m")
#define TERMINAL_FONT_PURPLE()      PRINTF("\033[1;35m")
#define TERMINAL_FONT_CYAN()        PRINTF("\033[1;36m")
#define TERMINAL_FONT_WHITE()       PRINTF("\033[1;37m")

/* background color */
#define TERMINAL_BACK_BLACK()       PRINTF("\033[1;40m")
#define TERMINAL_BACK_L_RED()       PRINTF("\033[0;41m")    /* light red */
#define TERMINAL_BACK_RED()         PRINTF("\033[1;41m")    /* red */
#define TERMINAL_BACK_GREEN()       PRINTF("\033[1;42m")
#define TERMINAL_BACK_YELLOW()      PRINTF("\033[1;43m")
#define TERMINAL_BACK_BLUE()        PRINTF("\033[1;44m")
#define TERMINAL_BACK_PURPLE()      PRINTF("\033[1;45m")
#define TERMINAL_BACK_CYAN()        PRINTF("\033[1;46m")
#define TERMINAL_BACK_WHITE()       PRINTF("\033[1;47m")

/* terminal clear end */
#define TERMINAL_CLEAR_END()        PRINTF("\033[K")

/* terminal clear all */
#define TERMINAL_DISPLAY_CLEAR()    PRINTF("\033[2J")

/* cursor move up */
#define TERMINAL_MOVE_UP(x)         PRINTF("\033[%dA", (x))

/* cursor move down */
#define TERMINAL_MOVE_DOWN(x)       PRINTF("\033[%dB", (x))

/* cursor move left */
#define TERMINAL_MOVE_LEFT(y)       PRINTF("\033[%dD", (y))

/* cursor move right */
#define TERMINAL_MOVE_RIGHT(y)      PRINTF("\033[%dC",(y))

/* cursor move to */
#define TERMINAL_MOVE_TO(x, y)      PRINTF("\033[%d;%dH", (x), (y))

/* cursor reset */
#define TERMINAL_RESET_CURSOR()     PRINTF("\033[H")

/* cursor invisible */
#define TERMINAL_HIDE_CURSOR()      PRINTF("\033[?25l")

/* cursor visible */
#define TERMINAL_SHOW_CURSOR()      PRINTF("\033[?25h")

/* reverse display */
#define TERMINAL_HIGH_LIGHT()       PRINTF("\033[7m")
#define TERMINAL_UN_HIGH_LIGHT()    PRINTF("\033[27m")

/* terminal display-------------------------------------------------------END */


#define RX_BUFF_TYPE        QUEUE64_S
extern RX_BUFF_TYPE         cli_rx_buff;


__packed typedef struct {
    const char *pCmd;
    const char *pHelp;
    uint8_t (*pInit)(void);
    uint8_t (*pFun)(void *, uint8_t);
} COMMAND_S;



extern void cli_init(uint32_t baud);
extern void cli_task(void);


#endif /* __SYS_COMMAND_LINE_H */

