
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "msgprint.h"


U32 gprint_level;
U32 gprint_module;


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
    pmsg_str->head.type = MSG_TYPE_PRT;

    /* 将buf挂入队列 */
    send_msg_to_tx_task(&pmsg_buf->node);
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

