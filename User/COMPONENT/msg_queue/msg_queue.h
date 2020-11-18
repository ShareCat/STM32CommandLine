/**
  ******************************************************************************
  * @file:      msg_queue.h
  * @author:    孙关平
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     消息队列
  * @attention:
  ******************************************************************************
  */


#ifndef __MSG_QUEUE_H_
#define __MSG_QUEUE_H_


#include "stdint.h"


// *********************

typedef struct {
    uint8_t  *buf_ptr;      /* 指向buff空间，必须是连续的，如buff[2][8]，表示2个buff，每个buff8个字节，那么初始化时候buf_size为2，buf_len为8 */
    volatile uint16_t rd;
    volatile uint16_t wr;
    uint16_t buf_size;      /* 消息队列中buff的个数 */
    uint16_t buf_len;       /* 消息队列中每个buff的字节数 */
} t_msg_queue;


// *********************
uint16_t msg_queue_get_size(t_msg_queue *p_m_q);
uint16_t msg_queue_get_used(t_msg_queue *p_m_q);
uint16_t msg_queue_get_free(t_msg_queue *p_m_q);
void msg_queue_clear_all_msg(t_msg_queue *p_m_q);
uint8_t msg_queue_get_msg(t_msg_queue *p_m_q, uint8_t *msg, uint16_t num);
uint8_t msg_queue_put_msg(t_msg_queue *p_m_q, uint8_t *msg, uint16_t num);
uint8_t msg_queue_put_msg_renew(t_msg_queue *p_m_q, uint8_t *msg, uint16_t num);
void msg_queue_init(t_msg_queue *p_m_q, const void *buffer, const uint16_t buffer_size, uint16_t buffer_len);

// *********************

#endif // ifndef __MSG_QUEUE_H_
