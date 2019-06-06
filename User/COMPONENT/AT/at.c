/**
  ******************************************************************************
  * @file:      at.c
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-5-9
  * @brief:     AT命令解析引擎
  * @attention:
  ******************************************************************************
  */


#include "./at.h"


static AT_S* head_at = 0;


#define AT_OK(p_at)     at_send_pack(p_at, "ok\r\n", sizeof("ok\r\n"))
#define AT_ERR(p_at)    at_send_pack(p_at, "err\r\n", sizeof("err\r\n"))
#define AT_BUSY(p_at)   at_send_pack(p_at, "busy\r\n", sizeof("busy\r\n"))


/**
  * @brief  发送一个字节，用于回复AT发送方
  * @param  p_at 命令对象
  * @param  c 要发送的数据
  * @retval null
  */
static void at_send_char(AT_S* p_at, uint8_t c)
{
    p_at->send_char(c);
}


/**
  * @brief  发送一个包，用于回复AT发送方
  * @param  p_at 命令对象
  * @param  data 要发送的数据
  * @param  num 要发送的个数
  * @retval null
  */
static void at_send_pack(AT_S* p_at, uint8_t* data, uint16_t num)
{
    uint16_t i;

    for(i = 0; i < num; i++) {
        at_send_char(p_at, data[i]);
    }
}


/**
  * @brief  获取一个字节，命令来源的地方，收到命令后调用保存起来
  * @param  p_at 命令对象
  * @param  c 收到的一个字节，需要保存起来
  * @retval null
  */
void at_get_char(AT_S* p_at, uint8_t c)
{
    if(p_at->sta != AT_STAT_PROCESS) {
        /* 入队列 */
        QUEUE_IN(p_at->rx_buff, c);
        /* 标记正在接收 */
        p_at->receive_flag = TRUE;
    } else {
        AT_BUSY(p_at);  /* 正在解析，回复忙 */
    }
}


/**
  * @brief  获取收到的命令长度，不包括"AT"
  * @param  p_cmd 命令指针
  * @retval 返回命令长度，返回-1表示错误
  */
static int8_t at_get_cmd_len(uint8_t* p_cmd)
{
    uint8_t n, i;

    n = 0;
    i = CMD_MAX_LEN;

    while(i--) {
        if((*p_cmd == '\r')
                || (*p_cmd == '=')
                || (*p_cmd == '?')
                || ((*p_cmd >= '0') && (*p_cmd <= '9'))) {
            /* 返回命令长度 */
            return n;
        } else {
            p_cmd++;
            n++;
        }
    }

    return -1;
}


/**
  * @brief  寻找匹配的命令，并执行对应的函数
  * @param  p_at 命令对象
  * @retval null
  */
static void at_cmd_match(AT_S* p_at)
{
    uint8_t error = FALSE;
    int8_t len;
    uint8_t i;
    int16_t id = -1;

    /*
        第一步：计算用户输入的命令的长度
    */
    len = at_get_cmd_len(p_at->p_cmd);
    PRINTF("%s, len = %d\r\n", p_at->p_cmd, len);

    if(len == -1) return;

    /*
        第二步：寻找匹配的命令
    */
    for(i = 0; i < p_at->cmd_max; i++) {
        if(p_at->cmd_list[i].cmd_len == len) {
            if(0 == memcmp(p_at->p_cmd, p_at->cmd_list[i].cmd_name, len)) {
                id = i;
                break;  /* 成功找到匹配的命令，不必再往下寻找了 */
            }
        }
    }

    if(id == -1) {      /* 如果命令没有找到 */
        AT_ERR(p_at);
        return;
    }

    /*
        第三步：执行命令对应的函数
    */
    p_at->p_cmd += len;

    if((p_at->p_cmd[0] >= '0') && (p_at->p_cmd[0] <= '9') || (p_at->p_cmd[0] == '=')) {
        if(p_at->cmd_list[id].setup_func) {    /* 命令支持这个函数 */
            p_at->cmd_list[id].setup_func((char*)p_at->p_cmd);
        } else {                                /* 命令不支持这个函数 */
            error = TRUE;
        }
    } else if(p_at->p_cmd[0] == '\r') {
        if(p_at->cmd_list[id].exe_func) {
            p_at->cmd_list[id].exe_func();
        } else {
            error = TRUE;
        }
    } else if((p_at->p_cmd[0] == '?') && (p_at->p_cmd[1] == '\r')) {
        if(p_at->cmd_list[id].query_func) {
            p_at->cmd_list[id].query_func();
        } else {
            error = TRUE;
        }
    } else if((p_at->p_cmd[0] == '=') && (p_at->p_cmd[1] == '?')  && (p_at->p_cmd[2] == '\r')) {
        if(p_at->cmd_list[id].test_func) {
            p_at->cmd_list[id].test_func();
        } else {
            error = TRUE;
        }
    } else {
        error = TRUE;
    }

    if(error == TRUE) {
        AT_ERR(p_at);
    } else {
        AT_OK(p_at);
    }
}


