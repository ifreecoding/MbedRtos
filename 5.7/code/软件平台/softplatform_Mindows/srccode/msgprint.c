
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "msgprint.h"


U32 gprint_level;
U32 gprint_module;


TASK_STR gprint_task;           /* print任务结构 */


/***********************************************************************************
函数功能: 创建print任务并进行初始化.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void create_print_task(void)
{
    /* 初始化打印开关, 关闭打印功能, 防止print任务正常输出的打印陷入死循环 */
    print_init();

    /* 创建print_task任务 */
    gprint_task.ptcb = MDS_TaskCreate("print_task", msg_print_task, NULL, NULL,
                                      PRINT_TASK_STACK, PRINT_TASK_PRIO, NULL);
    if(NULL == gprint_task.ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_PRINT,
                  "\r\ncreate print_task failed. (%s, %d)", __FILE__, __LINE__);
    }

    /* 初始化任务的队列 */
    if(NULL == (gprint_task.pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_PRINT,
                  "\r\nprint_task queue init failed. (%s, %d)", __FILE__, __LINE__);
    }

    print_msg(PRINT_SUGGEST, PRINT_PRINT, "\r\nprint_task init finished. (%s, %d)",
              __FILE__, __LINE__);
}

/***********************************************************************************
函数功能: 打印消息任务, 将要打印缓冲中的消息打印到串口.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void msg_print_task(void* pvPara)
{
    while(1)
    {
        /* 打印数据 */
        msg_print();
    }
}

/***********************************************************************************
函数功能: 打印消息.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_print(void)
{
    BUF_NODE* pbuf;

    /* 从队列获取要处理的消息 */
    pbuf = print_task_receive_msg();
    if(NULL == pbuf)
    {
        return;
    }

    /* 打印接收到的消息 */
    print_task_send_data(pbuf);
}

