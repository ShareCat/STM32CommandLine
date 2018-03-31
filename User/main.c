


#include "stm32f10x.h"
#include "./sys/command_line.h"
#include "./bsp/bsp_usart.h"


void Delay(__IO uint32_t nCount)	 //delay
{
    for(; nCount != 0; nCount--);
}


/**
  * @brief  main
  * @param  null
  * @retval null
  */
int main(void)
{
    /*Command Line init, 115200 8-N-1 */
    CLI_INIT(CLI_BAUDRATE);

    while(1) {
        CLI_RUN();
        Delay(0xFFFF);
    }
}
/*********************************************END OF FILE**********************/
