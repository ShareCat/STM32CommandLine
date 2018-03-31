

#include "command_line.h"
#include "../bsp/bsp_usart.h"
#include "../app/app_led.h"



static uint8_t CLI_Help(void *para, uint8_t len);
static uint8_t CLI_Clear(void *para, uint8_t len);
static uint8_t CLI_Echo(void *para, uint8_t len);
static uint8_t CLI_Reboot(void *para, uint8_t len);


__packed typedef struct {
#define HANDLE_LEN 128

    uint8_t buff[HANDLE_LEN];
    uint8_t len;
} HANDLE_TYPE;



uint8_t CLI_EchoFlag = DISABLE; /* ECHO default: ON */

RX_BUFF_TYPE CLI_RX_Buff; /* 128byte FIFO, saving command */



const char CLI_Cmd_Help[] =
    "\r\n"
    "[help]\r\n"
    " * get all the commands\r\n"
    "\r\n";

const char CLI_Cmd_Clear[] =
    "[cls]\r\n"
    " * clear the screen\r\n"
    "\r\n";

const char CLI_Cmd_Echo[] =
    "[echo]\r\n"
    " * echo 1: turn on echo\r\n"
    " * echo 0: turn off echo\r\n"
    "\r\n";

const char CLI_Cmd_Reboot[] =
    "[reboot]\r\n"
    " * reboot MCU\r\n"
    "\r\n";


/**
  * cmd struct
  */
const COMMAND_TYPE CLI_Cmd[] = {
    /* cmd            cmd help              init func.      func. */
    {"help\r",        CLI_Cmd_Help,         NULL,           CLI_Help},
    {"cls\r",         CLI_Cmd_Clear,        NULL,           CLI_Clear},
    {"echo",          CLI_Cmd_Echo,         NULL,           CLI_Echo},
    {"reboot\r",      CLI_Cmd_Reboot,       NULL,           CLI_Reboot},
    {"led",           CLI_Cmd_LED,          CLI_LED_Init,   CLI_LED},
};


/**
  * @brief  printf the help info.
  * @param  para addr. & length
  * @retval True means OK
  */
static uint8_t CLI_Help(void *para, uint8_t len)
{
    uint8_t i;
    for (i = 0; i < sizeof(CLI_Cmd) / sizeof(COMMAND_TYPE); i++) {
        PRINTF(CLI_Cmd[i].pHelp);
    }

    return True;
}


/**
  * @brief  clear the screen
  * @param  para addr. & length
  * @retval True means OK
  */
static uint8_t CLI_Clear(void *para, uint8_t len)
{
    TERMINAL_BACK_BLACK(); /* set terminal background color: black */
    TERMINAL_FONT_GREEN(); /* set terminal display color: green */

    /* This prints the clear screen and move cursor to top-left corner control
     * characters for VT100 terminals. This means it will not work on
     * non-VT100 compliant terminals, namely Windows' cmd.exe, but should
     * work on anything unix-y. */
    TERMINAL_RESET_CURSOR();
    TERMINAL_DISPLAY_CLEAR();

    return True;
}


/**
  * @brief  ECHO setting
  * @param  para addr. & length
  * @retval True means OK
  */
static uint8_t CLI_Echo(void *para, uint8_t len)
{
    uint8_t *pTemp;
    pTemp = (uint8_t *)para;
    if ((0 < len) && (NULL != pTemp)) {
        pTemp++; /* skip a blank space*/
        if ('1' == *pTemp) {
            /* ECHO on */
            CLI_EchoFlag = ENABLE;
            PRINTF("echo on\r\n");
        } else if ('0' == *pTemp) {
            /* ECHO off */
            CLI_EchoFlag = DISABLE;
            PRINTF("echo off\r\n");
        } else {
            /* wrong para, return False */
            return False;
        }
    }

    return True;
}


/**
  * @brief  get MCU info.
  * @param  para addr. & length
  * @retval True means OK
  */