/***********************************************************************************
函数功能: 向print任务发送消息.
入口参数: pbuf: 发送消息的buf.
返 回 值: none.
***********************************************************************************/
void send_msg_to_print_task(BUF_NODE* pbuf)
{
    /* 将消息放入队列 */
    if(RTN_SUCD != MDS_QuePut(gprint_task.pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_PRINT,
                  "\r\n%s put message to print_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
函数功能: print任务接收消息.
入口参数: none.
返 回 值: 接收到的消息的buf指针, 如果为NULL代表出错.
***********************************************************************************/
BUF_NODE* print_task_receive_msg(void)
{
    M_DLIST* pbuf;

    /* 从队列中获取消息 */
    if(RTN_SUCD != MDS_QueGet(gprint_task.pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_PRINT,
                  "\r\n%s get message from print_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
函数功能: 初始化打印开关.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void print_init(void)
{
    print_level_set(PRINT_ALL);
    print_module_set(PRINT_ALL);
}

/***********************************************************************************
函数功能: 打印消息函数, 只有此级别和此模块的消息开关被打开时才能打印该消息.
入口参数: level: 打印消息的级别.
                 PRINT_ALL: 全部.
                 PRINT_SUGGEST: 提示.
                 PRINT_NORMAL: 一般.
                 PRINT_IMPORTANT: 重要.
          module: 打印消息的模块.
                  PRINT_ALL: 全部.
                  PRINT_BUF: buf模块.
                  PRINT_COM: 通信模块.
                  PRINT_TASK: 任务模块.
                  PRINT_RX: 接收模块.
                  PRINT_TX: 发送模块.
                  PRINT_SCHED: 调度模块.
                  PRINT_PROCESS: 处理模块.
                  PRINT_PRINT: 打印模块.
          fmt: 第一个参数的指针, 必须为字符串指针.
          ...: 其它参数.
返 回 值: none.
***********************************************************************************/
void print_msg(U32 level, U32 module, U8* fmt, ...)
{
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    va_list args;

    /* 不符合打印条件则直接返回 */
    if(!((0 != (level & gprint_level)) && (0 != (module & gprint_module))))
    {
        return;
    }

    /* 申请buf, 用来存放需要打印的字符 */
    pmsg_buf = (MSG_BUF*)buf_malloc_without_print(PRINT_BUF_LEN);
    if(NULL == pmsg_buf)
    {
        //print_msg(PRINT_IMPORTANT, PRINT_PRINT, "\r\ncan't malloc buf. (%s, %d)",
        //          __FILE__, __LINE__);

        return;
    }

    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    /* 将字符串打印到内存 */
    va_start(args, fmt);
    (void)vsprintf(pmsg_str->buf, fmt, args);
    va_end(args);

    /* 填充维护层参数 */
    pmsg_str->head.len = strlen(pmsg_str->buf);

    /* 将buf挂入队列 */
    send_msg_to_print_task(&pmsg_buf->node);
}

/***********************************************************************************
函数功能: print任务发送接收到的消息.
入口参数: pbuf: 存储打印消息的缓冲.
返 回 值: none.
***********************************************************************************/
void print_task_send_data(BUF_NODE* pbuf)
{
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    U32 sent_len;
    U32 send_len;

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    sent_len = 0;
    send_len = pmsg_str->head.len;

    /* 采用查询的方式发送数据 */
    while(sent_len < send_len)
    {
        /* 等待串口可发送数据 */
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        {
            ;
        }

        /* 向串口发一个字符 */
        USART_SendData(USART2, pmsg_str->buf[sent_len++]);
    }

    /* 释放打印消息 */
    buf_free_without_print(pbuf);
}

/***********************************************************************************
函数功能: 设置打印消息级别开关, 只有此级别的消息开关被打开时才能打印此级别的消息.
入口参数: level: 设置打印消息的级别.
                 PRINT_ALL: 全部.
                 PRINT_SUGGEST: 提示.
                 PRINT_NORMAL: 一般.
                 PRINT_IMPORTANT: 重要.
返 回 值: none.
***********************************************************************************/
void print_level_set(U32 level)
{
    gprint_level |= level;
}

/***********************************************************************************
函数功能: 清除打印消息级别开关, 只有此级别的消息开关被打开时才能打印此级别的消息.
入口参数: level: 清除打印消息的级别.
                 PRINT_ALL: 全部.
                 PRINT_SUGGEST: 提示.
                 PRINT_NORMAL: 一般.
                 PRINT_IMPORTANT: 重要.
返 回 值: none.
***********************************************************************************/
void print_level_clr(U32 level)
{
    gprint_level &= ~level;
}

/***********************************************************************************
函数功能: 获取打印消息级别开关, 只有此级别的消息开关被打开时才能打印此级别的消息.
入口参数: none.
返 回 值: 清除打印消息的级别.
          PRINT_ALL: 全部.
          PRINT_SUGGEST: 提示.
          PRINT_NORMAL: 一般.
          PRINT_IMPORTANT: 重要.
***********************************************************************************/
U32 print_level_get(void)
{
    return gprint_level;
}

/***********************************************************************************
函数功能: 设置打印消息模块开关, 只有此模块的消息开关被打开时才能打印此模块的消息.
入口参数: module: 设置打印消息的模块.
                  PRINT_ALL: 全部.
                  PRINT_BUF: buf模块.
                  PRINT_COM: 通信模块.
                  PRINT_TASK: 任务模块.
                  PRINT_RX: 接收模块.
                  PRINT_TX: 发送模块.
                  PRINT_SCHED: 调度模块.
                  PRINT_PROCESS: 处理模块.
                  PRINT_PRINT: 打印模块.
返 回 值: none.
***********************************************************************************/
void print_module_set(U32 module)
{
    gprint_module |= module;
}

/***********************************************************************************
函数功能: 清除打印消息模块开关, 只有此模块的消息开关被打开时才能打印此模块的消息.
入口参数: module: 清除打印消息的模块.
                  PRINT_ALL: 全部.
                  PRINT_BUF: buf模块.
                  PRINT_COM: 通信模块.
                  PRINT_TASK: 任务模块.
                  PRINT_RX: 接收模块.
                  PRINT_TX: 发送模块.
                  PRINT_SCHED: 调度模块.
                  PRINT_PROCESS: 处理模块.
                  PRINT_PRINT: 打印模块.
返 回 值: none.
***********************************************************************************/
void print_module_clr(U32 module)
{
    gprint_module &= ~module;
}

/***********************************************************************************
函数功能: 获取打印消息模块开关, 只有此模块的消息开关被打开时才能打印此模块的消息.
入口参数: none.
返 回 值: 清除打印消息的级别.
          PRINT_ALL: 全部.
          PRINT_BUF: buf模块.
          PRINT_COM: 通信模块.
          PRINT_TASK: 任务模块.
          PRINT_RX: 接收模块.
          PRINT_TX: 发送模块.
          PRINT_SCHED: 调度模块.
          PRINT_PROCESS: 处理模块.
          PRINT_PRINT: 打印模块.
***********************************************************************************/
U32 print_module_get(void)
{
    return gprint_module;
}