/**
  * @brief  对接收缓存数据处理，寻找AT或at头部
  * @param  p_at 命令对象
  * @retval null
  */
static void at_process(AT_S* p_at)
{
    uint8_t temp;
    uint8_t atHead[2];
    uint8_t cmd_line[CMD_MAX_LEN];

    if(p_at->receive_flag == TRUE) {    /* 如果还在接收，则先不着急解析 */
        p_at->receive_flag = FALSE;
        return;
    }

    /*
        第一步：寻找"AT"或"at"
    */
    if(p_at->sta == AT_STAT_IDLE) {
        while(QUEUE_OUT(p_at->rx_buff, temp)) {
            atHead[0] = atHead[1];
            atHead[1] = temp;

            if((memcmp(atHead, "AT", 2) == 0) || (memcmp(atHead, "at", 2) == 0)) {
                p_at->p_cmd = cmd_line;
                p_at->sta = AT_STAT_RECEIVE;
                break;  /* 成功找到AT，跳出while循环 */
            } else if(temp == '\n') {
                /* 只收到回车 */
                AT_ERR(p_at);
            }
        }
    }

    /*
        第二步：如果成功找到"AT"或"at"，开始保存命令并解析，在解析过程中收到任何
        另外命令都不响应
    */
    if(p_at->sta == AT_STAT_RECEIVE) {
        while(QUEUE_OUT(p_at->rx_buff, temp)) {
            *p_at->p_cmd = temp;

            if(temp == '\n') {
                p_at->p_cmd++;
                *p_at->p_cmd = '\0';
                p_at->p_cmd = cmd_line;    /* 重新指向首地址 */
                p_at->sta = AT_STAT_PROCESS;
                break;  /* 收到回车，跳出while循环 */
            } else if(p_at->p_cmd >= &cmd_line[CMD_MAX_LEN - 1]) {  /* 临时缓存满了 */
                p_at->sta = AT_STAT_IDLE;
                break;  /* 命令太长，跳出while循环 */
            }

            p_at->p_cmd++;
        }

        if(p_at->sta == AT_STAT_PROCESS) {
            at_cmd_match(p_at);
        }

        p_at->sta = AT_STAT_IDLE;
    }
}

/**
  * @brief  AT注册，可以注册多个解析任务
  * @param  p_at AT对象
  * @param  p_at_cmd_list AT命令列表
  * @param  cmd_num AT命令个数
  * @param  p_send_char 发送一个字符接口，用于回复AT发送方
  * @retval 0表示成功，1表示已经注册过了
  */
uint8_t at_init(AT_S* p_at,
                const AT_CMD_S* p_at_cmd_list,
                uint8_t cmd_num,
                void(*p_send_char)(uint8_t c))
{
    AT_S* target = head_at;

    p_at->rx_buff.Front = 0;
    p_at->rx_buff.Rear = 0;
    p_at->sta = AT_STAT_IDLE;
    p_at->receive_flag = FALSE;
    p_at->send_char = p_send_char;
    p_at->cmd_list = p_at_cmd_list;
    p_at->cmd_max = cmd_num;

    while(target) {
        if(target == p_at) return 1; //already exist.

        target = target->next;
    }

    p_at->next = head_at;
    head_at = p_at;
    return 0;
}

/**
  * @brief  AT命令解析任务，由调度器20ms调度一次
  * @param  null
  * @retval null
  */
void at_task(void)
{
    AT_S* target;

    for(target = head_at; target; target = target->next) {
        at_process(target);
    }
}


/*********************************************END OF FILE**********************/
