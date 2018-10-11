#ifndef __S_UART_H_
#define __S_UART_H_

/******************************************************************************/
/****************************** Includes  *************************************/
/******************************************************************************/
#include "stdint.h"


/******************************************************************************/
/******************************* Common deifnes *******************************/
/******************************************************************************/

//#define HAL_LIB                     1    /* 是否使用硬件抽象库 */

#ifdef HAL_LIB
    #include "stm32f2xx_hal.h"
#else
    #include "stm32f10x.h"
#endif


#define SOFTWARE_UART               1

/* 波特率不要选太高，选择1200bps时候定时器定时为52us，因为sampling rate must be at least 16 times per baud */
#define S_BAUDERATE                 1200    /* max 9600 */

#ifdef SOFTWARE_UART
    //    #define S_UART_GPIO_CLK         RCC_APB2Periph_GPIOA
    //    #define S_UART_RX_PIN           GPIO_Pin_10
    //    #define S_UART_TX_PIN           GPIO_Pin_9
    //    #define S_UART_PORT             GPIOA

    #define S_UART_GPIO_CLK         RCC_APB2Periph_GPIOB
    #define S_UART_RX_PIN           GPIO_Pin_7
    #define S_UART_TX_PIN           GPIO_Pin_6
    #define S_UART_PORT             GPIOB

    #define BASIC_TIM_APBxClock_FUN RCC_APB1PeriphClockCmd
    #define S_UART_TIMER            TIM7
    #define S_UART_TIMER_CLOCK      RCC_APB1Periph_TIM7
    #define BASIC_TIM_IRQ           TIM7_IRQn
    #define BASIC_TIM_IRQHandler    TIM7_IRQHandler
    #define S_UART_TIMER_CLK        72000000
    #define S_UART_PRESCALER        0
    /*
    *  According to the standard, sampling rate must be at least 16 times per baud
    */
    #define S_UART_SAMPLES_PER_BIT  16
    #define S_UART_TIMER_PERIOD     S_UART_TIMER_CLK/(S_BAUDERATE*S_UART_SAMPLES_PER_BIT)
#endif

#define S_SET_FLAG(reg, pos)        reg = reg | (1<<pos)
#define S_CLEAR_FLAG(reg, pos)      reg = reg & (~(1<<pos))

/******************************************************************************/
/************************* Software UART code part ****************************/
/******************************************************************************/
#define BIT_HIGH                    1
#define BIT_LOW                     0

#define TXE_FLAG                    0
#define TC_FLAG                     1
#define RXNE_FLAG                   2
#define RX_START_BIT_FLAG           3

#define BIT_LEN                     8
#define DATA_BIT_LEN                10 // 8 bit data + start + stop bit
#define START_BIT_SAMPLES           3  // check first 3 samples when detect start bit edge

#define S_UART_START_BIT            0
#define S_UART_STOP_BIT             1

typedef struct {
    uint16_t s_uart_rx_pin;
    uint16_t s_uart_tx_pin;
    GPIO_TypeDef* s_uart_port;
    //TIM_TypeDef* s_uart_timer;
    //uint16_t s_uart_timer_period;
} S_UART_init_struct;

typedef struct {
    S_UART_init_struct peripheral;
    uint8_t rx_samples_buff[S_UART_SAMPLES_PER_BIT];
    uint8_t rx_bit_cnt;
    uint8_t tx_bit_cnt;
    uint8_t rx_sample_cnt;
    uint8_t tx_sample_cnt;
    uint8_t SR;
    uint8_t TDR;
    uint8_t RDR;
} soft_uart;

/******************************************************************************/
/***************** Software UART public function prototypes *******************/
/******************************************************************************/
void S_UART_init(soft_uart* s_uart);
void S_UART_Timer_Handler(soft_uart* s_uart);
uint8_t S_UART_SendData(soft_uart* s_uart, uint8_t data);
uint8_t S_UART_ReceiveData(soft_uart* s_uart, uint8_t* data);

/******************************************************************************/
#endif //__S_UART_H_
