#ifndef __COMMAND_LINE_H
#define	__COMMAND_LINE_H

#include <stdint.h>
#include <string.h>
#include "queue.h"


/**
  * CLI macro
  */
#define USING_CML

#ifdef USING_CML
#define CLI_INIT(...)       CLI_Init(__VA_ARGS__)
#define CLI_RUN(...)        CLI_Run(__VA_ARGS__)
#else
#define CLI_INIT(...)
#define CLI_RUN(...)
#endif



/**
  * debug macro
  */
#define USING_printf

#ifdef USING_printf
#define PRINTF(...)         printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* debug----------------------------------------------------------------BEGIN */

#define ERR(fmt)            do { TERMINAL_FONT_RED(); PRINTF("### ERROR ###: "fmt"\r\n"); TERMINAL_FONT_GREEN(); } while(0)
#define ERRA(fmt, ...)      do { TERMINAL_FONT_RED(); PRINTF("### ERROR ### %s(%d): "fmt"\r\n", __FUNCTION__, __LINE__, __VA_ARGS__); TERMINAL_FONT_GREEN(); } while(0)
#define LOG(fmt)            do { TERMINAL_FONT_BLUE(); PRINTF("[Log]: "fmt"\r\n"); TERMINAL_FONT_GREEN(); } while(0)
#define LOGA(fmt, ...)      do { TERMINAL_FONT_BLUE(); PRINTF("[Log]: "fmt"\r\n", __VA_ARGS__); TERMINAL_FONT_GREEN(); } while(0)
#define DBG(fmt)            do { TERMINAL_FONT_YELLOW(); PRINTF("[Debug] %s(%d): "fmt"\r\n", __FUNCTION__, __LINE__); TERMINAL_FONT_GREEN(); } while(0)
#define DBGA(fmt, ...)      do { TERMINAL_FONT_YELLOW(); PRINTF("[Debug] %s(%d): "fmt"\r\n", __FUNCTION__, __LINE__, __VA_ARGS__); TERMINAL_FONT_GREEN(); } while(0)
#define DIE(fmt)            do { TERMINAL_FONT_RED(); TERMINAL_HIGH_LIGHT(); PRINTF("### DIE ### %s(%d): "fmt"\r\n", __FUNCTION__, __LINE__); } while(1)
#define NL1()               do { PRINTF("\r\n"); } while(0)
#define NL2()               do { PRINTF("\r\n\r\n"); } while(0)
#define NL3()               do { PRINTF("\r\n\r\n\r\n"); } while(0)

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


    @background color: 40--49           font color: 30--39
     40: BLACK                          30: black
     41: RED                            31: red
     42: GREEN                          32: green
     43: YELLOW                         33: yellow
     44: BLUE                           34: blue
     45: PURPLE                         35: purple
     46: CYAN                           36: deep green
     47: WHITE                          37: white
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




#define CLI_BAUDRATE        ((uint32_t)115200)

#define USART_INIT(baud)    Debug_USART_Init(baud)

#define SYSTEM_REBOOT()     NVIC_SystemReset() /* MCU reboot */


#define RX_BUFF_TYPE        QUEUE64_TYPE

extern RX_BUFF_TYPE         CLI_RX_Buff;


typedef struct {
    const char *pCmd;
    const char *pHelp;
    uint8_t (*pInit)(void);
    uint8_t (*pFun)(void *, uint8_t);
} COMMAND_TYPE;



extern void CLI_Init(uint32_t baud);
extern void CLI_Run(void);


#endif /* __COMMAND_LINE_H */

