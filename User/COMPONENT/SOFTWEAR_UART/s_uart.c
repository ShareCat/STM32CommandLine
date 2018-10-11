
#include "s_uart.h"

/******************************************************************************/
/********************** Private functions prototypes **************************/
/******************************************************************************/
static void S_GPIO_Set_Bit(GPIO_TypeDef* port, uint16_t pin);
static void S_GPIO_Clear_Bit(GPIO_TypeDef* port, uint16_t pin);
static uint8_t S_GPIO_Read_Bit(GPIO_TypeDef* port, uint16_t pin);
static void S_UART_periph_init(S_UART_init_struct* struct_init);
static void send_byte(soft_uart* s_uart, uint8_t byte);
static uint8_t majority_function(uint8_t* middle_sample);

/******************************************************************************/
/************************* Rx/Tx handlers *************************************/
/******************************************************************************/
__weak void S_UART_TC_Handler()
{
    /* NOTE : This function Should not be modified, when the handler is needed,
              the S_UART_TC_Handler could be implemented in the user file
     */
}

__weak void S_UART_RXNE_Handler()
{
    /* NOTE : This function Should not be modified, when the handler is needed,
              the S_UART_RXNE_Handler could be implemented in the user file
     */
}

/******************************************************************************/
/********************** Platform dependent functions **************************/
/******************************************************************************/
static void S_GPIO_Set_Bit(GPIO_TypeDef* port, uint16_t pin)
{
#ifdef HAL_LIB
    /* HAL peripheral lib realization */
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
#else
    /* Standart peripheral lib realization */
    GPIO_SetBits(port, pin);
#endif
}

static void S_GPIO_Clear_Bit(GPIO_TypeDef* port, uint16_t pin)
{
#ifdef HAL_LIB
    /* HAL peripheral lib realization */
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
#else
    /* Standart peripheral lib realization */
    GPIO_ResetBits(port, pin);
#endif
}

