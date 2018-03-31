#ifndef __APP_LED_H
#define	__APP_LED_H

#include "stm32f10x.h"


/* LED help */
#define CLI_Cmd_LED \
    "[led]\r\n"\
    " * led 1: turn on led\r\n"\
    " * led 0: turn off led\r\n"\
    "\r\n"


extern uint8_t CLI_LED_Init(void);
extern uint8_t CLI_LED(void *para, uint8_t len);



#endif /* __APP_LED_H */