static uint8_t CLI_MCU_Info(void *para, uint8_t len)
{
    uint16_t flash_size;
    uint32_t MAC_buff[3];

    /* FLASH size */
    flash_size = *(__IO uint16_t *)(0x1FFF7A22);
    PRINTF("Flash size: %d KB\r\n", flash_size);

    /* ID */
    MAC_buff[0] = *(__IO uint32_t*)(0x1FFF7A10);
    MAC_buff[1] = *(__IO uint32_t*)(0x1FFF7A14);
    MAC_buff[2] = *(__IO uint32_t*)(0x1FFF7A18);
    PRINTF("UID(hex): %02X-%02X-%02X\r\n", MAC_buff[0], MAC_buff[1], MAC_buff[2]);

    return True;
}


/**
  * @brief  MCU reboot
  * @param  para addr. & length
  * @retval True means OK
  */
static uint8_t CLI_Reboot(void *para, uint8_t len)
{
    extern void Delay(__IO uint32_t nCount);
    PRINTF("\r\n[END]: System Rebooting");
    PRINTF(".");
    Delay(0x8FFFFF);
    PRINTF(".");
    Delay(0x8FFFFF);
    PRINTF(".");

    SYSTEM_REBOOT();

    return True;
}


/**
  * @brief  string compare
  * @param  two string
  * @retval null
  */
uint8_t StrComp(void * buffer,void * StrCmd)
{
    uint8_t i;
    uint8_t * ptBuf;
    uint8_t * ptCmd;

    ptBuf = (uint8_t *)buffer;
    ptCmd = (uint8_t *)StrCmd;
    for(i=0; i<255; i++) {
        if(ptCmd[i]) {
            if(ptBuf[i] != ptCmd[i])return False;
        } else {
            if(i)return True;
            else return False;
        }
    }
    return False;
}


/**
  * @brief  string compare strict
  * @param  two string
  * @retval null
  */
uint8_t StrEqual(void * buffer,void * StrCmd)
{
    volatile uint8_t i;
    uint8_t * ptBuf;
    uint8_t * ptCmd;

    ptBuf = (uint8_t *)buffer;
    ptCmd = (uint8_t *)StrCmd;
    for(i=0; i<255; i++) {
        if(ptCmd[i]) {
            if(ptBuf[i] != ptCmd[i])return False;
        } else {
            break;
        }
    }

    if(ptBuf[i] != ptCmd[i])return False;
    else if(0 == i)return False;
    else return True;
}


/**
  * @brief  command line init.
  * @param  bandrate
  * @retval null
  */
void CLI_Init(uint32_t baud)
{
    uint8_t i;
    memset((uint8_t *)&CLI_RX_Buff, 0, sizeof(RX_BUFF_TYPE));
    USART_INIT(baud);

    /* init. every command */
    for (i = 0; i < sizeof(CLI_Cmd) / sizeof(COMMAND_TYPE); i++) {
        /* need to init. or not */
        if (NULL != CLI_Cmd[i].pInit) {
            if (False == CLI_Cmd[i].pInit()) {
                /* something wrong */
                PRINTF("\r\n-> FUN[%d] INIT WRONG\r\n", i);
            }
        }
    }

    PRINTF(" \r\n");
    TERMINAL_BACK_BLACK(); /* set terminal background color: black */
    TERMINAL_FONT_GREEN(); /* set terminal display color: green */
    TERMINAL_DISPLAY_CLEAR();
    TERMINAL_RESET_CURSOR();

    PRINTF("---------------------------------------------\r\n\r\n");
    TERMINAL_HIGH_LIGHT();
    PRINTF("    CLI version: V0.31                       \r\n\r\n");
    PRINTF("    coder:       Cat                         \r\n\r\n");
    PRINTF("    github:      https://github.com/ShareCat \r\n\r\n");
    PRINTF("    Email:       843553493@qq.com            \r\n\r\n");
    TERMINAL_UN_HIGH_LIGHT();
    PRINTF("---------------------------------------------\r\n\r\n");

}