static uint8_t S_GPIO_Read_Bit(GPIO_TypeDef* port, uint16_t pin)
{
#ifdef HAL_LIB
    /* HAL peripheral lib realization */
    return HAL_GPIO_ReadPin(port, pin);
#else
    /* Standart peripheral lib realization */
    return GPIO_ReadInputDataBit(port, pin);
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
// checks the middle sample, previous and next sample in array!
static uint8_t majority_function(uint8_t* middle_sample)
{
    return ((*middle_sample) & (*(middle_sample - 1))) |
           ((*(middle_sample - 1)) & (*(middle_sample + 1))) |
           ((*middle_sample) & (*(middle_sample + 1)));
}
/******************************************************************************/
static void S_UART_periph_init(S_UART_init_struct* struct_init)
{
    struct_init->s_uart_port = S_UART_PORT;
    struct_init->s_uart_rx_pin = S_UART_RX_PIN;
    struct_init->s_uart_tx_pin = S_UART_TX_PIN;

#ifdef HAL_LIB
    /* HAL peripheral lib realization */
    TIM_HandleTypeDef s_uart_timer;

    s_uart_timer.Instance = S_UART_TIMER;
    s_uart_timer.Init.Prescaler = S_UART_PRESCALER;
    s_uart_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
    s_uart_timer.Init.Period = S_UART_TIMER_PERIOD;
    s_uart_timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if(HAL_TIM_Base_Init(&s_uart_timer) != HAL_OK) {
        Error_Handler();
    }

    HAL_TIM_Base_Start(&s_uart_timer);
    HAL_TIM_Base_Start_IT(&s_uart_timer);
#else
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /*
        step1：配置IO口模拟串口
    */
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* 使能GPIO的外设时钟 */
    RCC_AHBPeriphClockCmd(S_UART_GPIO_CLK, ENABLE);

    /* rx_pin */
    GPIO_InitStructure.GPIO_Pin = struct_init->s_uart_rx_pin;   /* 选择要控制的GPIO引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       /* 浮空输入 */
    GPIO_Init(struct_init->s_uart_port, &GPIO_InitStructure);   /* 调库函数，初始化GPIO */
    /* tx_pin */
    GPIO_InitStructure.GPIO_Pin = struct_init->s_uart_tx_pin;   /* 选择要控制的GPIO引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            /* 推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(struct_init->s_uart_port, &GPIO_InitStructure);   /* 调库函数，初始化GPIO */

    /*
        step2：配置定时器时间
    */
    // 开启定时器时钟,即内部时钟CK_INT=72M
    BASIC_TIM_APBxClock_FUN(S_UART_TIMER_CLOCK, ENABLE);

    // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = S_UART_TIMER_PERIOD;

    // 时钟预分频数为
    TIM_TimeBaseStructure.TIM_Prescaler = S_UART_PRESCALER;

    // 时钟分频因子 ，基本定时器没有，不用管
    //TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    // 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
    //TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;

    // 重复计数器的值，基本定时器没有，不用管
    //TIM_TimeBaseStructure.TIM_RepetitionCounter=0;

    // 初始化定时器
    TIM_TimeBaseInit(S_UART_TIMER, &TIM_TimeBaseStructure);

    // 清除计数器中断标志位
    TIM_ClearFlag(S_UART_TIMER, TIM_FLAG_Update);

    // 开启计数器中断
    TIM_ITConfig(S_UART_TIMER, TIM_IT_Update, ENABLE);

    // 使能计数器
    TIM_Cmd(S_UART_TIMER, ENABLE);

    /*
        step3：配置定时器中断
    */
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQ ;
    // 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}

/******************************************************************************/
static void send_byte(soft_uart* s_uart, uint8_t byte)
{
    s_uart->tx_bit_cnt = 0;
    s_uart->tx_sample_cnt = 0;
    s_uart->TDR = byte;
    S_CLEAR_FLAG(s_uart->SR, TXE_FLAG);
}

/******************************************************************************/
/*
 *
*/
void S_UART_init(soft_uart* s_uart)
{
    S_UART_periph_init(&(s_uart->peripheral));
    s_uart->rx_bit_cnt = 0;
    s_uart->rx_sample_cnt = 0;
    s_uart->tx_bit_cnt = 0;
    s_uart->tx_sample_cnt = 0;
    s_uart->TDR = 0;
    s_uart->RDR = 0;
    S_SET_FLAG(s_uart->SR, TXE_FLAG);
    S_CLEAR_FLAG(s_uart->SR, TC_FLAG);
    S_CLEAR_FLAG(s_uart->SR, RXNE_FLAG);
    S_CLEAR_FLAG(s_uart->SR, RX_START_BIT_FLAG);
    S_GPIO_Set_Bit(s_uart->peripheral.s_uart_port, s_uart->peripheral.s_uart_tx_pin);
}

/******************************************************************************/
/*
 *
*/
void S_UART_Timer_Handler(soft_uart* s_uart)
{
    /*** Transmitting part ***/
    if(((s_uart->SR) & (1 << TXE_FLAG)) == 0) {   // Means tx buffer is not empty
        if(s_uart->tx_sample_cnt < (S_UART_SAMPLES_PER_BIT - 1)) {
            if(s_uart->tx_bit_cnt == 0) { // means that we need to send a start bit
                S_GPIO_Clear_Bit(s_uart->peripheral.s_uart_port, s_uart->peripheral.s_uart_tx_pin);
            }

            s_uart->tx_sample_cnt++;

        } else {
            s_uart->tx_bit_cnt++;

            if(((s_uart->tx_bit_cnt) > 0) && ((s_uart->tx_bit_cnt) < (DATA_BIT_LEN - 1))) {  // sending a data byte
                if(((s_uart->TDR) & (1 << (s_uart->tx_bit_cnt - 1))) != 0) {
                    S_GPIO_Set_Bit(s_uart->peripheral.s_uart_port, s_uart->peripheral.s_uart_tx_pin);
                } else {
                    S_GPIO_Clear_Bit(s_uart->peripheral.s_uart_port, s_uart->peripheral.s_uart_tx_pin);
                }
            } else if(s_uart->tx_bit_cnt == (DATA_BIT_LEN - 1)) { // means that we need to send a stop bit
                S_GPIO_Set_Bit(s_uart->peripheral.s_uart_port, s_uart->peripheral.s_uart_tx_pin);
            } else if(s_uart->tx_bit_cnt == DATA_BIT_LEN) { // means that transmission of one byte is done
                S_SET_FLAG(s_uart->SR, TC_FLAG);
                S_SET_FLAG(s_uart->SR, TXE_FLAG);
                s_uart->tx_bit_cnt = 0;
            } else { // unknown counter value
                s_uart->tx_bit_cnt = 0;
                s_uart->tx_sample_cnt = 0;
                S_SET_FLAG(s_uart->SR, TXE_FLAG);
            }

            s_uart->tx_sample_cnt = 0;


        }
    }

    /*** Transmission complete part ***/
    if(((s_uart->SR) & (1 << TC_FLAG)) != 0) {   // Means that transmission of one byte has been completed
        S_UART_TC_Handler();
        S_CLEAR_FLAG(s_uart->SR, TC_FLAG); // Clear pending flag
    }

    /*** Receiving part ***/
    uint8_t received_sample = S_GPIO_Read_Bit(s_uart->peripheral.s_uart_port, s_uart->peripheral.s_uart_rx_pin);

    if(((s_uart->SR) & (1 << RX_START_BIT_FLAG)) == 0) {   // wait for start bit falling edge
        if(s_uart->rx_sample_cnt == 0) {
            if(received_sample == BIT_LOW) {
                s_uart->rx_samples_buff[s_uart->rx_sample_cnt] = received_sample;
                s_uart->rx_sample_cnt++;
            }
        } else if(s_uart->rx_sample_cnt < START_BIT_SAMPLES) {
            s_uart->rx_samples_buff[s_uart->rx_sample_cnt] = received_sample;
            s_uart->rx_sample_cnt++;
        } else if(s_uart->rx_sample_cnt == START_BIT_SAMPLES) { // Check with majority function
            if(majority_function(((s_uart->rx_samples_buff) + 1)) == S_UART_START_BIT) { // noise filter
                s_uart->rx_samples_buff[s_uart->rx_sample_cnt] = received_sample;
                s_uart->rx_sample_cnt++;
            } else { // noise
                s_uart->rx_sample_cnt = 0;
            }
        } else if(s_uart->rx_sample_cnt < (S_UART_SAMPLES_PER_BIT - 1)) {
            s_uart->rx_samples_buff[s_uart->rx_sample_cnt] = received_sample;
            s_uart->rx_sample_cnt++;
        } else if(s_uart->rx_sample_cnt == (S_UART_SAMPLES_PER_BIT - 1)) {
            if(majority_function(((s_uart->rx_samples_buff) + (S_UART_SAMPLES_PER_BIT - 1) / 2)) == S_UART_START_BIT) { // check in the middle of a bit
                S_SET_FLAG(s_uart->SR, RX_START_BIT_FLAG);
                s_uart->rx_sample_cnt = 0;
                s_uart->rx_bit_cnt = 1;
                s_uart->RDR = 0;
            } else {
                s_uart->rx_sample_cnt = 0;
            }
        } else { // unknown counter value
            s_uart->rx_sample_cnt = 0;
        }

    } else { // receive data
        uint8_t s_uart_rx_bit;

        if(s_uart->rx_sample_cnt < (S_UART_SAMPLES_PER_BIT - 1)) {
            s_uart->rx_samples_buff[s_uart->rx_sample_cnt] = received_sample;
            s_uart->rx_sample_cnt++;
        } else {
            if(majority_function(((s_uart->rx_samples_buff) + (S_UART_SAMPLES_PER_BIT - 1) / 2)) == 0) { // check in the middle of a bit
                /* 0 received */
                s_uart_rx_bit = BIT_LOW;
            } else {
                /* 1 received */
                s_uart_rx_bit = BIT_HIGH;
            }

            s_uart->rx_sample_cnt = 0;
            s_uart->rx_bit_cnt++;

            if((s_uart->rx_bit_cnt) < DATA_BIT_LEN) {
                if(s_uart_rx_bit == BIT_HIGH) {
                    S_SET_FLAG(s_uart->RDR, s_uart->rx_bit_cnt - 2); // shift 1 to data register
                } else {
                    S_CLEAR_FLAG(s_uart->RDR, s_uart->rx_bit_cnt - 2); // shift 0 to data register
                }
            } else if((s_uart->rx_bit_cnt) == DATA_BIT_LEN) { //stop bit required
                if(s_uart_rx_bit != BIT_HIGH) {
                    /* Capture error */
                } else {
                    S_SET_FLAG(s_uart->SR, RXNE_FLAG);
                }

                s_uart->rx_bit_cnt = 0;
                s_uart->rx_sample_cnt = 0;
                S_CLEAR_FLAG(s_uart->SR, RX_START_BIT_FLAG);
            }
        }
    }

    /*** Receive complete part ***/
    if(((s_uart->SR) & (1 << RXNE_FLAG)) != 0) {   // Means that transmission of one byte has been completed
        S_UART_RXNE_Handler();
        S_CLEAR_FLAG(s_uart->SR, RXNE_FLAG); // Clear pending flag
    }

#if 0   /* 用于调试，测量定时时间 */
    static uint8_t i = 0;

    if(i == 0) {
        S_GPIO_Set_Bit(s_uart->peripheral.s_uart_port, s_uart->peripheral.s_uart_tx_pin);
        i = 1;
    } else {
        S_GPIO_Clear_Bit(s_uart->peripheral.s_uart_port, s_uart->peripheral.s_uart_tx_pin);
        i = 0;
    }

#endif
}

/******************************************************************************/
uint8_t S_UART_SendData(soft_uart* s_uart, uint8_t data)
{
    if(((s_uart->SR) & (1 << TXE_FLAG)) != 0) {
        send_byte(s_uart, data);
        return 1;
    } else {
        /* Attempt to send something when the previous transmission is not completed yet */
        return 0;
    }
}
/******************************************************************************/
uint8_t S_UART_ReceiveData(soft_uart* s_uart, uint8_t* data)
{
    if(((s_uart->SR) & (1 << RXNE_FLAG)) != 0) {
        *data = s_uart->RDR;
        return 1;
    } else {
        /* Attempt to receive something when the data is not valid */
        return 0;
    }
}
/******************************************************************************/
