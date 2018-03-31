#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"


/************************************************************/
#define LED1_GPIO_PORT      	GPIOC			            /* GPIO port */
#define LED1_GPIO_CLK 	        RCC_APB2Periph_GPIOC		/* GPIO clock */
#define LED1_GPIO_PIN		    GPIO_Pin_13		            /* GPIO pin */

/************************************************************/


/* IO control */
#define	digitalHi(p,i)		    {p->BSRR=i;}	//output 1
#define digitalLo(p,i)		    {p->BRR=i;}	    //output 0
#define digitalToggle(p,i)      {p->ODR ^=i;}   //toggle

#define LED1_TOGGLE             digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_OFF                digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_ON                 digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN)


void LED_GPIO_Config(void);

#endif /* __LED_H */
