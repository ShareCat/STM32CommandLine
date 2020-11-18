/**
  ******************************************************************************
  * @file:      msg_queue.c
  * @author:    孙关平
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     消息队列
  * @attention:
  ******************************************************************************
  */


#include <string.h>

#include "./msg_queue.h"

// *****************************************************************************

uint16_t msg_queue_get_size(t_msg_queue *p_m_q)
{
    // return the usable size of the buffer
    uint16_t buf_size = p_m_q->buf_size;

    if (buf_size > 0) {
        return buf_size - 1;
    } else {
        return 0;
    }
}

uint16_t msg_queue_get_used(t_msg_queue *p_m_q)
{
    // return the number of bytes available in the rx buffer
    uint16_t rd = p_m_q->rd;
    uint16_t wr = p_m_q->wr;
    uint16_t buf_size  = p_m_q->buf_size;

    uint16_t num_bytes = wr - rd;

    if (wr < rd) {
        num_bytes = (buf_size - rd) + wr;
    }

    return num_bytes;
}

uint16_t msg_queue_get_free(t_msg_queue *p_m_q)
{
    // return the free space size in the buffer
    uint16_t buf_size  = p_m_q->buf_size;

    uint16_t num_bytes = msg_queue_get_used(p_m_q);

    return (buf_size - num_bytes) - 1;
}

void msg_queue_clear_all_msg(t_msg_queue *p_m_q)
{
    // remove all data from the buffer
    p_m_q->rd = p_m_q->wr;
}

uint8_t msg_queue_get_msg(t_msg_queue *p_m_q, uint8_t *msg, uint16_t num)
{
    // get msg from the buffer
    uint16_t rd        = p_m_q->rd;
    uint16_t buf_size  = p_m_q->buf_size;
    uint8_t *buff      = p_m_q->buf_ptr;

    if (num > p_m_q->buf_len) return 0;

    // get number of msg available
    uint16_t num_bytes = msg_queue_get_used(p_m_q);

    if (num_bytes < 1) {
        return 0; // no msg returned
    }

    memcpy(msg, &buff[rd * p_m_q->buf_len], num);

    if (++rd >= buf_size) {
        rd = 0;
    }

    p_m_q->rd = rd;

    return 1;
}

uint8_t msg_queue_put_msg(t_msg_queue *p_m_q, uint8_t *msg, uint16_t num)
{
    // add a data byte to the buffer
    uint16_t wr        = p_m_q->wr;
    uint16_t buf_size  = p_m_q->buf_size;
    uint8_t *buff      = p_m_q->buf_ptr;

    if (num > p_m_q->buf_len) return 0;

    uint16_t num_bytes = msg_queue_get_free(p_m_q);

    if (num_bytes < 1) {
        return 0;
    }

    memcpy(&buff[wr * p_m_q->buf_len], msg, num);

    if (++wr >= buf_size) {
        wr = 0;
    }

    p_m_q->wr = wr;

    return 1;
}

/* msg_queue_put_msg_renew接口：如果消息队列满了，就覆盖最老的一个 */
uint8_t msg_queue_put_msg_renew(t_msg_queue *p_m_q, uint8_t *msg, uint16_t num)
{
    // add a data byte to the buffer
    uint16_t wr        = p_m_q->wr;
    uint16_t rd        = p_m_q->rd;
    uint16_t buf_size  = p_m_q->buf_size;
    uint8_t *buff      = p_m_q->buf_ptr;

    if (num > p_m_q->buf_len) return 0;

    uint16_t num_bytes = msg_queue_get_free(p_m_q);

    if (num_bytes < 1) {
        /* 满了，就覆盖最老的一个 */
        memcpy(&buff[wr * p_m_q->buf_len], msg, num);

        if (++wr >= buf_size) {
            wr = 0;
        }
        if (++rd >= buf_size) {
            rd = 0;
        }

        p_m_q->wr = wr;
        p_m_q->rd = rd;
    } else {
        /* 没满，就正常写入 */
        memcpy(&buff[wr * p_m_q->buf_len], msg, num);

        if (++wr >= buf_size) {
            wr = 0;
        }

        p_m_q->wr = wr;
    }

    

    return 1;
}

void msg_queue_init(t_msg_queue *p_m_q, const void *buffer, const uint16_t buffer_size, uint16_t buffer_len)
{
    p_m_q->buf_ptr  = (uint8_t *)buffer;
    p_m_q->rd = 0;
    p_m_q->wr = 0;
    p_m_q->buf_size = buffer_size;
    p_m_q->buf_len = buffer_len;
}

// *****************************************************************************
