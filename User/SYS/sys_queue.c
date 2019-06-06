/**
  ******************************************************************************
  * @file:      sys_queue.c
  * @author:    Cat
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     queue
  * @attention:
  ******************************************************************************
  */


#include "sys_queue.h"


/**
 * @brief  queue_init
 * @param  Front , Rear , PBase , Len
 * @retval True
 */
uint8_t queue_init(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len)
{
    uint16_t index;

    for(index = 0; index < Len; index++) {
        PBase[index] = NULL;
    }

    *Front = *Rear = 0;
    return TRUE;
}


/**
 * @brief  queue_full
 * @param  Front , Rear , PBase , Len
 * @retval Result of Queue Operation as Enum
 */
uint8_t queue_full(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len)
{
    if((((*Rear) + 1) % Len) == *Front) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @brief  queue_empty
 * @param  Front , Rear , PBase , Len
 * @retval Result of Queue Operation as Enum
 */
uint8_t queue_empty(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len)
{
    if(*Front == *Rear) {
        return TRUE;
    } else {
        return FALSE;
    }
}


/**
 * @brief  queue_in
 * @param  Front , Rear , PBase , Len
 * @retval Result of Queue Operation as Enum
 */
uint8_t queue_in(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData)
{
    //DISABLE_ALL_IRQ();

    if(queue_full(Front, Rear, PBase, Len)) {
        return FALSE;
    }

    PBase[*Rear] = *PData;
    *Rear = ((*Rear) + 1) % Len;

    //ENABLE_ALL_IRQ();

    return TRUE;
}


/**
 * @brief  queue_out
 * @param  Front , Rear , PBase , Len
 * @retval Result of Queue Operation as Enum
 */

uint8_t queue_out(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData)
{
    //DISABLE_ALL_IRQ();

    if(queue_empty(Front, Rear, PBase, Len)) {
        return FALSE;
    }

    *PData = PBase[*Front];
    *Front = ((*Front) + 1) % Len;

    //ENABLE_ALL_IRQ();

    return TRUE;
}


