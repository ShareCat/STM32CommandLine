


#include "bsp_led.h"

/**
 * @brief  LED GPIO init
 * @param  null
 * @retval null
 */
void LED_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( LED1_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);

    LED1_OFF;
}
/*********************************************END OF FILE**********************/
