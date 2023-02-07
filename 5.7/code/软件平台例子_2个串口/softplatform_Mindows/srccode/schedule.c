
#include "schedule.h"


TASK_STR gsched_task;           /* sched任务结构 */


/***********************************************************************************
函数功能: 创建sched任务并进行初始化.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void create_sched_task(void)
{
    /* 创建sched_task任务 */
    gsched_task.ptcb = MDS_TaskCreate("sched_task", msg_sched_task, NULL, NULL,
                                      SCHED_TASK_STACK, SCHED_TASK_PRIO, NULL);
    if(NULL == gsched_task.ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\ncreate sched_task failed. (%s, %d)", __FILE__, __LINE__);
    }

    /* 初始化任务的队列 */
    if(NULL == (gsched_task.pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\nsched_task queue init failed. (%s, %d)", __FILE__, __LINE__);
    }

    print_msg(PRINT_SUGGEST, PRINT_SCHED, "\r\nsched_task init finished. (%s, %d)",
              __FILE__, __LINE__);
}

/***********************************************************************************
函数功能: 消息调度任务, 从任务调度池中申请一个空闲的消息处理任务处理消息.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void msg_sched_task(void* pvPara)
{
    while(1)
    {
        /* 调度消息 */
        msg_dispatch();
    }
}

/***********************************************************************************
函数功能: 将接收到的消息分发给不同的任务进行处理.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_dispatch(void)
{
    BUF_NODE* pbuf;

    /* 从队列获取要处理的消息 */
    pbuf = sched_task_receive_msg();
    if(NULL == pbuf)
    {
        return;
    }

    /* sched任务进行调度, 分配任务执行接收到的消息 */
    sched_task_schedule(pbuf);
}

/***********************************************************************************
函数功能: 向sched任务发送消息.
入口参数: pbuf: 发送消息的buf.
返 回 值: none.
***********************************************************************************/
void send_msg_to_sched_task(BUF_NODE* pbuf)
{
    /* 将消息放入队列 */
    if(RTN_SUCD != MDS_QuePut(gsched_task.pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\n%s put message to sched_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
函数功能: sched任务接收消息.
入口参数: none.
返 回 值: 接收到的消息的buf指针, 如果为NULL代表出错.
***********************************************************************************/
BUF_NODE* sched_task_receive_msg(void)
{
    M_DLIST* pbuf;

    /* 从队列中获取消息 */
    if(RTN_SUCD != MDS_QueGet(gsched_task.pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\n%s get message from sched_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
函数功能: sched任务调度, 分配任务.
入口参数: pbuf: 需要调度的消息的buf指针.
返 回 值: none.
***********************************************************************************/
void sched_task_schedule(BUF_NODE* pbuf)
{
    /* **********用户可以根据自身需要修改该函数********** */

    TASK_STR* ptask;

    /* 从任务池中申请可用的任务 */
    ptask = process_task_malloc();
    if(NULL == ptask)
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\ncan't malloc process_task, free message buf 0x%x. (%s, %d)",
                  pbuf, __FILE__, __LINE__);

        goto RTN;
    }

    /* 将消息转发给process任务 */
    send_msg_to_process_task(ptask, pbuf);

    return;

    /* 出错返回, 释放存放接收消息的缓冲 */
RTN:

    buf_free(pbuf);
}

