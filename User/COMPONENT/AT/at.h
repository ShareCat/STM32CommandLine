/**
  ******************************************************************************
  * @file:      at.h
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-5-9
  * @brief:     AT命令解析引擎
  * @attention:
  ******************************************************************************
  */


#ifndef __AT_H
#define __AT_H


#include "stdint.h"
#include "./../../sys/sys_scheduler.h"
#include "./../../sys/sys_command_line.h"
#include "./../../sys/sys_queue.h"


enum {
    AT_STAT_IDLE,                       /* 搜索AT帧头的状态 */
    AT_STAT_RECEIVE,                    /* 接收保存数据状态 */
    AT_STAT_PROCESS                     /* 将数据给对应的任务处理的状态 */
};


typedef struct {
    char* cmd_name;                     /* 命令 */
    uint8_t cmd_len;                    /* 命令长度 */
    void (*setup_func)(char *pPara);    /* 命令后面紧跟'='或'0'到'9' */
    void (*exe_func)(void);             /* 命令后面紧跟'\r' */
    void (*query_func)(void);           /* 命令后面紧跟'?\r' */
    void (*test_func)(void);            /* 命令后面紧跟'=?\r' */
} AT_CMD_S;


typedef struct AT_S {
#define CMD_MAX_LEN     32              /* 解析时候，临时缓存命令的buff大小，也
                                                就是支持最长命令字节数 */

    QUEUE32_S rx_buff;                  /* 接收队列缓存 */
    uint8_t sta : 7;                    /* 解析状态 */
    uint8_t receive_flag : 1;           /* 接收或空闲状态 */
    uint8_t* p_cmd;                     /* 解析时候用到的指针 */
    const AT_CMD_S* cmd_list;           /* 指向命令结构 */
    uint8_t cmd_max;                    /* 命令结构中命令个数 */
    void (*send_char)(uint8_t c);       /* 发送一个字符接口 */
    struct AT_S* next;
} AT_S;


extern void at_get_char(AT_S* p_at, uint8_t c);
extern void at_task(void);
extern uint8_t at_init(AT_S* p_at,
                       const AT_CMD_S* p_at_cmd_list,
                       uint8_t cmd_num,
                       void(*p_send_char)(uint8_t c));


#endif /* __AT_H */