/**
  * @brief  handle commands whict from the terminal
  * @param  commands
  * @retval null
  */
static void CLI_RX_Handle(RX_BUFF_TYPE *rx_buff)
{
    static HANDLE_TYPE Handle = {.len = 0};
    uint8_t i;
    uint8_t ParaLen;
    uint8_t *ParaAddr;
    uint8_t cmd_match = False;

    /*  ---------------------------------------
        Step1: save the command
        ---------------------------------------
     */
    while(1) {

        if (Handle.len < HANDLE_LEN) { /* check the buffer */

            /* new char coming from the terminal£¬copy to Handle.buff */
            if (True == QueueOut((*rx_buff), Handle.buff[Handle.len])) {
                /* display char in  terminal */
                USART_SendData(DEBUG_USARTx, Handle.buff[Handle.len]);

                /* '\b' -->DELETE key from terminal */
                if (('\b' == Handle.buff[Handle.len]) && (0 < Handle.len)) {
                    /* buffer not empty */
                    if (0 < Handle.len) {
                        Handle.len -= 1;  /* length -1 */
                        /* terminal delete a char */
                        TERMINAL_CLEAR_END();
                    }

                } else {
                    Handle.len++;
                }

            } else {
                /* all chars copied to Handle.buff */
                break;
            }

        } else {
            /* buffer full */
            break;
        }
    }

    /*  ---------------------------------------
        Step2: handle the commands
        ---------------------------------------
     */
    if ((1 == Handle.len) && ('\r' == Handle.buff[Handle.len - 1])) {
        /* '\r' -->ENTER key from terminal */
        Handle.len = 0;
    } else if (1 < Handle.len) { /* check for the length of command */
        /* a command must ending by "\r" */
        if ('\r' == Handle.buff[Handle.len - 1]) {

            /* looking for a match */
            for (i = 0; i < sizeof(CLI_Cmd) / sizeof(COMMAND_TYPE); i++) {
                if (True == StrComp(Handle.buff, (void *)CLI_Cmd[i].pCmd)) {
                    cmd_match = True;
                    ParaLen = Handle.len - strlen(CLI_Cmd[i].pCmd);   /* para. length */
                    ParaAddr = &Handle.buff[strlen(CLI_Cmd[i].pCmd)]; /* para. address */

                    if (NULL != CLI_Cmd[i].pFun) {
                        /* func. schedule */
                        if (CLI_Cmd[i].pFun(ParaAddr, ParaLen)) {
                            /* func. scheduled successful */
                            PRINTF("\r\n-> OK\r\n");

                            /* ECHO */
                            if (ENABLE == CLI_EchoFlag) {
                                Handle.buff[Handle.len] = '\0';
                                PRINTF("[echo]: %s\r\n", (const char*)Handle.buff);
                            }
                        } else {
                            PRINTF("\r\n-> ERROR\r\n");
                            /* parameter wrong */
                            PRINTF(CLI_Cmd[i].pHelp);
                        }
                    } else {
                        /* func. is void */
                        PRINTF("\r\n-> FUN VOID\r\n");
                    }
                }
            }

            if (False == cmd_match) {
                /* no matching command */
                PRINTF("\r\n-> WRONG CMD\r\n\r\n");
            }

            Handle.len = 0;

        }

    }


    if (Handle.len >= HANDLE_LEN) {
        /* full, reset the buff */
        Handle.len = 0;
    }
}


/**
  * @brief  tx handle
  * @param  null
  * @retval null
  */
static void CLI_TX_Handle(void)
{
    /* not used for now */
}


/**
  * @brief  command line task, schedule by sys. every 50ms
  * @param  null
  * @retval null
  */
void CLI_Run(void)
{
    CLI_RX_Handle(&CLI_RX_Buff);
    CLI_TX_Handle();
}


