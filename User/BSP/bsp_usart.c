


#include "bsp_usart.h"
#include "../sys/sys_queue.h"
#include "../sys/sys_command_line.h"


/**
 * @brief  UART interrupt func.
 * @param  null
 * @retval null
 */
void DEBUG_USART_TX_IRQHandler(void)
{
    /* not used for now */
}


/**
 * @brief  UART RX interrupt func.
 * @param  null
 * @retval null
 */
void DEBUG_USART_RX_IRQHandler(void)
{
    uint8_t ucData;
    if(USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET) {
        ucData = USART_ReceiveData(DEBUG_USARTx);
        /* save char */
        QUEUE_IN(cli_rx_buff, ucData);
        //printf("%02x", ucTemp);
    }
}

/**
 * @brief  NVIC conf.
 * @param  null
 * @retval null
 */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* NVIC group */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO init.
 * @param  null
 * @retval null
 */
void debug_usart_init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
    DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

    // USART Tx GPIO conf.
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    // USART Rx GPIO conf
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // USART bandrate
    USART_InitStructure.USART_BaudRate = baud;
    // data len.
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    // stop bit
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    // check
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    // hardwear flow control
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    // RX & Tx
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(DEBUG_USARTx, &USART_InitStructure);
    NVIC_Configuration();

    // enable Rx interrupt
    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);

    // enable UART
    USART_Cmd(DEBUG_USARTx, ENABLE);
}

/*****************  send a char **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
    /* send a char */
    USART_SendData(pUSARTx,ch);

    /* wait */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/****************** send a array ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
    uint8_t i;

    for(i=0; i<num; i++) {
        /* send a char */
        Usart_SendByte(pUSARTx,array[i]);

    }
    /* wait */
    while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  send a string **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
    unsigned int k=0;
    do {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');

    /* wait */
    while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
    {}
}

/*****************  send half word **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
    uint8_t temp_h, temp_l;

    /* get higher byte */
    temp_h = (ch&0XFF00)>>8;
    /* get lower byte */
    temp_l = ch&0XFF;

    /* send higher byte */
    USART_SendData(pUSARTx,temp_h);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

    /* send lower byte */
    USART_SendData(pUSARTx,temp_l);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}


int fputc(int ch, FILE *f)
{
    /* send a char */
    USART_SendData(DEBUG_USARTx, (uint8_t) ch);

    /* wait */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);

    return (ch);
}


int fgetc(FILE *f)
{
    /* get a char */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(DEBUG_USARTx);
}

